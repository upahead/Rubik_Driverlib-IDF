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
  * @file           rghled_show.c
  * @version        1.0
  * @date           2021-5-24
  */

#include "rgbled_show.h"
#include "rgbled_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

/**
  * @brief  RGBled RGB Gradient.
  * @param[in]  rgb_handle RGBled operation handle.
  * @param[in]  time  Intervals time. (Fastest is 1)
  * @retval void
  * @note   It is recommended to use a separate task to call this function. 
  *         In order to be compatible with task scheduling, it is not recommended 
  *         to set a too low gradient time, which may affect the operation of 
  *         other tasks.
  */
void RGBled_ShowGradient(RGBled_handle_t rgb_handle, uint32_t time)
{
    uint8_t i=0;
    uint8_t j=0;
    uint8_t k=255;

    for (i=0; i<255; i++) {
        k--;
        RGBled_SetAllOneColor(rgb_handle, RGBled_GetRgb888(i, j, k));
        vTaskDelay(time / portTICK_PERIOD_MS);
    }
    for (j=0; j<255; j++) {
        i--;
        RGBled_SetAllOneColor(rgb_handle, RGBled_GetRgb888(i, j, k));
        vTaskDelay(time / portTICK_PERIOD_MS);
    }
    for (k=0; k<255; k++) {
        j--;
        RGBled_SetAllOneColor(rgb_handle, RGBled_GetRgb888(i, j, k));
        vTaskDelay(time / portTICK_PERIOD_MS);
    }
}

/**
  * @brief  RGBled Color bar scroll.
  * @param[in]  rgb_handle RGBled operation handle.
  * @param[in]  color_list Color data array, the same length as the number of lights.
  * @param[in]  delay_time Time between each scroll.
  * @retval void
  */
void RGBled_ShowColorRoll(RGBled_handle_t rgb_handle, RGBled_Color_t* color_list, uint32_t delay_time)
{
    uint32_t led_len = rgb_handle->led_len;
    uint32_t color_num = 0;

    RGBled_Color_t* color_buf = malloc(sizeof(RGBled_Color_t) * rgb_handle->led_len);
    if (NULL == color_buf) {
        return;
    }
        
    for (uint32_t i=0; i<led_len; i++) {
        for (uint32_t j=0; j<led_len; j++) {
            color_buf[j] = color_list[(j+color_num) % led_len];
        }
        RGBled_SetAllColor(rgb_handle, color_buf);
        color_num++;
        vTaskDelay(delay_time / portTICK_PERIOD_MS);
    }
    free(color_buf);
}

/**
  * @brief  RGBled Color fill enter.
  * @param[in]  rgb_handle  RGBled operation handle.
  * @param[in]  color  RGB888 color data.
  * @param[in]  delay_time  Filling interval time.
  * @retval void
  */
void RGBled_ShowFillIn(RGBled_handle_t rgb_handle, RGBled_Color_t color, uint32_t delay_time)
{
    uint32_t led_len = rgb_handle->led_len;

    RGBled_Color_t* color_buf = malloc(sizeof(RGBled_Color_t) * rgb_handle->led_len);
    if (NULL == color_buf) {
        return;
    }
    
    for (uint32_t i=0; i<led_len; i++) {
        for (uint32_t j=0; j<(i+1); j++) {
            color_buf[j] = color;
        }
        RGBled_SetAllColor(rgb_handle, color_buf);
        vTaskDelay(delay_time / portTICK_PERIOD_MS);
    }
    free(color_buf);
}