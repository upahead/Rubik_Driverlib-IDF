#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"
#include "tcs34725_driver.h"

#define TCS34725_ADDR     0x29

void app_main(void)
{
    uint16_t red=0, green=0, blue=0, clear=0;

    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    TCS34725_handle_t tcs34725 = TCS34725_Init(i2c_0, TCS34725_ADDR);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    while(1){
        TCS34725_GetRawData(tcs34725, &red, &green, &blue, &clear);
        printf("red = %d green = %d blue = %d clear = %d\n", red, green, blue, clear);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
