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
  * @file           axp192_driver.h
  * @version        1.0
  * @date           2021-6-7
  */

#ifndef __AXP192_DRIVER_H
#define __AXP192_DRIVER_H

#include "driver/i2c.h"
#include "../../i2c_master/include/i2c_master.h"

typedef struct{
    I2cMaster_handle_t i2c_handle;
    uint8_t i2c_addr;
}AXP192_t;
typedef AXP192_t *AXP192_handle_t;

/**
  * @brief  Initialize the AXP192 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  * @retval  
  *         successful  axp192 operation handle.
  *         failed      NULL.
  * @note  Use AXP192_Deinit() to release it.
  */
AXP192_handle_t AXP192_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr);

/**
  * @brief  AXP192 deinitialization.
  * @param[in]  axp192_handle  axp192 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t AXP192_Deinit(AXP192_handle_t* axp192_handle);

/**
  * @brief  AXP192 checks whether ACIN power input exists.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval 
  *         - true  ACIN exists.
  *         - false  ACIN does not exist.
  * @note  ACIN generally stands for external adapter power input.
  */
bool AXP192_CheckAcinExist(AXP192_handle_t axp192_handle);

/**
  * @brief  AXP192 checks whether the VBUS power input exists.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval 
  *         - true  VBUS exists.
  *         - false  VBUS does not exist.
  * @note  VBUS generally stands for USB power input.
  */
bool AXP192_CheckVbusExist(AXP192_handle_t axp192_handle);

/**
  * @brief  AXP192 check if the battery exists.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval 
  *         - true  The battery has been connected to AXP192.
  *         - false  Connect to AXP192 without battery.
  */
bool AXP192_CheckBatteryExist(AXP192_handle_t axp192_handle);

/**
  * @brief  AXP192 checks whether the battery is charging.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval 
  *         - true  The battery is charging.
  *         - false  The battery is not charging or the charging is complete.
  */
bool AXP192_CheckBatteryCharge(AXP192_handle_t axp192_handle);

/**
  * @brief  AXP192 battery charging enable control.
  * @param[in]   axp192_handle  axp192 operation handle.
  * @param[in]  enable  Charge switch control.
  * @retval  reference esp_err_t.
  */
esp_err_t AXP192_BatteryChargeControl(AXP192_handle_t axp192_handle, bool enable);

/**
  * @brief  AXP192 backup battery charging enable control.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @param[in]  enable  Charge switch control.
  * @retval  reference esp_err_t.
  * @note  RTC backup battery, the default is not turned on when you need to charge.
  */
esp_err_t AXP192_BackupChargeControl(AXP192_handle_t axp192_handle, bool enable);

/**
  * @brief  AXP192 gets ACIN voltage.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval  ACIN voltage value unit V is accurate to 5 decimal places.
  */
float AXP192_GetAcinVal(AXP192_handle_t axp192_handle);

/**
  * @brief  AXP192 gets VBUS voltage.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval  VBUS voltage value unit V is accurate to 5 decimal places.
  */
float AXP192_GetVbusVal(AXP192_handle_t axp192_handle);

/**
  * @brief  AXP192 get battery voltage.
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval  Battery voltage value unit V is accurate to 5 decimal places.
  */
float AXP192_GetBatteryVal(AXP192_handle_t axp192_handle);

/**
  * @brief  AXP192 reads the internal temperature of the chip
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval  The internal temperature of the chip, in degrees Celsius.
  */
float AXP192_GetInternalTemperature(AXP192_handle_t axp192_handle);

/**
  * @brief  AXP192 estimates the percentage of battery power (using 3.7v lithium battery).
  * @param[in]  axp192_handle  axp192 operation handle.
  * @retval  Battery percentage, resolution is 10%.
  * @note   The power is estimated from the battery voltage and is for reference only. 
  *         And it cannot be used while charging.
  */
uint8_t AXP192_GetBatteryValPower(AXP192_handle_t axp192_handle);

#endif /* __AXP192_DRIVER_H */