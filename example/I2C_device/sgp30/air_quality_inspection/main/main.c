#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"
#include "sgp30_driver.h"

#define SGP30_ADDR     0x58

void app_main(void)
{
    uint16_t co2=0,tvoc=0;

    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    SGP30_handle_t sgp30 = SGP30_Init(i2c_0, SGP30_ADDR);

    while(1){
        SGP30_StartMessure(sgp30);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        SGP30_GetValue(sgp30 ,&co2, &tvoc);
        printf("co2 = %d, tvoc = %d\n", co2, tvoc);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
