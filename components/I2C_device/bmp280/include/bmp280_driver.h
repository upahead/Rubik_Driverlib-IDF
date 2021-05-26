#ifndef __BMP280_DRIVER_H__
#define __BMP280_DRIVER_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "../../i2c_master/include/i2c_master.h"
#include "bmp280_reg.h"

typedef struct{
    uint16_t  dig_T1; /* calibration T1 data */
    int16_t   dig_T2; /* calibration T2 data */
    int16_t   dig_T3; /* calibration T3 data */
    uint16_t  dig_P1; /* calibration P1 data */
    int16_t   dig_P2; /* calibration P2 data */
    int16_t   dig_P3; /* calibration P3 data */
    int16_t   dig_P4; /* calibration P4 data */
    int16_t   dig_P5; /* calibration P5 data */
    int16_t   dig_P6; /* calibration P6 data */
    int16_t   dig_P7; /* calibration P7 data */
    int16_t   dig_P8; /* calibration P8 data */
    int16_t   dig_P9; /* calibration P9 data */
    int32_t   t_fine; /* calibration t_fine data */
}bmp280Calib;

typedef struct {
    I2cMaster_handle_t i2c_handle;    
    uint8_t i2c_addr;     
    bmp280Calib  bmp280Cal;
    int32_t bmp280RawPressure;
    int32_t bmp280RawTemperature;
} BMP280_t;
typedef BMP280_t *BMP280_handle_t;

/**
  * @brief  Initialize the BMP280 and obtain an operation handle.
  * @param[in]  i2c_handle  i2c master operation handle.
  * @param[in]  i2c_addr  i2c slave address(7bit).
  *                       Under normal circumstances it is 0X77.
  * @retval  
  *         successful  bmp280 operation handle.
  *         failed      NULL.
  * @note  Initialize and obtain calibration data.
  * @note  Use BMP280_Deinit() to release it.
  */
BMP280_handle_t BMP280_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr);

/**
  * @brief  BMP280 deinitialization.
  * @param[in]  bmp280_handle  bmp280 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BMP280_Deinit(BMP280_handle_t* bmp280_handle);

/**
  * @brief  BMP280 collect data once.
  * @param[in]  bmp280_handle  bmp280 operation handle pointer.
  * @param[out]  pressure  Barometric pressure.(hPa)
  * @param[out]  temperature  Temperature value.(℃)
  * @param[out]  asl  Altitude value.(m)
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BMP280_GetData(BMP280_handle_t bmp280_handle, float* pressure, float* temperature, float* asl);

#endif /* __BMP280_DRIVER_H__ */