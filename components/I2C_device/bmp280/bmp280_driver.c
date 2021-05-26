#include "bmp280_driver.h"
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "BMP280";

#define BMP280_HANDLE_CHECK(a, ret)  if (NULL == a) {                            \
        ESP_LOGE(TAG, "%s (%d) driver handle is NULL.", __FUNCTION__, __LINE__); \
        return (ret);                                                            \
        }

#define BMP280_PRESSURE_OSR			(BMP280_OVERSAMP_8X)
#define BMP280_TEMPERATURE_OSR		(BMP280_OVERSAMP_16X)
#define BMP280_MODE					(BMP280_PRESSURE_OSR << 2 | BMP280_TEMPERATURE_OSR << 5 | BMP280_NORMAL_MODE)

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
BMP280_handle_t BMP280_Init(I2cMaster_handle_t i2c_handle, uint8_t i2c_addr)
{
    esp_err_t err = ESP_OK;
    uint8_t tmp[10] = {0};

    if (NULL == i2c_handle) {
        ESP_LOGE(TAG, "%s (%d) i2c handle is not initialized.", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (i2c_handle->i2c_clk > 400000) {
        ESP_LOGE(TAG, "%s (%d) BMP280 I2C supports up to 400Kbit/s", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (false == I2CMaster_CheckDeviceAlive(i2c_handle, i2c_addr)) {
        ESP_LOGE(TAG, "%s (%d) device not alive.", __FUNCTION__, __LINE__);
        return NULL;
    }

    BMP280_handle_t bmp280_handle = malloc(sizeof(BMP280_t));
    if (NULL == bmp280_handle) {
        ESP_LOGE(TAG, "%s (%d) driver handle malloc failed.", __FUNCTION__, __LINE__);
        return NULL;
    }
    bmp280_handle->i2c_handle = i2c_handle;
    bmp280_handle->i2c_addr = i2c_addr;

    // Read calibration data.
    err = I2cMaster_ReadReg(i2c_handle, i2c_addr, BMP280_DIG_T1_LSB_REG, (uint8_t*)&(bmp280_handle->bmp280Cal), 24);
    if (ESP_OK != err) {
        goto BMP280_INIT_FAILED;
    }
    tmp[0] = BMP280_MODE;
    err = I2cMaster_WriteReg(i2c_handle, i2c_addr, BMP280_CTRLMEAS_REG, tmp, 1);
    if (ESP_OK != err) {
        goto BMP280_INIT_FAILED;
    }
    tmp[0] = (5<<2);
    err = I2cMaster_WriteReg(i2c_handle, i2c_addr, BMP280_CONFIG_REG, tmp, 1);
    if (ESP_OK != err) {
        goto BMP280_INIT_FAILED;
    }

    ESP_LOGI(TAG, "%s (%d) bmp280 init ok.", __FUNCTION__, __LINE__);
    return bmp280_handle;

BMP280_INIT_FAILED:
    free(bmp280_handle);
    return NULL;
}

/**
  * @brief  BMP280 deinitialization.
  * @param[in]  bmp280_handle  bmp280 operation handle pointer.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */
esp_err_t BMP280_Deinit(BMP280_handle_t* bmp280_handle)
{
    BMP280_HANDLE_CHECK(*bmp280_handle, ESP_FAIL);

    free(*bmp280_handle);
    *bmp280_handle = NULL;
    ESP_LOGI(TAG, "%s (%d) bmp280 handle deinit ok.", __FUNCTION__, __LINE__);
    return ESP_OK;
}

// Get the raw data.
static void BMP280GetPressure(BMP280_handle_t bmp280_handle)
{
    uint8_t data[BMP280_DATA_FRAME_SIZE];

    // read data from sensor
    I2cMaster_ReadReg(bmp280_handle->i2c_handle, bmp280_handle->i2c_addr, 
                      BMP280_PRESSURE_MSB_REG, (uint8_t*)&data, BMP280_DATA_FRAME_SIZE);
    bmp280_handle->bmp280RawPressure = (int32_t)((((uint32_t)(data[0])) << 12) | (((uint32_t)(data[1])) << 4) | ((uint32_t)data[2] >> 4));
    bmp280_handle->bmp280RawTemperature = (int32_t)((((uint32_t)(data[3])) << 12) | (((uint32_t)(data[4])) << 4) | ((uint32_t)data[5] >> 4));
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of "5123" equals 51.23 DegC
// t_fine carries fine temperature as global value
static int32_t BMP280CompensateT(BMP280_handle_t bmp280_handle)
{
    int32_t var1=0, var2=0, T=0;
    int32_t adcT = bmp280_handle->bmp280RawTemperature;

    var1 = ((((adcT >> 3) - ((int32_t)(bmp280_handle->bmp280Cal).dig_T1 << 1))) * ((int32_t)(bmp280_handle->bmp280Cal).dig_T2)) >> 11;
    var2  = (((((adcT >> 4) - ((int32_t)(bmp280_handle->bmp280Cal).dig_T1)) * ((adcT >> 4) - ((int32_t)(bmp280_handle->bmp280Cal).dig_T1))) >> 12) * ((int32_t)(bmp280_handle->bmp280Cal).dig_T3)) >> 14;
    (bmp280_handle->bmp280Cal).t_fine = var1 + var2;

    T = ((bmp280_handle->bmp280Cal).t_fine * 5 + 128) >> 8;

    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of "24674867" represents 24674867/256 = 96386.2 Pa = 963.862 hPa
static uint32_t BMP280CompensateP(BMP280_handle_t bmp280_handle)
{
    int32_t adcP = bmp280_handle->bmp280RawPressure;
    int64_t var1=0, var2=0, p=0;
    var1 = ((int64_t)(bmp280_handle->bmp280Cal).t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)(bmp280_handle->bmp280Cal).dig_P6;
    var2 = var2 + ((var1*(int64_t)(bmp280_handle->bmp280Cal).dig_P5) << 17);
    var2 = var2 + (((int64_t)(bmp280_handle->bmp280Cal).dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)(bmp280_handle->bmp280Cal).dig_P3) >> 8) + ((var1 * (int64_t)(bmp280_handle->bmp280Cal).dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)(bmp280_handle->bmp280Cal).dig_P1) >> 33;
    if (var1 == 0)
        return 0;
    p = 1048576 - adcP;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)(bmp280_handle->bmp280Cal).dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)(bmp280_handle->bmp280Cal).dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)(bmp280_handle->bmp280Cal).dig_P7) << 4);
    return (uint32_t)p;
}

#define CONST_PF 0.1902630958	//(1/5.25588f) Pressure factor
#define FIX_TEMP 25				// Fixed Temperature. ASL is a function of pressure and temperature, but as the temperature changes so much (blow a little towards the flie and watch it drop 5 degrees) it corrupts the ASL estimates.
								// TLDR: Adjusting for temp changes does more harm than good.
/**
 * Converts pressure to altitude above sea level (ASL) in meters
 */
static float BMP280PressureToAltitude(float* pressure/*, float* groundPressure, float* groundTemp*/)
{
    if (*pressure > 0){
        return ((pow((1015.7f / *pressure), CONST_PF) - 1.0f) * (FIX_TEMP + 273.15f)) / 0.0065f;
    }else{
        return 0;
    }
}

#define FILTER_NUM	5
#define FILTER_A	0.1f

/* Limiting Average Filtering Method. */
static void presssureFilter(float* in, float* out)
{
	static uint8_t i = 0;
	static float filter_buf[FILTER_NUM] = {0.0};
	double filter_sum = 0.0;
	uint8_t cnt = 0;
	float deta;

	if(filter_buf[i] == 0.0f){
		filter_buf[i] = *in;
		*out = *in;
		if(++i >= FILTER_NUM)
            i=0;
	}else{
		if(i)
            deta = *in-filter_buf[i - 1];
		else
            deta = *in-filter_buf[FILTER_NUM - 1];

		if(fabs(deta) < FILTER_A){
			filter_buf[i] = *in;
			if(++i >= FILTER_NUM)
                i = 0;
		}
		for(cnt = 0; cnt < FILTER_NUM; cnt++){
			filter_sum += filter_buf[cnt];
		}
		*out = filter_sum / FILTER_NUM;
	}
}

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
esp_err_t BMP280_GetData(BMP280_handle_t bmp280_handle, float* pressure, float* temperature, float* asl)
{
    static float t=0;
    static float p=0;

    BMP280_HANDLE_CHECK(bmp280_handle, ESP_FAIL);
    if (pressure==NULL || temperature==NULL || asl==NULL) {
        return ESP_FAIL;
    }

	BMP280GetPressure(bmp280_handle);
	t = BMP280CompensateT(bmp280_handle) / 100.0;
	p = BMP280CompensateP(bmp280_handle) / 25600.0;
	presssureFilter(&p, pressure);
	*temperature = (float)t; /* Celsius */
	*asl = BMP280PressureToAltitude(pressure);	/* Converted to altitude value */
    return ESP_OK;
}