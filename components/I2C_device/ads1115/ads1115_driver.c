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
  * @file           ads1115_driver.c
  * @version        1.0
  * @date           2021-6-8
  */

#include "ads1115_driver.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "../i2c_master/include/i2c_master.h"

static const char *TAG = "ADS1115";

#define ADS1115_HANDLE_CHECK(a, ret)  if (NULL == a) {                           \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

/**
  * @brief  Initialize the ADS1115 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                       ADDR = GND : 0x48
  *                       ADDR = VDD : 0x49
  *                       ADDR = SDA : 0x4A
  *                       ADDR = SCL : 0x4B
  * @retval  
  *         successful  ads1115 operation handle.
  *         failed      NULL.
  * @note  Use ADS1115_Deinit() to release it.
  */
ADS1115_handle_t ADS1115_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr)
{
    if (NULL == i2c_handle) {
        ESP_LOGE(TAG, "%s (%d) i2c handle is not initialized.", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (i2c_handle->i2c_clk > 400000) {
        ESP_LOGE(TAG, "%s (%d) ADS1115 I2C supports up to 400Kbit/s", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (false == I2CMaster_CheckDeviceAlive(i2c_handle, i2c_addr)) {
        ESP_LOGE(TAG, "%s (%d) device not alive.", __FUNCTION__, __LINE__);
        return NULL;
    }

    ADS1115_handle_t ads1115_handle = malloc(sizeof(ADS1115_t));
    if (NULL == ads1115_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    ads1115_handle->i2c_handle = i2c_handle;
    ads1115_handle->i2c_addr = i2c_addr;
    ESP_LOGI(TAG, "%s (%d) ads1115 init ok.", __FUNCTION__, __LINE__);
    return ads1115_handle;
}

/**
  * @brief  ADS1115 deinitialization.
  * @param[in]  ads1115_handle  ads1115 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t ADS1115_Deinit(ADS1115_handle_t* ads1115_handle)
{
    ADS1115_HANDLE_CHECK(*ads1115_handle, ESP_FAIL);

    free(*ads1115_handle);
    *ads1115_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) ads1115 handle deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

/**
  * @brief  ADS1115 Device channel selection settings.
  * @param[in]  ads1115_handle  ads1115 operation handle.
  * @param[in]  mux_config  ads1115 channel selection.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t ADS1115_SetMux(ADS1115_handle_t ads1115_handle, ADS1115_RegConfigMux_t mux_config)
{
    esp_err_t err = ESP_OK;
    uint8_t data_buf[2] = {0};

    ADS1115_HANDLE_CHECK(ads1115_handle, ESP_FAIL);

    // Read current configuration value.
    err = I2cMaster_ReadReg(ads1115_handle->i2c_handle, ads1115_handle->i2c_addr, 
                            ADS1115_POINTER_CONFIG_REG, data_buf, 2);
    if (ESP_OK != err) {
        return ESP_FAIL;
    }
    // Write new configuration value.
    data_buf[0] &= ~(0b111 << 4);
    data_buf[0] |= mux_config;
    err = I2cMaster_WriteReg(ads1115_handle->i2c_handle, ads1115_handle->i2c_addr, 
                             ADS1115_POINTER_CONFIG_REG, data_buf, 2);
    if (ESP_OK != err) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
  * @brief  ADS1115 Range configuration.
  * @param[in]  ads1115_handle  ads1115 operation handle.
  * @param[in]  pga_config ads1115 range.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t ADS1115_SetPga(ADS1115_handle_t ads1115_handle, ADS1115_RegConfigPga_t pga_config)
{
    esp_err_t err = ESP_OK;
    uint8_t data_buf[2] = {0};

    ADS1115_HANDLE_CHECK(ads1115_handle, ESP_FAIL);

    //  Read current configuration value.
    err = I2cMaster_ReadReg(ads1115_handle->i2c_handle, ads1115_handle->i2c_addr, 
                            ADS1115_POINTER_CONFIG_REG, data_buf, 2);
    if (ESP_OK != err) {
        return ESP_FAIL;
    }
    // Write new configuration value.
    data_buf[0] &= ~(0b111 << 1);
    data_buf[0] |= pga_config;
    err = I2cMaster_WriteReg(ads1115_handle->i2c_handle, ads1115_handle->i2c_addr, 
                             ADS1115_POINTER_CONFIG_REG, data_buf, 2);
    if (ESP_OK != err) {
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "%s (%d) ads1115 set pga ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

/**
  * @brief  ADS1115 Read the voltage value once. 
  * @param[in]  ads1115_handle  ads1115 operation handle.
  * @retval  Voltage value, single: V.
  */
double ADS1115_GetVoltageOnce(ADS1115_handle_t ads1115_handle)
{
    uint8_t data_buf[2] = {0};
    uint8_t crh = 0x00;
    uint16_t vol_data = 0;
    double vol_val = 0.0;

    ADS1115_HANDLE_CHECK(ads1115_handle, 0);

    // 读取当前配置值
    I2cMaster_ReadReg(ads1115_handle->i2c_handle, ads1115_handle->i2c_addr, 
                      ADS1115_POINTER_CONFIG_REG, data_buf, 2);
    crh = data_buf[0];  // Save the high bits of the configuration register.
    
    // Write start conversion signal.
    data_buf[0] &= ~(0b1 << 7);
    data_buf[0] |= ADS1115_REG_CONFIG_OS_W_SINGLE_START; 
    I2cMaster_WriteReg(ads1115_handle->i2c_handle, ads1115_handle->i2c_addr, 
                       ADS1115_POINTER_CONFIG_REG, data_buf, 2);

    // Read the conversion register after waiting for the end of the conversion.
    vTaskDelay(20 / portTICK_PERIOD_MS);
    I2cMaster_ReadReg(ads1115_handle->i2c_handle, ads1115_handle->i2c_addr, 
                      ADS1115_POINTER_CONVERT_REG, data_buf, 2);
    vol_data = data_buf[0]*256 + data_buf[1];
    // Determine whether the range is exceeded. 
    if (vol_data==0x7FFF || vol_data==0x8000) {
        ESP_LOGE(TAG, "%s (%d) ads1115 voltage Out of range.", __FUNCTION__, __LINE__);
        return 0;
    }

    // Calculate the voltage value according to the range.
    switch(0b00001110&crh){
        case(ADS1115_REG_CONFIG_PGA_FSR_6114):
            vol_val = (double)vol_data*187.5/1000000.0;
        break;
        case(ADS1115_REG_CONFIG_PGA_FSR_4096):
            vol_val = (double)vol_data*125/1000000.0;
        break;
        case(ADS1115_REG_CONFIG_PGA_FSR_2048):
            vol_val = (double)vol_data*62.5/1000000.0;
        break;
        case(ADS1115_REG_CONFIG_PGA_FSR_1024):
            vol_val = (double)vol_data*31.25/1000000.0;
        break;
        case(ADS1115_REG_CONFIG_PGA_FSR_0512):
            vol_val = (double)vol_data*15.625/1000000.0;
        break;
        case(ADS1115_REG_CONFIG_PGA_FSR_0256):
            vol_val = (double)vol_data*7.8125/1000000.0;
        break;
    }
    return vol_val;
}
