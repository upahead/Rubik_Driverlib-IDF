#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"
#include "bm8563_driver.h"

#define BM8563_ADDR     0x51

void app_main(void)
{
    BM8563_Time_t time1;
    time1.sec = 1;
    time1.min = 1;
    time1.hour = 1;
    time1.day = 1;
    time1.week = 1;
    time1.month = 1;
    time1.year = 21;
    BM8563_Time_t time2;

    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    BM8563_handle_t bm8563 = BM8563_Init(i2c_0, BM8563_ADDR);

    BM8563_SetTime(bm8563, time1);

    while(1){
        BM8563_GetTime(bm8563, &time2);
        printf("now time : %d-%d-%d %d-%d-%d Week %d \n", time2.year, time2.month, time2.day, time2.hour, time2.min, time2.sec, time2.week);
        vTaskDelay(1000 / portTICK_PERIOD_MS);  
    }
}
