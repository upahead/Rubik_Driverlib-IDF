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
  * @file           rghled_driver.c
  * @version        1.0
  * @date           2021-5-24
  */

#include "rgbled_driver.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/rmt.h"

const char TAG[] = "rgbled";

#define RGBLED_HANDLE_CHECK(a, ret)  if (NULL == a) {                            \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

static RGBled_DeviceSeq_t sk6812_device_seq = RGBLED_DEVICE_SK6812_SEQ_CONFIG;
static RGBled_DeviceSeq_t ws2812_device_seq = RGBLED_DEVICE_WS2812_SEQ_CONFIG;

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
                            uint32_t led_len, rmt_channel_t rmt_channel)
{
    esp_err_t err = ESP_OK;

    if (device_type < 0 || device_type >= RGBLED_DEVICE_TYPE_MAX) {
        ESP_LOGE(TAG, "%s (%d) device not supported.", __FUNCTION__, __LINE__);
        return NULL;
    }

    RGBled_handle_t rgb_handle = malloc(sizeof(RGBled_t));
    if (NULL == rgb_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }

    rmt_config_t config = {
        .rmt_mode = RMT_MODE_TX,		
        .channel = rmt_channel,	
        .gpio_num = io_num,			
        .mem_block_num = 3,				
        .tx_config.loop_en = false,
        .tx_config.carrier_en = false,
        .tx_config.idle_output_en = true,
        .tx_config.idle_level = 0,
        .clk_div = RGBLED_RMT_CLK_DIV		
    };
    ESP_ERROR_CHECK(rmt_config(&config));
    err = rmt_driver_install(config.channel, 0, 0);
    if (ESP_OK != err) {
        free(rgb_handle);
        return NULL;
    }

    rgb_handle->device_type = device_type;
    rgb_handle->io_num = io_num;
    rgb_handle->led_len = led_len;
    rgb_handle->rmt_channel = rmt_channel;
    ESP_LOGI(TAG, "%s (%d) rgbled init ok.", __FUNCTION__, __LINE__);
    return rgb_handle;
}

/**
  * @brief  RGBled deinitialization.
  * @param[in]  rgb_handle  RGBled operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note  uinstall rmt device.
  */
esp_err_t RGBled_Deinit(RGBled_handle_t* rgb_handle)
{
    esp_err_t err = ESP_OK;
    RGBLED_HANDLE_CHECK(*rgb_handle, ESP_FAIL);

    err = rmt_driver_uninstall((*rgb_handle)->rmt_channel);
    if (ESP_OK != err) {
        return ESP_FAIL;
    }

    free(*rgb_handle);
    *rgb_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) rgbled deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

/**
  * @brief  Obtain a color data in RGB888 format.
  * @param[in]  red  red(0~255).
  * @param[in]  green  green(0~255).
  * @param[in]  blue  blue(0~255).
  * @retval  RGB888 format color data.
  */
RGBled_Color_t RGBled_GetRgb888(uint8_t red, uint8_t green, uint8_t blue)
{
    return (RGBled_Color_t)((red << 16) + (green << 8) + (blue));
}

/**
  * @brief  Convert RGB888 to GRB888.
  * @param[in]  rgb  RGB88 format data.
  * @retval  GRB888 format data.
  * @note   The data format used by SK6812 and WS2812 is GRB 24bit.
  */
static inline uint32_t rgb_to_grb(RGBled_Color_t rgb)
{
    uint8_t red = rgb>>16 & 0xFF; 
    uint8_t green = rgb>>8 & 0xFF;
    uint8_t blue = rgb & 0xFF;
    return (uint32_t)((green << 16) + (red << 8) + (blue));
}

/**
  * @brief  RGBled send color data.
  * @param[in]  rgb_handle  RGBled operation handle.
  * @param[in]  led_color  GRB888 format color data.
  * @retval void 
  * @note   The data length depends on the led_len.
  */
static void RGBled_SendColorData(RGBled_handle_t rgb_handle, int* led_color)
{   
    uint32_t item_num = rgb_handle->led_len * RGBLED_BITS_PER_LED_CMD;

    rmt_item32_t* led_data_buffer = malloc(item_num * sizeof(rmt_item32_t)); 
    if(NULL == led_data_buffer) {
        return;
    }

    // Select different timing Settings depending on the device.
    RGBled_DeviceSeq_t* rgb_seq;
    if (rgb_handle->device_type == RGBLED_DEVICE_TYPE_SK6812) {
        rgb_seq = &sk6812_device_seq;
    } else {
        rgb_seq = &ws2812_device_seq;
    }

    rmt_item32_t bit0 = {{{rgb_seq->T0H, 1, rgb_seq->T0L, 0}}};  
    rmt_item32_t bit1 = {{{rgb_seq->T1H, 1, rgb_seq->T1L, 0}}};
    for (uint32_t led=0; led<rgb_handle->led_len; led++) {
        uint32_t bits_to_send = led_color[led];
        uint32_t mask = 1 << (RGBLED_BITS_PER_LED_CMD - 1);
        for (uint32_t bit=0; bit<RGBLED_BITS_PER_LED_CMD; bit++) {
            uint32_t bit_is_set = bits_to_send & mask;
            led_data_buffer[led*RGBLED_BITS_PER_LED_CMD+bit] = bit_is_set ? bit1 : bit0;
            mask >>= 1;
        }
    }
    ESP_ERROR_CHECK(rmt_write_items(rgb_handle->rmt_channel, led_data_buffer, item_num, false));
    ESP_ERROR_CHECK(rmt_wait_tx_done(rgb_handle->rmt_channel, portMAX_DELAY));
    free(led_data_buffer);
}

/**
  * @brief  RGBled Sets the color of all LEDs.
  * @param[in]  rgb_handle  RGBled operation handle.
  * @param[in]  color_lis  RGB color array.
  *             The length of the array needs to be the same as the length of the LED.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t RGBled_SetAllColor(RGBled_handle_t rgb_handle, RGBled_Color_t *color_list)
{
    RGBLED_HANDLE_CHECK(rgb_handle, ESP_FAIL);
    if (NULL == color_list) {
        return ESP_FAIL;
    }

    int* pixels = malloc(sizeof(int) * rgb_handle->led_len);
    if (NULL == pixels) {
        return ESP_FAIL;
    }
        
    // Convert RGB888 to GRB888.
    for (uint32_t i=0; i<rgb_handle->led_len; i++) {
        pixels[i] = rgb_to_grb(color_list[i]);
    }
    RGBled_SendColorData(rgb_handle, pixels);
    free(pixels);
    return ESP_OK;
}

/**
  * @brief  RGBled Set all lights to one color.
  * @param[in]  rgb_handle  RGBled operation handle.
  * @param[in]  color_data  RGB888 color data.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t RGBled_SetAllOneColor(RGBled_handle_t rgb_handle, RGBled_Color_t color_data)
{
    RGBLED_HANDLE_CHECK(rgb_handle, ESP_FAIL);

    int* pixels = malloc(sizeof(int) * rgb_handle->led_len);
    if (NULL == pixels) {
        return ESP_FAIL;
    }
        
    // Convert RGB888 to GRB888.
    for (uint32_t i=0; i<rgb_handle->led_len; i++) {
        pixels[i] = rgb_to_grb(color_data);
    }
    RGBled_SendColorData(rgb_handle, pixels);
    free(pixels);
    return ESP_OK;
}

/**
  * @brief  RGBled Turn off all LEDs.
  * @param[in]  rgb_handle  RGBled operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t RGBled_SetAllOff(RGBled_handle_t rgb_handle)
{
    RGBLED_HANDLE_CHECK(rgb_handle, ESP_FAIL);
    RGBled_SetAllOneColor(rgb_handle, 0x00);
    return ESP_OK;
}
