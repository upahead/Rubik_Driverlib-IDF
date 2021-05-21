#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"


#define PAJ_ADDR     0x73

void app_main(void)
{
    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    I2cMaster_Deinit(&i2c_0);
    i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    if (true == I2CMaster_CheckDeviceAlive(i2c_0, PAJ_ADDR)) {
        printf("PAJ_ADDR alive .\n");
    }

    uint8_t send_buf = 0x55;
    uint8_t recv_buf = 0x00;
    if(ESP_OK != I2cMaster_WriteReg(i2c_0, PAJ_ADDR, 0x6B, &send_buf, 1))
        printf("write failed.\n");
    if(ESP_OK != I2cMaster_ReadReg(i2c_0, PAJ_ADDR, 0x6B, &recv_buf, 1)){
        printf("read failed.\n");
    }
    printf("recv buf = 0x%x .\n", recv_buf);

    while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
