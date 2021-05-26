#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"
#include "bmp280_driver.h"

#define BMP280_ADDR     0x77

void app_main(void)
{
    float P,T,ALT;

    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    BMP280_handle_t bmp280 = BMP280_Init(i2c_0, BMP280_ADDR);
    BMP280_Deinit(&bmp280);
    bmp280 = BMP280_Init(i2c_0, BMP280_ADDR);

    while(1){
        BMP280_GetData(bmp280, &P, &T, &ALT);
        printf("Air pressure: %0.4f hPa\n",P);
        printf("Temperature: %0.2f ℃\n",T);
        printf("Altitude: %0.2f m\n",ALT);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
