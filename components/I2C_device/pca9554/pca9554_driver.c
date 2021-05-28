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
  * @file           pca9554_driver.c
  * @version        1.0
  * @date           2021-5-28
  */

#include "pca9554_driver.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "PCA9554";

#define PCA9554_HANDLE_CHECK(a, ret)  if (NULL == a) {                           \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

/**
  * @brief  Initialize the PCA9554 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                       
  * @retval  
  *         successful  pca9554 operation handle.
  *         failed      NULL.
  * @note  Use PCA9554_Deinit() to release it.
  */
PCA9554_handle_t PCA9554_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr)
{
    if (NULL == i2c_handle) {
        ESP_LOGE(TAG, "%s (%d) i2c handle is not initialized.", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (i2c_handle->i2c_clk > 400000) {
        ESP_LOGE(TAG, "%s (%d) PCA9554 I2C supports up to 400Kbit/s", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (false == I2CMaster_CheckDeviceAlive(i2c_handle, i2c_addr)) {
        ESP_LOGE(TAG, "%s (%d) device not alive.", __FUNCTION__, __LINE__);
        return NULL;
    }

    PCA9554_handle_t pca9554_handle = malloc(sizeof(PCA9554_t));
    if (NULL == pca9554_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    pca9554_handle->i2c_handle = i2c_handle;
    pca9554_handle->i2c_addr = i2c_addr;

    ESP_LOGI(TAG, "%s (%d) pca9554 init ok.", __FUNCTION__, __LINE__);
    return pca9554_handle;
}

/**
  * @brief  PCA9554 deinitialization.
  * @param[in]  pca9554_handle  pca9554 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t PCA9554_Deinit(PCA9554_handle_t* pca9554_handle)
{
    PCA9554_HANDLE_CHECK(*pca9554_handle, ESP_FAIL);

    free(*pca9554_handle);
    *pca9554_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) pca9554 handle deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

/**
  * @brief  PCA9554 Configure the direction of a port.
  * @param[in]  pca9554_handle  pca9554 operation handle.
  * @param[in]  pin  PCA9554 port number.
  * @param[in]  dir  PCA9554 port direction.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t PCA9554_ConfigPinDirection(PCA9554_handle_t pca9554_handle, 
                                     PCA9554_PinNum_t pin, PCA9554_PinDir_t dir)
{
    esp_err_t ret = 0;
    uint8_t data_buf = 0;

    PCA9554_HANDLE_CHECK(pca9554_handle, ESP_FAIL);

    // Read the current configuration status.
    ret = I2cMaster_ReadReg(pca9554_handle->i2c_handle, pca9554_handle->i2c_addr, 
                            PCA9554_CONFIGURATION_REGISTER, &data_buf, 1);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }

    // Clear the bit to be configured and write the new configuration.
    data_buf &= ~(0x1 << pin);
    data_buf |= (dir << pin);
    ret = I2cMaster_WriteReg(pca9554_handle->i2c_handle, pca9554_handle->i2c_addr, 
                             PCA9554_CONFIGURATION_REGISTER, &data_buf, 1);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
  * @brief  PCA9554 Set port level.
  * @param[in]  pca9554_handle  pca9554 operation handle.
  * @param[in]  pin  PCA9554 port number.
  * @param[in]  level  PCA9554 port level.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t PCA9554_SetPinLevel(PCA9554_handle_t pca9554_handle, PCA9554_PinNum_t pin, 
                              PCA9554_PinLevel_t level)
{   
    esp_err_t err = 0;
    uint8_t data_buf = 0;
    
    // Read the current configuration status.
    err = I2cMaster_ReadReg(pca9554_handle->i2c_handle, pca9554_handle->i2c_addr, 
                            PCA9554_OUTPUT_PORT_REGISTER, &data_buf, 1);
    if (err != ESP_OK) {
        return ESP_FAIL;
    }

    // Clear the bit to be configured and write the new configuration.
    data_buf &= ~(0x1 << pin);
    data_buf |= (level << pin);
    err = I2cMaster_WriteReg(pca9554_handle->i2c_handle, pca9554_handle->i2c_addr, 
                             PCA9554_OUTPUT_PORT_REGISTER, &data_buf, 1);
    if (err != ESP_OK) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
  * @brief  PCA9554 Get port level.
  * @param[in]  pca9554_handle  pca9554 operation handle.
  * @param[in]  pin  PCA9554 port number.
  * @retval  PCA9554 port level.
  */
PCA9554_PinLevel_t PCA9554_GetPinLevel(PCA9554_handle_t pca9554_handle, PCA9554_PinNum_t pin)
{   
    uint8_t data_buf = 0;

    I2cMaster_ReadReg(pca9554_handle->i2c_handle, pca9554_handle->i2c_addr, 
                      PCA9554_INPUT_PORT_REGISTER, &data_buf, 1);

    data_buf = (data_buf >> pin) & 0x01;
    if(data_buf){
        return PCA9554_PIN_LEVEL_HIG;
    }
    return PCA9554_PIN_LEVEL_LOW;
}

/**
  * @brief  PCA9554 Set the level of all ports (8 bits in total)
  * @param[in]  pca9554_handle  pca9554 operation handle.
  * @param[in]  level PCA9554 Level value.(8bit)
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note   Write the level of 8 ports at a time, using this interface is more efficient.
  */
esp_err_t PCA9554_SetLevelHex(PCA9554_handle_t pca9554_handle, uint8_t level)
{
    esp_err_t err = ESP_OK;
    uint8_t buf = level;

    PCA9554_HANDLE_CHECK(pca9554_handle, ESP_FAIL);

    err = I2cMaster_WriteReg(pca9554_handle->i2c_handle, pca9554_handle->i2c_addr, 
                       PCA9554_OUTPUT_PORT_REGISTER, &buf, 1);
    if (ESP_OK != err) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
  * @brief  PCA9554 Get all port levels.(8bit)
  * @param[in] pca9554_handle  pca9554 operation handle.
  * @retval port level.(8bit)
  */
uint8_t PCA9554_GetLevelHex(PCA9554_handle_t pca9554_handle)
{
    uint8_t data_buf = 0x0;

    I2cMaster_ReadReg(pca9554_handle->i2c_handle, pca9554_handle->i2c_addr, 
                      PCA9554_INPUT_PORT_REGISTER, &data_buf, 1);
    return data_buf;
}