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
  * @file           tcs34725_driver.h
  * @version        1.0
  * @date           2021-6-22
  */

#ifndef __TCS34725_DRIVER_H__
#define __TCS34725_DRIVER_H__

#include "driver/i2c.h"
#include "../../i2c_master/include/i2c_master.h"
#include "tcs34725_reg.h"

typedef struct{
    I2cMaster_handle_t i2c_handle;
    uint8_t i2c_addr;
}TCS34725_t;
typedef TCS34725_t *TCS34725_handle_t;

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
TCS34725_handle_t TCS34725_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr);

/**
  * @brief  TCS34725 deinitialization.
  * @param[in]  tcs34725_handle  tcs34725 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t TCS34725_Deinit(TCS34725_handle_t* tcs34725_handle);

/**
  * @brief  TCS34725 Set integration time.
  * @param[in]  tcs34725_handle  tcs34725 operation handle.
  * @param[in]  i_time  integratiion time config.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t TCS34725_SetIntegrationTime(TCS34725_handle_t tcs34725_handle, TCS34725_IntegrationTime_t i_time);

/**
  * @brief  TCS34725 Set gain.
  * @param[in]  tcs34725_handle  tcs34725 operation handle.
  * @param[in]  gain  Gain config.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t TCS34725_SetGain(TCS34725_handle_t tcs34725_handle, TCS34725_GainConfig_t gain);

/**
  * @brief  TCS34725 Get the color data of each channel.
  * @param[in]  tcs34725_handle  tcs34725 operation handle.
  * @param[out]  Returns the color value of the four channels (0-255).
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t TCS34725_GetRawData(TCS34725_handle_t tcs34725_handle, 
                              uint16_t *red, uint16_t *green, uint16_t *blue, uint16_t *clear);

#endif /* __SGP30_DRIVER_H__ */
