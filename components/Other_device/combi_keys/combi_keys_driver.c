/*****************************************************************************
 *                                                                           *
 *  Copyright 2021 upahead PTE LTD                                           *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *      http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/
/**
  * @file           combi_keys_driver.c
  * @version        1.0
  * @date           2021-6-1
  */

#include "combi_keys_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

static const char *TAG = "Combi keys";

#define COMBIKEYS_HANDLE_CHECK(a, ret)  if (NULL == a) {                         \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

/**
  * @brief  Combination key initialization.
  * @param[in]  key_up  up key number.
  * @param[in]  key_mid  mid key number.
  * @param[in]  key_down  down key number.
  * @retval  
  *         successful  CombiKeys operation handle.
  *         failed      NULL.
  * @note  Use CombiKeys_Deinit() to release it.
  * @note  The default long press time is 2000ms.
  */
CombiKey_handle_t CombiKeys_Init(gpio_num_t key_up, gpio_num_t key_mid, gpio_num_t key_down)
{
    esp_err_t err = ESP_FAIL;

    CombiKey_handle_t combi_keys_handle = malloc(sizeof(CombiKey_t));
    if (NULL == combi_keys_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    gpio_pad_select_gpio(key_up);
    err = gpio_set_direction(key_up, GPIO_MODE_INPUT);
    if(ESP_OK != err) {
        goto COMBI_KEYS_CONFIG_FAIL;
    }
    gpio_pad_select_gpio(key_mid);
    err = gpio_set_direction(key_mid, GPIO_MODE_INPUT);
    if (ESP_OK != err) {
        goto COMBI_KEYS_CONFIG_FAIL;
    }
    gpio_pad_select_gpio(key_down);
    err = gpio_set_direction(key_down, GPIO_MODE_INPUT);
    if (ESP_OK != err) {
        goto COMBI_KEYS_CONFIG_FAIL;
    }
    
    combi_keys_handle->key_up = key_up;
    combi_keys_handle->key_mid = key_mid;
    combi_keys_handle->key_down = key_down;
    // The long press time defaults to 2000ms.
    combi_keys_handle->long_press_time = 2000;
    ESP_LOGI(TAG, "%s (%d) Combi keys init ok.", __FUNCTION__, __LINE__);
    return combi_keys_handle;

COMBI_KEYS_CONFIG_FAIL:
    ESP_LOGE(TAG, "%s (%d) Config combi keys IO failed.", __FUNCTION__, __LINE__);
    free(combi_keys_handle);
    return NULL;
}

/**
  * @brief  combi keys deinitialization.
  * @param[in]  combi_keys_handle  combi keys operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t CombiKeys_Deinit(CombiKey_handle_t* combi_keys_handle)
{
    COMBIKEYS_HANDLE_CHECK(*combi_keys_handle, ESP_FAIL);

    // reset gpio.
    gpio_reset_pin((*combi_keys_handle)->key_up);
    gpio_reset_pin((*combi_keys_handle)->key_mid);
    gpio_reset_pin((*combi_keys_handle)->key_down);

    free(*combi_keys_handle);
    *combi_keys_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) combi keys handle deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

/**
  * @brief  combi keys Set long press time.
  * @param[in]  combi_keys_handle  combi keys combi keys operation handle.
  * @param[in]  time_ms  long press time(ms).
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note   The default long press time is 2000ms.
  */
esp_err_t CombiKeys_SetLongPress(CombiKey_handle_t combi_keys_handle, uint32_t time_ms)
{

    COMBIKEYS_HANDLE_CHECK(combi_keys_handle, ESP_FAIL);
    if (0 == time_ms) {
        ESP_LOGE(TAG, "%s (%d) long press time cannot be set to 0", __FUNCTION__, __LINE__);
        return ESP_FAIL;
    }

    combi_keys_handle->long_press_time = time_ms;
    return ESP_OK;
}

/**
  * @brief  Get the current system time(us)
  * @retval 
  *         system time(us)
  */
static int64_t combi_get_current_time(void)
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    return (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
}

/**
  * @brief  Get the key combination of keys
  * @param[in]  combi_keys_handle  combi keys combi keys operation handle.
  * @retval 
  *         - key value combi_Key_Value_t.
  * @note  
  *         Only one can support the detection of one button at a time.
  *         Support three states: press, short press and long press.
  */
CombiKey_Value_t CombiKeys_GetValue(CombiKey_handle_t combi_keys_handle)
{
    static uint8_t current_press = COMBI_KEY_NONE_VALUE_PRESS;
    static int64_t begin_time = 0, end_time = 0;
    
    // Check if there is a button pressed, if so, keep the current system time, and mark the button pressed.
    if(current_press == COMBI_KEY_NONE_VALUE_PRESS){
        if(0 == gpio_get_level(combi_keys_handle->key_up)){
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if(0 == gpio_get_level(combi_keys_handle->key_up)){
                current_press = COMBI_KEY_UP_VALUE_PRESS;
                begin_time = combi_get_current_time();
                return COMBI_KEY_UP_VALUE_PRESS;
            } 
        }else if(0 == gpio_get_level(combi_keys_handle->key_mid)){
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if(0 == gpio_get_level(combi_keys_handle->key_mid)){
                current_press = COMBI_KEY_MID_VALUE_PRESS;
                begin_time = combi_get_current_time();
                return COMBI_KEY_MID_VALUE_PRESS;
            }
        }else if(0 == gpio_get_level(combi_keys_handle->key_down)){
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if(0 == gpio_get_level(combi_keys_handle->key_down)){
                current_press = COMBI_KEY_DOUN_VALUE_PRESS;
                begin_time = combi_get_current_time();
                return COMBI_KEY_DOUN_VALUE_PRESS;
            }
        }else{
            return COMBI_KEY_NONE_VALUE_PRESS;
        }
    }else{ // If a button is marked as pressed, judge whether the button is released, if released, judge whether to press long or short according to the system time.
        if(current_press == COMBI_KEY_UP_VALUE_PRESS){
            if(1 == gpio_get_level(combi_keys_handle->key_up)){
                end_time = combi_get_current_time();
                if((end_time-begin_time) > (combi_keys_handle->long_press_time)*1000){
                    begin_time = 0;
                    current_press = COMBI_KEY_NONE_VALUE_PRESS;
                    return COMBI_KEY_UP_VALUE_LONG_PRESS;
                }else{
                    begin_time = 0;
                    current_press = COMBI_KEY_NONE_VALUE_PRESS;
                    return COMBI_KEY_UP_VALUE_SHORT_PRESS;
                }
            }
            return COMBI_KEY_UP_VALUE_PRESS;
        }else if(current_press == COMBI_KEY_MID_VALUE_PRESS){
            if(1 == gpio_get_level(combi_keys_handle->key_mid)){
                end_time = combi_get_current_time();
                if((end_time-begin_time) > (combi_keys_handle->long_press_time)*1000){
                    begin_time = 0;
                    current_press = COMBI_KEY_NONE_VALUE_PRESS;
                    return COMBI_KEY_MID_VALUE_LONG_PRESS;
                }else{
                    begin_time = 0;
                    current_press = COMBI_KEY_NONE_VALUE_PRESS;
                    return COMBI_KEY_MID_VALUE_SHORT_PRESS;
                }
            }
            return COMBI_KEY_MID_VALUE_PRESS;
        }else if(current_press == COMBI_KEY_DOUN_VALUE_PRESS){
            if(1 == gpio_get_level(combi_keys_handle->key_down)){
                end_time = combi_get_current_time();
                if((end_time-begin_time) > (combi_keys_handle->long_press_time)*1000){
                    begin_time = 0;
                    current_press = COMBI_KEY_NONE_VALUE_PRESS;
                    return COMBI_KEY_DOUN_VALUE_LONG_PRESS;
                }else{
                    begin_time = 0;
                    current_press = COMBI_KEY_NONE_VALUE_PRESS;
                    return COMBI_KEY_DOUN_VALUE_SHORT_PRESS;
                }
            }
            return COMBI_KEY_DOUN_VALUE_PRESS;
        }
    }
    return  COMBI_KEY_NONE_VALUE_PRESS;
}
