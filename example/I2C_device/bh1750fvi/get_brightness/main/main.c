#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"
#include "bh1750fvi_driver.h"

#define BH1750FVI_ADDR     0x23

void app_main(void)
{
    float brightness;
    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    BH1750FVI_handle_t bh1750fvi = BH1750FVI_Init(i2c_0, BH1750FVI_ADDR);

    while(1){
        BH750FVI_GetData(bh1750fvi, &brightness);
        printf("brightness = %f .\n", brightness);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
