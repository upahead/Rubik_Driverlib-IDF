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
  * @file           sgp30_driver.h
  * @version        1.0
  * @date           2021-5-22
  */

#ifndef __SGP30_DRIVER_H
#define __SGP30_DRIVER_H

#include "driver/i2c.h"
#include "../../i2c_master/include/i2c_master.h"

#define SGP30_SOFT_RESET_CMD        0x06          // soft reset command.
#define SGP30_INIT_AIR_QUALITY      0x2003        // Initialize air quality detection command.
#define SGP30_MEASURE_AIR_QUALITY   0x2008        // start air quality detection command.

#define SGP30_CRC8_POLYNOMIAL       0x31

typedef struct{
    I2cMaster_handle_t i2c_handle;
    uint8_t i2c_addr;
}SGP30_t;
typedef SGP30_t *SGP30_handle_t;

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
SGP30_handle_t SGP30_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr);

/**
  * @brief  SGP30 deinitialization.
  * @param[in]  sgp30_handle  sgp30 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t SGP30_Deinit(SGP30_handle_t* sgp30_handle);

/**
  * @brief  SGP30 send statrt messure command.
  * @param[in]  sgp30_handle  sgp30 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note  Send the start measurement command and wait for 1s before data collection.
  */
esp_err_t SGP30_StartMessure(SGP30_handle_t sgp30_handle);

/**
  * @brief  SGP30 get co2 and tvoc value.
  * @param[in]  sgp30_handle  sgp30 operation handle pointer.
  * @param[out]  co2_val  co2 value.
  * @param[out]  tvoc_val  TVOC value.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t SGP30_GetValue(SGP30_handle_t sgp30_handle, uint16_t *co2_val, uint16_t *tvoc_val);

#endif /* __SGP30_DRIVER_H */