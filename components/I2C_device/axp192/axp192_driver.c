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
  * @file           axp192_driver.c
  * @version        1.0
  * @date           2021-6-7
  */

#include "axp192_driver.h"
#include "axp192_reg.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "AXP192";

#define AXP192_HANDLE_CHECK(a, ret)  if (NULL == a) {                            \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

/**
  * @brief  Initialize the AXP192 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  * @retval  
  *         successful  axp192 operation handle.
  *         failed      NULL.
  * @note  Use AXP192_Deinit() to release it.
  */
AXP192_handle_t AXP192_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr)
{
    if (NULL == i2c_handle) {
        ESP_LOGE(TAG, "%s (%d) i2c handle is not initialized.", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (i2c_handle->i2c_clk > 400000) {
        ESP_LOGE(TAG, "%s (%d) AXP192 I2C supports up to 400Kbit/s", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (false == I2CMaster_CheckDeviceAlive(i2c_handle, i2c_addr)) {
        ESP_LOGE(TAG, "%s (%d) device not alive.", __FUNCTION__, __LINE__);
        return NULL;
    }

    AXP192_handle_t axp192_handle = malloc(sizeof(AXP192_t));
    if (NULL == axp192_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    axp192_handle->i2c_handle = i2c_handle;
    axp192_handle->i2c_addr = i2c_addr;
    ESP_LOGI(TAG, "%s (%d) axp192 init ok.", __FUNCTION__, __LINE__);
    return axp192_handle;
}

/**
  * @brief  AXP192 deinitialization.
  * @param[in]  axp192_handle  axp192 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t AXP192_Deinit(AXP192_handle_t* axp192_handle)
{
    AXP192_HANDLE_CHECK(*axp192_handle, ESP_FAIL);

    free(*axp192_handle);
    *axp192_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) axp192 handle deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

/**
  * @brief  AXP192 checks whether ACIN power input exists.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval 
  *         - true  ACIN exists.
  *         - false  ACIN does not exist.
  * @note  ACIN generally stands for external adapter power input.
  */
bool AXP192_CheckAcinExist(AXP192_handle_t axp192_handle)
{
    uint8_t exist = 0;

    I2cMaster_ReadRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                         AXP192_INPUT_POWER_STATUS_REG, 7, &exist, 1);
    if (exist) {
        return true;
    }
    return false;
}

/**
  * @brief  AXP192 checks whether the VBUS power input exists.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval 
  *         - true  VBUS exists.
  *         - false  VBUS does not exist.
  * @note  VBUS generally stands for USB power input.
  */
bool AXP192_CheckVbusExist(AXP192_handle_t axp192_handle)
{
    uint8_t exist = 0;

    I2cMaster_ReadRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                         AXP192_INPUT_POWER_STATUS_REG, 5, &exist, 1);
    if (exist) {
        return true;
    }
    return false;
}

/**
  * @brief  AXP192 check if the battery exists.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval 
  *         - true  The battery has been connected to AXP192.
  *         - false  Connect to AXP192 without battery.
  */
bool AXP192_CheckBatteryExist(AXP192_handle_t axp192_handle)
{
    uint8_t exist = 0;

    I2cMaster_ReadRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                         AXP192_POWER_MODE_CHARGE_STATUS_REG, 5, &exist, 1);
    if (exist) {
        return true;
    }
    return false;
}

/**
  * @brief  AXP192 checks whether the battery is charging.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval 
  *         - true  The battery is charging.
  *         - false  The battery is not charging or the charging is complete.
  */
bool AXP192_CheckBatteryCharge(AXP192_handle_t axp192_handle)
{
    uint8_t charge = 0;

    I2cMaster_ReadRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                         AXP192_POWER_MODE_CHARGE_STATUS_REG, 6, &charge, 1);
    if (charge) {
        return true;
    }
    return false;
}

/**
  * @brief  AXP192 battery charging enable control.
  * @param[in]   axp192_handle  axp192 operation handle.
  * @param[in]  enable  Charge switch control.
  * @retval  reference esp_err_t.
  */
esp_err_t AXP192_BatteryChargeControl(AXP192_handle_t axp192_handle, bool enable)
{
    AXP192_HANDLE_CHECK(axp192_handle, ESP_FAIL);

    return I2cMaster_WriteRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                                 AXP192_CHARGE_CONTROL_REG_1, 7, enable, 1);
}

/**
  * @brief  AXP192 backup battery charging enable control.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @param[in]  enable  Charge switch control.
  * @retval  reference esp_err_t.
  * @note  RTC backup battery, the default is not turned on when you need to charge.
  */
esp_err_t AXP192_BackupChargeControl(AXP192_handle_t axp192_handle, bool enable)
{
    AXP192_HANDLE_CHECK(axp192_handle, ESP_FAIL);

    return I2cMaster_WriteRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                                 AXP192_BACKUP_CHARGE_CONTROL_REG, 7, enable, 1);
}

/**
  * @brief  AXP192 gets ACIN voltage.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval  ACIN voltage value unit V is accurate to 5 decimal places.
  */
float AXP192_GetAcinVal(AXP192_handle_t axp192_handle)
{
    uint8_t reg_data[2] = {0};
    uint16_t vol_data = 0;
    uint8_t adc_enable = 0;

    AXP192_HANDLE_CHECK(axp192_handle, 0);

    // If the VCIN voltage ADC is not enabled, enable it first.
    I2cMaster_ReadRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                         AXP192_ADC_ENABLE_CONTROL_REG_1, 5, &adc_enable, 1);
    if (!adc_enable) {
        I2cMaster_WriteRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                              AXP192_ADC_ENABLE_CONTROL_REG_1, 5, 0x01, 1);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    // Read voltage value.
    I2cMaster_ReadReg(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                      AXP192_ACIN_VAL_ADC_REG, reg_data, 2);
    vol_data = reg_data[0] << 4;
    vol_data |= reg_data[1] & 0x0f;
    return ((float)(vol_data/4096.0))*6.9615;
}

/**
  * @brief  AXP192 gets VBUS voltage.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval  VBUS voltage value unit V is accurate to 5 decimal places.
  */
float AXP192_GetVbusVal(AXP192_handle_t axp192_handle)
{
    uint8_t reg_data[2] = {0};
    uint16_t vol_data = 0;
    uint8_t adc_enable = 0;

    AXP192_HANDLE_CHECK(axp192_handle, 0);

    // If VBUS voltage ADC is not enabled, enable first.
    I2cMaster_ReadRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                         AXP192_ADC_ENABLE_CONTROL_REG_1, 3, &adc_enable, 1);
    if (!adc_enable) {
        I2cMaster_WriteRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                              AXP192_ADC_ENABLE_CONTROL_REG_1, 3, 0x01, 1);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    // Read voltage value.
    I2cMaster_ReadReg(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                      AXP192_VBUS_VAL_ADC_REG, reg_data, 2);
    vol_data = reg_data[0] << 4;
    vol_data |= reg_data[1] & 0x0f;
    return ((float)(vol_data/4096.0))*6.9615;
}

/**
  * @brief  AXP192 get battery voltage.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval  Battery voltage value unit V is accurate to 5 decimal places.
  */
float AXP192_GetBatteryVal(AXP192_handle_t axp192_handle)
{
    uint8_t reg_data[2] = {0};
    uint16_t vol_data = 0;
    uint8_t adc_enable = 0;

    AXP192_HANDLE_CHECK(axp192_handle, 0);

    // If the battery voltage ADC is not enabled, enable it first.
    I2cMaster_ReadRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                         AXP192_ADC_ENABLE_CONTROL_REG_1, 7, &adc_enable, 1);
    if (!adc_enable) {
        I2cMaster_WriteRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                              AXP192_ADC_ENABLE_CONTROL_REG_1, 7, 0x01, 1);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    // Read voltage value.
    I2cMaster_ReadReg(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                      AXP192_BATTERY_VAL_ADC_REG, reg_data, 2);
    vol_data = reg_data[0] << 4;
    vol_data |= reg_data[1] & 0x0f;
    return ((float)(vol_data/4096.0))*4.5045;
}

/**
  * @brief  AXP192 reads the internal temperature of the chip
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval  The internal temperature of the chip, in degrees Celsius.
  */
float AXP192_GetInternalTemperature(AXP192_handle_t axp192_handle)
{
    uint8_t data_buf[2] = {0};
    uint8_t adc_enable = 0;

    // If the temperature ADC is not enabled, enable it first.
    I2cMaster_ReadRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                         AXP192_ADC_ENABLE_CONTROL_REG_2, 7, &adc_enable, 1);
    if (!adc_enable) {
        I2cMaster_WriteRegBit(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                              AXP192_ADC_ENABLE_CONTROL_REG_2, 7, 0x01, 1);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    // Read voltage value.
    I2cMaster_ReadReg(axp192_handle->i2c_handle, axp192_handle->i2c_addr, 
                      AXP192_INTERNAL_TEMPERATURE_ADC_REG, data_buf, 2);
    return ((data_buf[0] << 4) + (data_buf[1] & 0x0f)) * 0.1 -144.7;
}

/**
  * @brief  AXP192 estimates the percentage of battery power (using 3.7v lithium battery).
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval  Battery percentage, resolution is 10%.
  * @note   The power is estimated from the battery voltage and is for reference only. 
  *         And it cannot be used while charging.
  */
uint8_t AXP192_GetBatteryValPower(AXP192_handle_t axp192_handle)
{
    float power_val = AXP192_GetBatteryVal(axp192_handle);
    if(power_val >= 4.15)
        return 100;
    else if(power_val >= 4.08)
        return 90;
    else if(power_val >= 3.97)
        return 80;
    else if(power_val >= 3.90)
        return 70;
    else if(power_val >= 3.84)
        return 60;
    else if(power_val >= 3.79)
        return 50;
    else if(power_val >= 3.76)
        return 40;
    else if(power_val >= 3.73)
        return 30;
    else if(power_val >= 3.71)
        return 20;
    else if(power_val >= 3.65)
        return 10;
    else
        return 0;
}

