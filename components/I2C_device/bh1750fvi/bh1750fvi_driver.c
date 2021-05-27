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
  * @file           bh1750fvi_driver.c
  * @version        1.0
  * @date           2021-5-27
  */

#include "bh1750fvi_driver.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "BH1750FVI";

#define BH1750FVI_HANDLE_CHECK(a, ret)  if (NULL == a) {                         \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

/**
  * @brief  Initialize the bh1750fvi and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                       
  * @retval  
  *         successful  bh1750fvi operation handle.
  *         failed      NULL.
  * @note  Use BH1750FVI_Deinit() to release it.
  */
BH1750FVI_handle_t BH1750FVI_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr)
{
    if (NULL == i2c_handle) {
        ESP_LOGE(TAG, "%s (%d) i2c handle is not initialized.", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (i2c_handle->i2c_clk > 400000) {
        ESP_LOGE(TAG, "%s (%d) BH1750FVI I2C supports up to 400Kbit/s", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (false == I2CMaster_CheckDeviceAlive(i2c_handle, i2c_addr)) {
        ESP_LOGE(TAG, "%s (%d) device not alive.", __FUNCTION__, __LINE__);
        return NULL;
    }

    BH1750FVI_handle_t bh1750fvi_handle = malloc(sizeof(BH1750FVI_t));
    if (NULL == bh1750fvi_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    bh1750fvi_handle->i2c_handle = i2c_handle;
    bh1750fvi_handle->i2c_addr = i2c_addr;

    ESP_LOGI(TAG, "%s (%d) bh1750fvi init ok.", __FUNCTION__, __LINE__);
    return bh1750fvi_handle;
}

/**
  * @brief  BH1750FVI deinitialization.
  * @param[in]  bh1750fvi_handle  bh1750fvi operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BH1750FVI_Deinit(BH1750FVI_handle_t* bh1750fvi_handle)
{
    BH1750FVI_HANDLE_CHECK(*bh1750fvi_handle, ESP_FAIL);

    free(*bh1750fvi_handle);
    *bh1750fvi_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) bh1750fvi handle deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

/**
  * @brief  BH1750FVI Get data once.
  * @param[in]  sgp30_handle  bh1750fvi operation handle.
  * @param[out]  brightness  brightness data, (0-65535), unit: lx
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note This process requires a small 200ms.
  */
esp_err_t BH750FVI_GetData(BH1750FVI_handle_t bh1750fvi_handle, float* brightness)
{
    uint8_t read_buf[2] = {0};
    uint8_t send_buf = 0x00;
    uint32_t dis_data = 0;               

    BH1750FVI_HANDLE_CHECK(bh1750fvi_handle, ESP_FAIL);

    send_buf = BH1750FVI_POWER_ON;
    I2cMaster_WriteData(bh1750fvi_handle->i2c_handle, bh1750fvi_handle->i2c_addr, &send_buf, 1);
    send_buf = BH1750FVI_CONTI_H_MODE;
    I2cMaster_WriteData(bh1750fvi_handle->i2c_handle, bh1750fvi_handle->i2c_addr, &send_buf, 1);

    vTaskDelay(200 / portTICK_PERIOD_MS);
    I2cMaster_ReadData(bh1750fvi_handle->i2c_handle, bh1750fvi_handle->i2c_addr, read_buf, 2);

    dis_data = read_buf[0];
    dis_data = (dis_data<<8)+read_buf[1];
    *brightness = (float)dis_data/1.2;                           // Get the integer part
    *brightness += (float)((int)(10*dis_data/1.2)%10)/10;        // Get the fractional part
    return ESP_OK;
}