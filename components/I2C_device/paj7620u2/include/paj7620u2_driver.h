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
  * @file           paj7620u2_driver.h
  * @version        1.0
  * @date           2021-5-21
  */

#ifndef __PAJ7620U2_DRIVER_H
#define __PAJ7620U2_DRIVER_H

#include "../../i2c_master/include/i2c_master.h"
#include "driver/i2c.h"

typedef struct {
    I2cMaster_handle_t i2c_handle;    
    uint8_t i2c_addr;     
} PAJ7620U2_t;
typedef PAJ7620U2_t *PAJ7620U2_handle_t;

typedef enum {
    PAJ7620U2_BANK_0,
    PAJ7620U2_BANK_1,
    PAJ7620U2_BANK_MAX,
} PAJ7620U2_Bank_t;

enum {
    PAJ7620U2_GESTURE_UP = (1<<0),                // up.
    PAJ7620U2_GESTURE_DOWM = (1<<1),              // down.
    PAJ7620U2_GESTURE_LEFT = (1<<2),              // left.
    PAJ7620U2_GESTURE_RIGHT = (1<<3),             // right.
    PAJ7620U2_GESTURE_FORWARD = (1<<4),           // forward.
    PAJ7620U2_GESTURE_BACKWARD = (1<<5),          // backward.
    PAJ7620U2_GESTURE_CLOCKWISE = (1<<6),         // clockwise.
    PAJ7620U2_GESTURE_COUNT_CLOCKWISE = (1<<7),   // count clockwise.
    PAJ7620U2_GESTURE_WAVE = (1<<8),              // wave.
};

/**
  * @brief  Initialize the PAJ7620U2 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                       Under normal circumstances it is 0X73.
  * @retval  
  *         successful  paj7620u2 operation handle.
  *         failed      NULL.
  * @note  Use PAJ7620U2_Deinit() to release it.
  */
PAJ7620U2_handle_t PAJ7620U2_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr);

/**
  * @brief  PAJ7620U2 deinitialization.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t PAJ7620U2_Deinit(PAJ7620U2_handle_t* paj7620u2_handle);

/**
  * @brief  PAJ7620U2 initialize to gesture recognition mode.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t PAJ7620U2_GestureInit(PAJ7620U2_handle_t paj7620u2_handle);

/**
  * @brief  PAJ7620U2 Get gesture status.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @param[out]  gesture_status  gesture status.
  * @note  Make sure to perform gesture recognition initialization before using 
  *        this function.
  */
esp_err_t PAJ7620U2_GestureGetState(PAJ7620U2_handle_t paj7620u2_handle, uint16_t* gesture_status);

/**
  * @brief  PAJ7620U2 initialize to object proximity measurement mode.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t PAJ7620U2_ApproachInit(PAJ7620U2_handle_t paj7620u2_handle);

/**
  * @brief  PAJ7620U2 Obtain object proximity measurement data.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @param[out]  obj_brightness  Object brightness value (0~255).
  * @param[out]  obj_size  Object size value (0~900).
  * @note  Make sure to perform object proximity measurement mode initialization 
  *        before using this function.
  */
esp_err_t PAJ7620U2_ApproachGetData(PAJ7620U2_handle_t paj7620u2_handle, uint8_t *obj_brightness, 
                               uint16_t *obj_size);

#endif /* __PAJ7620U2_DRIVER_H */