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
  * @file           ads1115_driver.h
  * @version        1.0
  * @date           2021-6-8
  */

#ifndef __ADS1115_DRIVER_H
#define __ADS1115_DRIVER_H

#include "driver/i2c.h"
#include "../../i2c_master/include/i2c_master.h"
#include "ads1115_reg.h"

typedef struct{
    I2cMaster_handle_t i2c_handle;
    uint8_t i2c_addr;
}ADS1115_t;
typedef ADS1115_t *ADS1115_handle_t;

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
ADS1115_handle_t ADS1115_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr);

/**
  * @brief  ADS1115 deinitialization.
  * @param[in]  ads1115_handle  ads1115 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t ADS1115_Deinit(ADS1115_handle_t* ads1115_handle);

/**
  * @brief  ADS1115 Device channel selection settings.
  * @param[in]  ads1115_handle  ads1115 operation handle.
  * @param[in]  mux_config  ads1115 channel selection.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t ADS1115_SetMux(ADS1115_handle_t ads1115_handle, ADS1115_RegConfigMux_t mux_config);

/**
  * @brief  ADS1115 Range configuration.
  * @param[in]  ads1115_handle  ads1115 operation handle.
  * @param[in]  pga_config ads1115 range.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t ADS1115_SetPga(ADS1115_handle_t ads1115_handle, ADS1115_RegConfigPga_t pga_config);

/**
  * @brief  ADS1115 Read the voltage value once. 
  * @param[in]  ads1115_handle  ads1115 operation handle.
  * @retval  Voltage value, single: V.
  */
double ADS1115_GetVoltageOnce(ADS1115_handle_t ads1115_handle);

#endif /* __ADS1115_DRIVER_H */