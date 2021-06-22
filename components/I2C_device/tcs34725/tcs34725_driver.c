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
  * @file           tcs34725_driver.c
  * @version        1.0
  * @date           2021-6-22
  */

#include "tcs34725_driver.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "../i2c_master/include/i2c_master.h"

static const char *TAG = "TCS34725";

#define TCS34725_HANDLE_CHECK(a, ret)  if (NULL == a) {                          \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

/**
  * @brief  Initialize the TCS34725 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                       Under normal circumstances it is 0x29.
  * @retval  
  *         successful  tcs34725 operation handle.
  *         failed      NULL.
  * @note  Initially set the integral time to 240ms and the gain to 4X.
  * @note  Use TCS34725_Deinit() to release it.
  */
TCS34725_handle_t TCS34725_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr)
{
    esp_err_t err = ESP_OK;

    if (NULL == i2c_handle) {
        ESP_LOGE(TAG, "%s (%d) i2c handle is not initialized.", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (i2c_handle->i2c_clk > 400000) {
        ESP_LOGE(TAG, "%s (%d) TCS34725 I2C supports up to 400Kbit/s", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (false == I2CMaster_CheckDeviceAlive(i2c_handle, i2c_addr)) {
        ESP_LOGE(TAG, "%s (%d) device not alive.", __FUNCTION__, __LINE__);
        return NULL;
    }

    TCS34725_handle_t tcs34725_handle = malloc(sizeof(TCS34725_t));
    if (NULL == tcs34725_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    tcs34725_handle->i2c_handle = i2c_handle;
    tcs34725_handle->i2c_addr = i2c_addr;

    // Initialize settings integration and gain.
    err = TCS34725_SetIntegrationTime(tcs34725_handle, TCS34725_INTEGRATIONTIME_240MS);
    if (ESP_OK != err) {
        goto TCS34725_INIT_FAILED;
    }
    err = TCS34725_SetGain(tcs34725_handle, TCS34725_GAIN_4X);
    if (ESP_OK != err) {
        goto TCS34725_INIT_FAILED;
    }

    ESP_LOGI(TAG, "%s (%d) tcs34725 init ok.", __FUNCTION__, __LINE__);
    return tcs34725_handle;

TCS34725_INIT_FAILED:
    free(tcs34725_handle);
    return NULL;
}

/**
  * @brief  TCS34725 deinitialization.
  * @param[in]  tcs34725_handle  tcs34725 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t TCS34725_Deinit(TCS34725_handle_t* tcs34725_handle)
{
    TCS34725_HANDLE_CHECK(*tcs34725_handle, ESP_FAIL);

    free(*tcs34725_handle);
    *tcs34725_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) tcs34725 handle deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

/**
  * @brief  TCS34725 Set integration time.
  * @param[in]  tcs34725_handle  tcs34725 operation handle.
  * @param[in]  i_time  integratiion time config.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t TCS34725_SetIntegrationTime(TCS34725_handle_t tcs34725_handle, TCS34725_IntegrationTime_t i_time)
{
    esp_err_t err = ESP_OK;
    uint8_t cmd = i_time;

    TCS34725_HANDLE_CHECK(tcs34725_handle, ESP_FAIL);
    
    err = I2cMaster_WriteReg(tcs34725_handle->i2c_handle, tcs34725_handle->i2c_addr, 
                             TCS34725_ATIME, &cmd, 1);
    if (ESP_OK != err) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
  * @brief  TCS34725 Set gain.
  * @param[in]  tcs34725_handle  tcs34725 operation handle.
  * @param[in]  gain  Gain config.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t TCS34725_SetGain(TCS34725_handle_t tcs34725_handle, TCS34725_GainConfig_t gain)
{
    esp_err_t err = ESP_OK;
    uint8_t cmd = gain;

    TCS34725_HANDLE_CHECK(tcs34725_handle, ESP_FAIL);
    
    err = I2cMaster_WriteReg(tcs34725_handle->i2c_handle, tcs34725_handle->i2c_addr, 
                             TCS34725_ATIME, &cmd, 1);
    if (ESP_OK != err) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
  * @brief  TCS34725 collect enable.
  * @param[in]  tcs34725_handle  tcs34725 operation handle.
  * @retval void
  */
static void TCS34725_Enable(TCS34725_handle_t tcs34725_handle)
{
    uint8_t cmd = TCS34725_ENABLE_PON;
    I2cMaster_WriteReg(tcs34725_handle->i2c_handle, tcs34725_handle->i2c_addr, 
                       TCS34725_ENABLE, &cmd, 1);
    cmd = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
    I2cMaster_WriteReg(tcs34725_handle->i2c_handle, tcs34725_handle->i2c_addr, 
                       TCS34725_ENABLE, &cmd, 1);
    vTaskDelay(3 / portTICK_PERIOD_MS);
}

/**
  * @brief  TCS34725 collect disable.
  * @param[in]  tcs34725_handle  tcs34725 operation handle.
  * @retval void.
  */
static void TCS34725_Disable(TCS34725_handle_t tcs34725_handle)
{
    uint8_t cmd = 0;

    I2cMaster_ReadReg(tcs34725_handle->i2c_handle, tcs34725_handle->i2c_addr, 
                      TCS34725_ENABLE, &cmd, 1);
    cmd = cmd & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
    I2cMaster_WriteReg(tcs34725_handle->i2c_handle, tcs34725_handle->i2c_addr, 
                       TCS34725_ENABLE, &cmd, 1);
}

/**
  * @brief  TCS34725 Get single channel data.
  * @param[in]  tcs34725_handle  tcs34725 operation handle.
  * @param[in]  reg  Channel register.
  * @retval Conversion value of the channel.
  */
static uint16_t TCS34725_GetChannelData(TCS34725_handle_t tcs34725_handle, uint8_t reg)
{
    uint8_t tmp[2] = {0,0};
    uint16_t data = 0;

    I2cMaster_ReadReg(tcs34725_handle->i2c_handle, tcs34725_handle->i2c_addr, 
                      reg, tmp, 2);
    data = ((uint16_t)tmp[1] << 8) | tmp[0];
    return data;
}

/**
  * @brief  TCS34725 Get the color data of each channel.
  * @param[in]  tcs34725_handle  tcs34725 operation handle.
  * @param[out]  Returns the color value of the four channels (0-255).
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t TCS34725_GetRawData(TCS34725_handle_t tcs34725_handle, 
                              uint16_t *red, uint16_t *green, uint16_t *blue, uint16_t *clear)
{
    uint8_t status = 0x00;

    TCS34725_HANDLE_CHECK(tcs34725_handle, ESP_FAIL);

    status = TCS34725_STATUS_AVALID;
    TCS34725_Enable(tcs34725_handle);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    I2cMaster_ReadReg(tcs34725_handle->i2c_handle, tcs34725_handle->i2c_addr, 
                      TCS34725_STATUS, &status, 1);
    if (status & TCS34725_STATUS_AVALID) {
        *clear = TCS34725_GetChannelData(tcs34725_handle, TCS34725_CDATAL);	
        *red = TCS34725_GetChannelData(tcs34725_handle, TCS34725_RDATAL);	
        *green = TCS34725_GetChannelData(tcs34725_handle, TCS34725_GDATAL);	
        *blue = TCS34725_GetChannelData(tcs34725_handle, TCS34725_BDATAL);
        TCS34725_Disable(tcs34725_handle);
        return ESP_OK;
    }
    TCS34725_Disable(tcs34725_handle);
    return ESP_FAIL;
}
