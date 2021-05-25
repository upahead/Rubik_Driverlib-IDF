#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void app_main(void)
{
    printf("hello esp32!.\n");
    while(1){        
        printf("task running... \n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
