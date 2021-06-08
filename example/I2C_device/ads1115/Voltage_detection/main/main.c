#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"
#include "ads1115_driver.h"

#define ADS1115_ADDR     0x48

void app_main(void)
{
    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    ADS1115_handle_t ads1115 = ADS1115_Init(i2c_0, ADS1115_ADDR);

    ADS1115_SetPga(ads1115, ADS1115_REG_CONFIG_PGA_FSR_6114);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    while(1){
        ADS1115_SetMux(ads1115, ADS1115_REG_CONFIG_MUX_SING_0);
        printf("ads1115 channel 0 voltage = %f .\n", ADS1115_GetVoltageOnce(ads1115));
        vTaskDelay(200 / portTICK_PERIOD_MS);
        ADS1115_SetMux(ads1115, ADS1115_REG_CONFIG_MUX_SING_1);
        printf("ads1115 channel 1 voltage = %f .\n", ADS1115_GetVoltageOnce(ads1115));
        vTaskDelay(200 / portTICK_PERIOD_MS);
        ADS1115_SetMux(ads1115, ADS1115_REG_CONFIG_MUX_SING_2);
        printf("ads1115 channel 2 voltage = %f .\n", ADS1115_GetVoltageOnce(ads1115));
        vTaskDelay(200 / portTICK_PERIOD_MS);
        ADS1115_SetMux(ads1115, ADS1115_REG_CONFIG_MUX_SING_3);
        printf("ads1115 channel 3 voltage = %f .\n", ADS1115_GetVoltageOnce(ads1115));

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
