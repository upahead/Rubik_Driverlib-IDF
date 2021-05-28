#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"
#include "pca9554_driver.h"

#define PCA9554_ADDR     0x20

void app_main(void)
{
    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    PCA9554_handle_t pca9554 = PCA9554_Init(i2c_0, PCA9554_ADDR);

    PCA9554_ConfigPinDirection(pca9554, PCA9554_PIN_0, PCA9554_PIN_OUTPUT);
    PCA9554_ConfigPinDirection(pca9554, PCA9554_PIN_1, PCA9554_PIN_OUTPUT);

    while(1){
        PCA9554_SetPinLevel(pca9554, PCA9554_PIN_0, PCA9554_PIN_LEVEL_HIG);
        PCA9554_SetPinLevel(pca9554, PCA9554_PIN_0, PCA9554_PIN_LEVEL_LOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        PCA9554_SetPinLevel(pca9554, PCA9554_PIN_0, PCA9554_PIN_LEVEL_LOW);
        PCA9554_SetPinLevel(pca9554, PCA9554_PIN_0, PCA9554_PIN_LEVEL_HIG);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("task runnig...\n");
    }
}
