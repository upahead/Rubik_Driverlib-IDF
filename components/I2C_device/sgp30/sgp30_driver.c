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
  * @file           sgp30_driver.c
  * @version        1.0
  * @date           2021-5-22
  */

#include "sgp30_driver.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "../i2c_master/include/i2c_master.h"

static const char *TAG = "SGP30";

#define SGP30_HANDLE_CHECK(a, ret)  if (NULL == a) {                             \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

/**
  * @brief  Initialize the SGP30 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                       Under normal circumstances it is 0X58.
  * @retval  
  *         successful  sgp30 operation handle.
  *         failed      NULL.
  * @note  Use SGP30_Deinit() to release it.
  */
SGP30_handle_t SGP30_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr)
{
    esp_err_t err = ESP_OK;
    uint8_t cmd_buf[2] = {0};

    if(NULL == i2c_handle){
        ESP_LOGE(TAG, "%s (%d) i2c handle is not initialized.", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (i2c_handle->i2c_clk > 400000) {
        ESP_LOGE(TAG, "%s (%d) SGP30 I2C supports up to 400Kbit/s", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (false == I2CMaster_CheckDeviceAlive(i2c_handle, i2c_addr)) {
        ESP_LOGE(TAG, "%s (%d) device not alive.", __FUNCTION__, __LINE__);
        return NULL;
    }

    SGP30_handle_t sgp30_handle = malloc(sizeof(SGP30_t));
    if (NULL == sgp30_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    sgp30_handle->i2c_handle = i2c_handle;
    sgp30_handle->i2c_addr = i2c_addr;

    // Send a soft reset command.
    cmd_buf[0] = SGP30_SOFT_RESET_CMD;
    err = I2cMaster_WriteData(i2c_handle, i2c_addr, cmd_buf, 1);
    if (ESP_OK != err) {
        ESP_LOGE(TAG, "%s (%d) soft reset failed.", __FUNCTION__, __LINE__);
        goto SGP30_INIT_FAILED;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Send initialize the air quality measurement command.
    cmd_buf[0] = (uint8_t)(SGP30_INIT_AIR_QUALITY>>8);
    cmd_buf[1] = (uint8_t)SGP30_INIT_AIR_QUALITY;
    err = I2cMaster_WriteData(i2c_handle, i2c_addr, cmd_buf, 2);
    if (ESP_OK != err) {
        ESP_LOGE(TAG, "%s (%d) send init cmd failed.", __FUNCTION__, __LINE__);
        goto SGP30_INIT_FAILED;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "%s (%d) sgp30 init ok.", __FUNCTION__, __LINE__);
    return sgp30_handle;

SGP30_INIT_FAILED:
    free(sgp30_handle);
    return NULL;
}

/**
  * @brief  SGP30 deinitialization.
  * @param[in]  sgp30_handle  sgp30 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t SGP30_Deinit(SGP30_handle_t* sgp30_handle)
{
    SGP30_HANDLE_CHECK(*sgp30_handle, ESP_FAIL);

    free(*sgp30_handle);
    *sgp30_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) sgp30 handle deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

// CRC
static uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t  remainder = 0;
    uint8_t  i = 0, j = 0;

    remainder = initial_value;
    for (j=0; j<2; j++) {
        remainder ^= message[j];
        for (i=0; i<8; i++) {
            if (remainder & 0x80) {
                remainder = (remainder << 1)^SGP30_CRC8_POLYNOMIAL;
            } else {
                remainder = (remainder << 1);
            }
        }
    }
    return remainder;
}

/**
  * @brief  SGP30 send statrt messure command.
  * @param[in]  sgp30_handle  sgp30 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note  Send the start measurement command and wait for 1s before data collection.
  */
esp_err_t SGP30_StartMessure(SGP30_handle_t sgp30_handle)
{
    esp_err_t err = ESP_OK;
    uint8_t cmd_buf[2] = {0};

    cmd_buf[0] = (uint8_t)(SGP30_MEASURE_AIR_QUALITY>>8);
    cmd_buf[1] = (uint8_t)SGP30_MEASURE_AIR_QUALITY;
    err = I2cMaster_WriteData(sgp30_handle->i2c_handle, sgp30_handle->i2c_addr, 
                              cmd_buf, 2);
    if (ESP_OK != err) {
        ESP_LOGE(TAG, "%s (%d) send messure cmd failed.", __FUNCTION__, __LINE__);
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
  * @brief  SGP30 get co2 and tvoc value.
  * @param[in]  sgp30_handle  sgp30 operation handle pointer.
  * @param[out]  co2_val  co2 value.
  * @param[out]  tvoc_val  TVOC value.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t SGP30_GetValue(SGP30_handle_t sgp30_handle, uint16_t *co2_val, uint16_t *tvoc_val)
{
    esp_err_t err = ESP_OK;
    uint8_t recv_buf[6] = {0};

    SGP30_HANDLE_CHECK(sgp30_handle, ESP_FAIL);
    if (NULL == co2_val || NULL == tvoc_val) {
        return ESP_FAIL;
    }

    err = I2cMaster_ReadData(sgp30_handle->i2c_handle, sgp30_handle->i2c_addr, 
                             recv_buf, 6);
    if (err != ESP_OK) {
        return ESP_FAIL;
    }
    if (CheckCrc8(&recv_buf[0], 0xFF) != recv_buf[2]) {
        return ESP_FAIL;
    }  
    if (CheckCrc8(&recv_buf[3], 0xFF) != recv_buf[5]) {
        return ESP_FAIL;
    }
    *co2_val  = recv_buf[0] << 8 | recv_buf[1];
    *tvoc_val = recv_buf[3] << 8 | recv_buf[4];

    return ESP_OK;
}
