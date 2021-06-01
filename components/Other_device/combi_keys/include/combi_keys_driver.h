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
  * @file           combi_keys_driver.h
  * @version        1.0
  * @date           2021-6-1
  */

#ifndef __COMBI_KEYS_DRIVER_H_
#define __COMBI_KEYS_DRIVER_H_

#include "driver/gpio.h"
#include "esp_err.h"

// Combination key key value enumeration.
typedef enum{
    COMBI_KEY_NONE_VALUE_PRESS,
    COMBI_KEY_UP_VALUE_PRESS,
    COMBI_KEY_UP_VALUE_SHORT_PRESS,
    COMBI_KEY_UP_VALUE_LONG_PRESS,
    COMBI_KEY_MID_VALUE_PRESS,
    COMBI_KEY_MID_VALUE_SHORT_PRESS,
    COMBI_KEY_MID_VALUE_LONG_PRESS,
    COMBI_KEY_DOUN_VALUE_PRESS,
    COMBI_KEY_DOUN_VALUE_SHORT_PRESS,
    COMBI_KEY_DOUN_VALUE_LONG_PRESS,
    COMBI_KEY_VALUE_MAX,
}CombiKey_Value_t;

typedef struct{
    gpio_num_t key_up;
    gpio_num_t key_mid;
    gpio_num_t key_down;
    uint32_t long_press_time;    // ms
}CombiKey_t;
typedef CombiKey_t *CombiKey_handle_t;
    
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
CombiKey_handle_t CombiKeys_Init(gpio_num_t key_up, gpio_num_t key_mid, gpio_num_t key_down);

/**
  * @brief  combi keys deinitialization.
  * @param[in]  combi_keys_handle  combi keys operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t CombiKeys_Deinit(CombiKey_handle_t* combi_keys_handle);

/**
  * @brief  combi keys Set long press time.
  * @param[in]  combi_keys_handle  combi keys combi keys operation handle.
  * @param[in]  time_ms  long press time(ms).
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note   The default long press time is 2000ms.
  */
esp_err_t CombiKeys_SetLongPress(CombiKey_handle_t combi_keys_handle, uint32_t time_ms);

/**
  * @brief  Get the key combination of keys
  * @param[in]  combi_keys_handle  combi keys combi keys operation handle.
  * @retval 
  *         - key value combi_Key_Value_t.
  * @note  
  *         Only one can support the detection of one button at a time.
  *         Support three states: press, short press and long press.
  */
CombiKey_Value_t CombiKeys_GetValue(CombiKey_handle_t combi_keys_handle);

#endif /* __COMBINING_KEYS_DRIVER_H_ */