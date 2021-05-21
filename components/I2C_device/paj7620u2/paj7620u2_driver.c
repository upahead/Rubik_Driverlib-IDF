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
  * @file           paj7620u2_driver.c
  * @version        1.0
  * @date           2021-5-21
  */

#include "paj7620u2_driver.h"
#include "paj7620u2_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "PAJ7620U2";

#define PAJ7620U2_HANDLE_CHECK(a, ret)  if (NULL == a) {                         \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

/**
  * @brief  select PAJ7620U2 bank partition.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @param[in]  bank  PAJ7620 bank 0 or 1.
  * @retval  reference esp_err_t.
  */
static esp_err_t paj7620u2_select_bank(PAJ7620U2_handle_t paj7620u2_handle, PAJ7620U2_Bank_t bank)
{
    esp_err_t ret = ESP_OK; 
    uint8_t bank_0 = PAJ7620U2_BANK0;
    uint8_t bank_1 = PAJ7620U2_BANK1;
    
    if (PAJ7620U2_BANK_0 == bank) {
        ret = I2cMaster_WriteReg(paj7620u2_handle->i2c_handle, paj7620u2_handle->i2c_addr, 
                                    PAJ7620U2_REGITER_BANK_SEL, &bank_0, 1);
    } else if (PAJ7620U2_BANK_1 == bank) {
        ret = I2cMaster_WriteReg(paj7620u2_handle->i2c_handle, paj7620u2_handle->i2c_addr, 
                                    PAJ7620U2_REGITER_BANK_SEL, &bank_1, 1);
    } else {
        return ESP_FAIL;
    }
    return ret;
}

/**
  * @brief  paj7620u2 i2c wake up.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @retval  reference esp_err_t.
  */
static esp_err_t paj7620u2_i2c_wakeup(PAJ7620U2_handle_t paj7620u2_handle)
{   
    esp_err_t ret = ESP_OK; 
        
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (paj7620u2_handle->i2c_addr << 1)|I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin((paj7620u2_handle->i2c_handle)->i2c_port, cmd, 100/portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
  * @brief  PAJ7620U2 wake up.
  *         wake up and select bank 0.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
static esp_err_t paj7620u2_wakeup(PAJ7620U2_handle_t paj7620u2_handle)
{
    esp_err_t ret = ESP_OK;
    uint8_t data_buf = 0x00;
    
    paj7620u2_i2c_wakeup(paj7620u2_handle);               
    vTaskDelay(10 / portTICK_PERIOD_MS);
    paj7620u2_i2c_wakeup(paj7620u2_handle);               
    vTaskDelay(10 / portTICK_PERIOD_MS);
    paj7620u2_select_bank(paj7620u2_handle, PAJ7620U2_BANK_0);
    ret = I2cMaster_ReadReg(paj7620u2_handle->i2c_handle, paj7620u2_handle->i2c_addr, 
                            PAJ7620U2_STATUS_REG, &data_buf, 1);
    if (ESP_OK != ret) {
        return ESP_FAIL; 
    }
    if (0x20 != data_buf) {     // wake up failed.
        return ESP_FAIL;
    }                 
    return ESP_OK;
}

/**
  * @brief  Initialize the PAJ7620U2 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                      Under normal circumstances it is 0X73.
  * @retval  
  *         successful  paj7620u2 operation handle.
  *         failed      NULL.
  * @note  Use PAJ7620U2_Deinit() to release it.
  */
PAJ7620U2_handle_t PAJ7620U2_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr)
{   
    esp_err_t err = ESP_OK;
    uint8_t reg_buf = 0x00;
    uint8_t val_buf = 0x00;

    if (NULL == i2c_handle) {
        ESP_LOGE(TAG, "%s (%d) i2c handle is not initialized.", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (i2c_handle->i2c_clk > 400000) {
        ESP_LOGE(TAG, "%s (%d) PAJ7620U2 I2C supports up to 400Kbit/s", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (false == I2CMaster_CheckDeviceAlive(i2c_handle, i2c_addr)) {
        ESP_LOGE(TAG, "%s (%d) device not alive.", __FUNCTION__, __LINE__);
        return NULL;
    }

    PAJ7620U2_handle_t paj7620u2_handle = malloc(sizeof(PAJ7620U2_t));
    if (NULL == paj7620u2_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    paj7620u2_handle->i2c_handle = i2c_handle;
    paj7620u2_handle->i2c_addr = i2c_addr;

    err = paj7620u2_wakeup(paj7620u2_handle);
    if (ESP_OK != err) {
        ESP_LOGE(TAG, "%s (%d) paj7620u2 wake up failed.", __FUNCTION__, __LINE__);
        goto PAJ7620U2_INIT_FAILED;
    }
    paj7620u2_select_bank(paj7620u2_handle, PAJ7620U2_BANK_0);
    for (uint32_t i=0; i<sizeof(init_array)/2; i++) {
        reg_buf = init_array[i][0];
        val_buf = init_array[i][1];
        err = I2cMaster_WriteReg(paj7620u2_handle->i2c_handle, paj7620u2_handle->i2c_addr, 
                                 reg_buf, &val_buf, 1);    
        if (ESP_OK != err) {
            ESP_LOGE(TAG, "%s (%d) paj7620u2 send init array failed.", __FUNCTION__, __LINE__);
            goto PAJ7620U2_INIT_FAILED;
        }
    }
    paj7620u2_select_bank(paj7620u2_handle, PAJ7620U2_BANK_0);
    ESP_LOGI(TAG, "%s (%d) paj7620u2 init ok.", __FUNCTION__, __LINE__);
    return paj7620u2_handle;

PAJ7620U2_INIT_FAILED:
    free(paj7620u2_handle);
    return NULL;
}

/**
  * @brief  PAJ7620U2 deinitialization.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  *                     May be caused by no initialization.
  */
esp_err_t PAJ7620U2_Deinit(PAJ7620U2_handle_t* paj7620u2_handle)
{
    PAJ7620U2_HANDLE_CHECK(*paj7620u2_handle, ESP_FAIL);

    free(*paj7620u2_handle);
    *paj7620u2_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) paj7620u2 handle deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

/**
  * @brief  PAJ7620U2 initialize to gesture recognition mode.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t PAJ7620U2_GestureInit(PAJ7620U2_handle_t paj7620u2_handle)
{ 
    esp_err_t err = ESP_OK;
    uint8_t reg_buf = 0x00;
    uint8_t val_buf = 0x00;

    PAJ7620U2_HANDLE_CHECK(paj7620u2_handle, ESP_FAIL);
        
    paj7620u2_select_bank(paj7620u2_handle, PAJ7620U2_BANK_0);
    for (uint32_t i=0; i<sizeof(gesture_array)/2; i++) {
        reg_buf = gesture_array[i][0];
        val_buf = gesture_array[i][1];
        err = I2cMaster_WriteReg(paj7620u2_handle->i2c_handle, paj7620u2_handle->i2c_addr, 
                                 reg_buf, &val_buf, 1);    
        if (ESP_OK != err) {
            ESP_LOGE(TAG, "%s (%d) paj7620u2 send gesture init array failed.", __FUNCTION__, __LINE__);
            return ESP_FAIL;
        }
    }
    paj7620u2_select_bank(paj7620u2_handle, PAJ7620U2_BANK_0);
    return ESP_OK;
}

/**
  * @brief  PAJ7620U2 Get gesture status.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @param[out]  gesture_status  gesture status.
  * @note  Make sure to perform gesture recognition initialization before using 
  *        this function.
  */
esp_err_t PAJ7620U2_GestureGetState(PAJ7620U2_handle_t paj7620u2_handle, uint16_t* gesture_status)
{
    esp_err_t err = ESP_OK;
    uint8_t data_buf[2] = {0};

    PAJ7620U2_HANDLE_CHECK(paj7620u2_handle, ESP_FAIL);
    if (NULL == gesture_status) {
        return ESP_FAIL;
    }

    I2cMaster_ReadReg(paj7620u2_handle->i2c_handle, paj7620u2_handle->i2c_addr, 
                      PAJ7620U2_GET_INT_FLAG1, data_buf, 2);
    if(ESP_OK != err){
        return ESP_FAIL;
    }
    *gesture_status = (uint16_t)data_buf[1]<<8 | data_buf[0];
    return ESP_OK;
}

/**
  * @brief  PAJ7620U2 initialize to object proximity measurement mode.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t PAJ7620U2_ApproachInit(PAJ7620U2_handle_t paj7620u2_handle)
{
    esp_err_t err = ESP_OK;
    uint8_t reg_buf = 0x00;
    uint8_t val_buf = 0x00;

    PAJ7620U2_HANDLE_CHECK(paj7620u2_handle, ESP_FAIL);

    paj7620u2_select_bank(paj7620u2_handle, PAJ7620U2_BANK_0);
    for (uint32_t i=0; i<sizeof(proximity_array)/2; i++) {
        reg_buf = proximity_array[i][0];
        val_buf = proximity_array[i][1];
        err = I2cMaster_WriteReg(paj7620u2_handle->i2c_handle, paj7620u2_handle->i2c_addr, 
                                 reg_buf, &val_buf, 1);    
        if (ESP_OK != err) {
            ESP_LOGE(TAG, "%s (%d) paj7620u2 send gesture init array failed.", __FUNCTION__, __LINE__);
            return ESP_FAIL;
        }
    }
    paj7620u2_select_bank(paj7620u2_handle, PAJ7620U2_BANK_0);
    return ESP_OK;
}

/**
  * @brief  PAJ7620U2 Obtain object proximity measurement data.
  * @param[in]  paj7620u2_handle  paj7620u2 operation handle.
  * @param[out]  obj_brightness  Object brightness value (0~255).
  * @param[out]  obj_size  Object size value (0~900).
  * @note  Make sure to perform object proximity measurement mode initialization 
  *        before using this function.
  */
esp_err_t PAJ7620U2_ApproachGetData(PAJ7620U2_handle_t paj7620u2_handle, uint8_t *obj_brightness, 
                               uint16_t *obj_size)
{
    esp_err_t err = ESP_OK;
    uint8_t data_buf[3] = {0};

    PAJ7620U2_HANDLE_CHECK(paj7620u2_handle, ESP_FAIL);
    if (NULL == obj_brightness || NULL == obj_size) {
        return ESP_FAIL;
    }

    err = I2cMaster_ReadReg(paj7620u2_handle->i2c_handle, paj7620u2_handle->i2c_addr, 
                      PAJ7620U2_GET_OBJECT_BRIGHTNESS, data_buf, 3);
    if(ESP_OK != err){
        return ESP_FAIL;
    }
    *obj_brightness = data_buf[0];
    *obj_size = ((uint16_t)data_buf[2] & 0x0f)<<8 | data_buf[1];
    return ESP_OK;
}
