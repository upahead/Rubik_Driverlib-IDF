#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "driver/rmt.h"

#include "rgbled_driver.h"
#include "rgbled_color.h"
#include "rgbled_show.h"

void app_main(void)
{
    RGBled_handle_t sk6812 = RGBled_Init(RGBLED_DEVICE_TYPE_SK6812, 5, 1, RMT_CHANNEL_0);
    if(NULL == sk6812){
        printf("sk6812 init failed.");
    }

    // red.
    RGBled_SetAllOneColor(sk6812, RGBLED_COLOR_RED);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    // green.
    RGBled_SetAllOneColor(sk6812, RGBLED_COLOR_GREEN);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // blue.
    RGBled_SetAllOneColor(sk6812, RGBLED_COLOR_BLUE);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    RGBled_SetAllOff(sk6812);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    while(1){   
        RGBled_ShowGradient(sk6812, 10);
        printf("running......\n");
    } 
}


