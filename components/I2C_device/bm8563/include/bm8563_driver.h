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
  * @file           bm8563_driver.h
  * @version        1.0
  * @date           2021-6-1
  */

#ifndef __BM8563_DRIVER_H
#define __BM8563_DRIVER_H

#include "driver/i2c.h"
#include "../../i2c_master/include/i2c_master.h"

// BM8563 register
// control
#define BM8563_CONTROL_STATE_1_REG      0x00    // Control/Status Register 1.
#define BM8563_CONTROL_STATE_2_REG      0x01    // Control/Status Register 2.
#define BM8563_CLKOUT_FREQ__REG         0x0D    // CLKOUT Frequency register.
#define BM8563_TIMER_CONTROL_REG        0x0E    // Timer control register.
#define BM8563_TIMER_COUNT_DOWN_REG     0x0F    // Timer countdown register.
// time
#define BM8563_TIME_SEC_REG             0x02    // Seconds register.
#define BM8563_TIME_MIN_REG             0x03    // Minute register.
#define BM8563_TIME_HOUR_REG            0x04    // Hour register.
#define BM8563_TIME_DAY_REG             0x05    // Day register.
#define BM8563_TIME_WEEK_REG            0x06    // week register.
#define BM8563_TIME_MONTH_REG           0x07    // Month register.
#define BM8563_TIME_YEAR_REG            0x08    // Year register.
// alarm
#define BM8563_ALARM_MIN_REG            0x09    // Minute alarm register.
#define BM8563_ALARM_HOUR_REG           0x0A    // Hour alarm register.
#define BM8563_ALARM_DAY_REG            0x0B    // Day alarm register.
#define BM8563_ALARM_WEEK_REG           0x0C    // Week alarm register.

typedef struct{
    uint8_t sec;    // 0~59
    uint8_t min;    // 0~59
    uint8_t hour;   // 0~23
    uint8_t day;    // 1~31
    uint8_t week;   // Sunday = 0 , Monday = 1 ... Saturday = 6
    uint8_t month;  // 1~12
    uint8_t year;   // 0~99
}BM8563_Time_t;

typedef struct{
    I2cMaster_handle_t i2c_handle;
    uint8_t i2c_addr;
}BM8563_t;
typedef BM8563_t *BM8563_handle_t;

/**
  * @brief  Initialize the BM8563 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                       Under normal circumstances it is 0x51.
  * @retval  
  *         successful  bm8563 operation handle.
  *         failed      NULL.
  * @note  Use BM8563_Deinit() to release it.
  */
BM8563_handle_t BM8563_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr);

/**
  * @brief  BM8563 deinitialization.
  * @param[in]  bm8563_handle  bm8563 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BM8563_Deinit(BM8563_handle_t* bm8563_handle);

/**
  * @brief  BM8563 set time.
  * @param[in]  time  BM8563 time struct
  * @param[in]  bm8563_handle  bm8563 operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BM8563_SetTime(BM8563_handle_t bm8563_handle, BM8563_Time_t time);

/**
  * @brief  BM8563 get time.
  * @param[out]  time BM8563 time struct pointer.
  * @param[in]  bm8563_handle  bm8563 operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BM8563_GetTime(BM8563_handle_t bm8563_handle, BM8563_Time_t* time);

#endif /* __BM8563_DRIVER_H */