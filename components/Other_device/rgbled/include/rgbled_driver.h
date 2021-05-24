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
  * @file           rghled_driver.h
  * @version        1.0
  * @date           2021-5-24
  */

#ifndef _RGBLED_DRIVER_H_
#define _RGBLED_DRIVER_H_

#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/rmt.h"

#define RGBLED_RMT_CLK_DIV              2
#define RGBLED_BITS_PER_LED_CMD         24 

// Supported device model.
typedef enum{
    RGBLED_DEVICE_TYPE_SK6812,
    RGBLED_DEVICE_TYPE_WS2812,
    RGBLED_DEVICE_TYPE_MAX,
}RGBled_DeviceType_t;

// Communication timing control.
typedef struct{
    uint8_t T0H;
    uint8_t T1H;
    uint8_t T0L;
    uint8_t T1L;
    uint32_t TRES;
}RGBled_DeviceSeq_t;

// SK6812
#define RGBLED_DEVICE_SK6812_SEQ_CONFIG { \
    .T0H = 13, \
    .T1H = 37, \
    .T0L = 25, \
    .T1L = 25, \
    .TRES = 3200 \
};

// WS2812
#define RGBLED_DEVICE_WS2812_SEQ_CONFIG { \
    .T0H = 16, \
    .T1H = 34, \
    .T0L = 34, \
    .T1L = 16, \
    .TRES = 14000 \
};

// RGB888 format.
typedef uint32_t RGBled_Color_t;       
 
typedef struct{
    RGBled_DeviceType_t device_type;
    gpio_num_t io_num;
    uint32_t led_len;
    rmt_channel_t rmt_channel;
}RGBled_t;
typedef RGBled_t *RGBled_handle_t;

/**
  * @brief  Initialization RGB LED.
  * @param[in]  device_type  Supported device model. (See RGBled_DeviceType_t)
  * @param[in]  io_num  IO port used by RGB LED.
  * @param[in]  led_len  Number of LEDs.
  * @param[in]  rmt_channel  rmt channel used.
  * @retval  
  *         successful  RGBled operation handle.
  *         failed      NULL.
  * @note  Use RGBled_Deinit() to release it.
  */
RGBled_handle_t RGBled_Init(RGBled_DeviceType_t device_type, gpio_num_t io_num, 
                            uint32_t led_len, rmt_channel_t rmt_channel);

/**
  * @brief  RGBled deinitialization.
  * @param[in]  rgb_handle  RGBled operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note  uinstall rmt device.
  */
esp_err_t RGBled_Deinit(RGBled_handle_t* rgb_handle);

/**
  * @brief  Obtain a color data in RGB888 format.
  * @param[in]  red  red(0~255).
  * @param[in]  green  green(0~255).
  * @param[in]  blue  blue(0~255).
  * @retval  RGB888 format color data.
  */
RGBled_Color_t RGBled_GetRgb888(uint8_t red, uint8_t green, uint8_t blue);

/**
  * @brief  RGBled Sets the color of all LEDs.
  * @param[in]  rgb_handle  RGBled operation handle.
  * @param[in]  color_lis  RGB color array.
  *             The length of the array needs to be the same as the length of the LED.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t RGBled_SetAllColor(RGBled_handle_t rgb_handle, RGBled_Color_t *color_list);

/**
  * @brief  RGBled Set all lights to one color.
  * @param[in]  rgb_handle  RGBled operation handle.
  * @param[in]  color_data  RGB888 color data.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t RGBled_SetAllOneColor(RGBled_handle_t rgb_handle, RGBled_Color_t color_data);

/**
  * @brief  RGBled Turn off all LEDs.
  * @param[in]  rgb_handle  RGBled operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t RGBled_SetAllOff(RGBled_handle_t rgb_handle);

#endif /* _RGBLED_DRIVER_H_ */