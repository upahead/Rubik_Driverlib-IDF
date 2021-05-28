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
  * @file           pca9554_driver.h
  * @version        1.0
  * @date           2021-5-28
  */

#ifndef __PCA9554_DRIVER_H
#define __PCA9554_DRIVER_H

#include "driver/i2c.h"
#include "../../i2c_master/include/i2c_master.h"

#define PCA9554_INPUT_PORT_REGISTER           0x00    //PCA9554 input port register.
#define PCA9554_OUTPUT_PORT_REGISTER          0x01    //PCA9554 out put port register.
#define PCA9554_POLARITY_INVERSION_REGISTER   0x02    //PCA9554 polarity inversion register.
#define PCA9554_CONFIGURATION_REGISTER        0x03    //PCA9554 configuration register.

// PCA9554 port number enum.
typedef enum{
    PCA9554_PIN_0, 
    PCA9554_PIN_1,
    PCA9554_PIN_2,
    PCA9554_PIN_3,
    PCA9554_PIN_4,
    PCA9554_PIN_5,
    PCA9554_PIN_6,
    PCA9554_PIN_7,
    PCA9554_PIN_MAX,
}PCA9554_PinNum_t;

// PCA9554 port direction enum.
typedef enum{
    PCA9554_PIN_OUTPUT,
    PCA9554_PIN_INPUT,
}PCA9554_PinDir_t;

// PCA9554 port level enum.
typedef enum{
    PCA9554_PIN_LEVEL_LOW,
    PCA9554_PIN_LEVEL_HIG,
}PCA9554_PinLevel_t;

typedef struct{
    I2cMaster_handle_t i2c_handle;
    uint8_t i2c_addr;
}PCA9554_t;
typedef PCA9554_t *PCA9554_handle_t;

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
PCA9554_handle_t PCA9554_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr);

/**
  * @brief  PCA9554 deinitialization.
  * @param[in]  pca9554_handle  pca9554 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t PCA9554_Deinit(PCA9554_handle_t* pca9554_handle);

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
                                     PCA9554_PinNum_t pin, PCA9554_PinDir_t dir);

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
                              PCA9554_PinLevel_t level);

/**
  * @brief  PCA9554 Get port level.
  * @param[in]  pca9554_handle  pca9554 operation handle.
  * @param[in]  pin  PCA9554 port number.
  * @retval  PCA9554 port level.
  */
PCA9554_PinLevel_t PCA9554_GetPinLevel(PCA9554_handle_t pca9554_handle, PCA9554_PinNum_t pin);

/**
  * @brief  PCA9554 Set the level of all ports (8 bits in total)
  * @param[in]  pca9554_handle  pca9554 operation handle.
  * @param[in]  level PCA9554 Level value.(8bit)
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note   Write the level of 8 ports at a time, using this interface is more efficient.
  */
esp_err_t PCA9554_SetLevelHex(PCA9554_handle_t pca9554_handle, uint8_t level);

/**
  * @brief  PCA9554 Get all port levels.(8bit)
  * @param[in] pca9554_handle  pca9554 operation handle.
  * @retval port level.(8bit)
  */
uint8_t PCA9554_GetLevelHex(PCA9554_handle_t pca9554_handle);

#endif /* __PCA9554_DRIVER_H */