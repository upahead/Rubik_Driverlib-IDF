#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"
#include "axp192_driver.h"

#define AXP192_ADDR     0x34

void app_main(void)
{
    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    AXP192_handle_t axp192 = AXP192_Init(i2c_0, AXP192_ADDR);

    if (AXP192_CheckAcinExist(axp192)){
        printf("ACIN exist.\n");
        printf("ACIN voltage = %f.\n", AXP192_GetAcinVal(axp192));
    } else {
        printf("ACIN not exist.\n");
    }

    if (AXP192_CheckVbusExist(axp192)){
        printf("VBUS exist.\n");
        printf("VBUS voltage = %f.\n", AXP192_GetVbusVal(axp192));
    } else {
        printf("VBUS not exist.\n");
    }

    if (AXP192_CheckBatteryExist(axp192)) {
        printf("battery exist.\n");
        printf("battery voltage = %f.\n", AXP192_GetBatteryVal(axp192));
    } else {
        printf("battery not exist.\n");
    }

    if (AXP192_CheckBatteryCharge(axp192)){
        printf("battery in the charging.\n");
    } else {
        printf("The battery is not charging.\n");
    }

    printf("AXP192 internal temperature = %f.\n", AXP192_GetInternalTemperature(axp192));

    printf("AXP192 battery level = %d.\n", AXP192_GetBatteryValPower(axp192));

    AXP192_BatteryChargeControl(axp192, true);
    printf("Battery charging on.\n");

    AXP192_BackupChargeControl(axp192, true);
    printf("Backup battery chargin on.\n");

    while(1) {
        printf("task running...\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
