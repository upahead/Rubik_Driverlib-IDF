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
  * @file           bm8563_driver.c
  * @version        1.0
  * @date           2021-6-1
  */

#include "bm8563_driver.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "../i2c_master/include/i2c_master.h"

static const char *TAG = "BM8563";

#define BM8563_HANDLE_CHECK(a, ret)  if (NULL == a) {                            \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

/**
  * @brief  Initialize the BM8563 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                       Under normal circumstances it is 0x51.
  * @retval  
  *         successful  sgp30 operation handle.
  *         failed      NULL.
  * @note  Use BM8563_Deinit() to release it.
  */
BM8563_handle_t BM8563_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr)
{
    esp_err_t err = ESP_OK;
    uint8_t cmd_buf = 0x00;

    if (NULL == i2c_handle) {
        ESP_LOGE(TAG, "%s (%d) i2c handle is not initialized.", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (i2c_handle->i2c_clk > 400000) {
        ESP_LOGE(TAG, "%s (%d) BM8563 I2C supports up to 400Kbit/s", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (false == I2CMaster_CheckDeviceAlive(i2c_handle, i2c_addr)) {
        ESP_LOGE(TAG, "%s (%d) device not alive.", __FUNCTION__, __LINE__);
        return NULL;
    }

    BM8563_handle_t bm8563_handle = malloc(sizeof(BM8563_t));
    if (NULL == bm8563_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    // Reset to normal mode.
    cmd_buf = 0x00;
    err = I2cMaster_WriteReg(i2c_handle, i2c_addr, BM8563_CONTROL_STATE_1_REG, &cmd_buf, 1);
    if (err != ESP_OK) {
        goto BM8563_INIT_FAILED;
    }
    err = I2cMaster_WriteReg(i2c_handle, i2c_addr, BM8563_CONTROL_STATE_2_REG, &cmd_buf, 1);
    if (err != ESP_OK) {
        goto BM8563_INIT_FAILED;
    }
    bm8563_handle->i2c_handle = i2c_handle;
    bm8563_handle->i2c_addr = i2c_addr;
    ESP_LOGI(TAG, "%s (%d) bm8563 init ok.", __FUNCTION__, __LINE__);
    return bm8563_handle;

BM8563_INIT_FAILED:
    free(bm8563_handle);
    ESP_LOGE(TAG, "%s (%d) bm8563 init failed.", __FUNCTION__, __LINE__);
    return NULL;
}

/**
  * @brief  BM8563 deinitialization.
  * @param[in]  bm8563_handle  bm8563 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BM8563_Deinit(BM8563_handle_t* bm8563_handle)
{
    BM8563_HANDLE_CHECK(*bm8563_handle, ESP_FAIL);

    free(*bm8563_handle);
    *bm8563_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) bm8563 handle deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

/**
  * @brief  Convert HEX format to BCD format.
  * @param[in]  hex_data  HEX format data.
  * @retval  BCD format data.
  * @note   Note that the input data range is 0-99, the maximum range of this two-byte BCD code.
  */
static uint8_t BM8563_HexToBcd(uint8_t hex_data){return hex_data/10*16 + hex_data%10;}

/**
  * @brief  Convert BCD format to HEX format.
  * @param[in]  bcd_data  BCD format data.
  * @retval  HEX format data.
  */
static uint8_t BM8563_BcdToHex(uint8_t bcd_data){return bcd_data/16*10 + bcd_data%16;}

/**
  * @brief  BM8563 set time.
  * @param[in]  time  BM8563 time struct
  * @param[in]  bm8563_handle  bm8563 operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BM8563_SetTime(BM8563_handle_t bm8563_handle, BM8563_Time_t time)
{   
    esp_err_t err = ESP_OK;
    uint8_t data_buf[7] = {0};
    uint8_t *p = (uint8_t*)&time;

    BM8563_HANDLE_CHECK(bm8563_handle, ESP_FAIL);

    // Check the time format.
    if (time.sec > 59)
        goto BM8563_SET_TIME_FORMAT_ERR;
    if (time.min > 59)
        goto BM8563_SET_TIME_FORMAT_ERR;
    if (time.hour > 23)
        goto BM8563_SET_TIME_FORMAT_ERR;
    if (time.day < 1 || time.day > 31)
        goto BM8563_SET_TIME_FORMAT_ERR;
    if (time.week > 6)
        goto BM8563_SET_TIME_FORMAT_ERR;
    if (time.month < 1 || time.month > 12)
        goto BM8563_SET_TIME_FORMAT_ERR;
    if (time.year > 99)
        goto BM8563_SET_TIME_FORMAT_ERR;

    // Convert time data hex format to bcd format.
    for (uint32_t i=0; i<7; i++) {
        data_buf[i] = BM8563_HexToBcd(*(p+i));
    }
        
    // send time data.
    err = I2cMaster_WriteReg(bm8563_handle->i2c_handle, bm8563_handle->i2c_addr, 
                             BM8563_TIME_SEC_REG, data_buf, 7);
    if (ESP_OK != err) {
        return ESP_FAIL;   
    }
    return ESP_OK;

BM8563_SET_TIME_FORMAT_ERR:
    ESP_LOGE(TAG, "%s (%d) Time format error.", __FUNCTION__, __LINE__);
    return ESP_FAIL;
}

/**
  * @brief  BM8563 get time.
  * @param[out]  time BM8563 time struct pointer.
  * @param[in]  bm8563_handle  bm8563 operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BM8563_GetTime(BM8563_handle_t bm8563_handle, BM8563_Time_t* time)
{
    esp_err_t err = ESP_OK;
    uint8_t data_buf[7] = {0};
    uint8_t *p = (uint8_t*)time;

    BM8563_HANDLE_CHECK(bm8563_handle, ESP_FAIL);
    if (NULL == time) {
        return ESP_FAIL;
    }

    // Read raw time data.
    err = I2cMaster_WriteReg(bm8563_handle->i2c_handle, bm8563_handle->i2c_addr, 
                             BM8563_TIME_SEC_REG, data_buf, 7);
    if (ESP_OK != err) {
        return ESP_FAIL;
    }
    // Convert time data from bcd format to hex format.
    for (uint32_t i=0; i<7; i++) {
        p[i] = BM8563_BcdToHex(data_buf[i]);
    }
        
    // Mask unwanted bits.
    time->sec = BM8563_BcdToHex(data_buf[0] & 0x7f);
    time->min = BM8563_BcdToHex(data_buf[1] & 0x7f);
    time->hour = BM8563_BcdToHex(data_buf[2] & 0x3f);
    time->day = BM8563_BcdToHex(data_buf[3] & 0x3f);
    time->week = BM8563_BcdToHex(data_buf[4] & 0x07);
    time->month = BM8563_BcdToHex(data_buf[5] & 0x1f);
    time->year = BM8563_BcdToHex(data_buf[6] & 0xff);
    return ESP_OK;
}