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
  * @file           bh1750fvi_driver.h
  * @version        1.0
  * @date           2021-5-27
  */

#ifndef __BH1750FVI_H__
#define __BH1750FVI_H__

#include "driver/i2c.h"
#include "../../i2c_master/include/i2c_master.h"

#define BH1750FVI_POWER_ON 				0x01		//bh1750fvi power on
#define BH1750FVI_POWER_DOWN 			0x00		//bh1750fvi power off
#define BH1750FVI_RESET 				  0x07		//bh1750fvi reset
#define BH1750FVI_CONTI_H_MODE 		0x10		//bh1750fvi 11x resolution start measurement

typedef struct{
    I2cMaster_handle_t i2c_handle;
    uint8_t i2c_addr;
}BH1750FVI_t;
typedef BH1750FVI_t *BH1750FVI_handle_t;

/**
  * @brief  Initialize the bh1750fvi and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                       
  * @retval  
  *         successful  bh1750fvi operation handle.
  *         failed      NULL.
  * @note  Use BH1750FVI_Deinit() to release it.
  */
BH1750FVI_handle_t BH1750FVI_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr);

/**
  * @brief  BH1750FVI deinitialization.
  * @param[in]  bh1750fvi_handle  bh1750fvi operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BH1750FVI_Deinit(BH1750FVI_handle_t* bh1750fvi_handle);

/**
  * @brief  BH1750FVI Get data once.
  * @param[in]  sgp30_handle  bh1750fvi operation handle.
  * @param[out]  brightness  brightness data, (0-65535), unit: lx
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note This process requires a small 200ms.
  */
esp_err_t BH750FVI_GetData(BH1750FVI_handle_t bh1750fvi_handle, float* brightness);

#endif /* __BH1750FVI_H__ */