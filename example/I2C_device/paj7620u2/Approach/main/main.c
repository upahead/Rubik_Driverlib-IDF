#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"
#include "paj7620u2_driver.h"

#define PAJ_ADDR     0x73

void app_main(void)
{
    uint8_t obj_brightness=0;
    uint16_t obj_size=0;

    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    PAJ7620U2_handle_t paj = PAJ7620U2_Init(i2c_0, PAJ_ADDR);

    PAJ7620U2_ApproachInit(paj);
    while(1){
        PAJ7620U2_ApproachGetData(paj, &obj_brightness, &obj_size);
        printf("obj brightness = %d   obj size = %d   \n", obj_brightness, obj_size);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
