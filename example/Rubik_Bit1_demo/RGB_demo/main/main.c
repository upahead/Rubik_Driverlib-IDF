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

#include "rubik_bit1_config.h"
#include "rgbled_driver.h"
#include "rgbled_color.h"
#include "rgbled_show.h"

void app_main(void)
{
    // Initialize 8 RGB LED. 
    RGBled_handle_t rgb = RGBled_Init(RGBLED_DEVICE_TYPE_SK6812, RUBIK_BIT1_SK6812_IO, 
                                         RUBIK_BIT1_SK6812_LEN, RMT_CHANNEL_0);
    if(NULL == rgb){
        printf("rgb init failed.");
    }

    // red.
    RGBled_SetAllOneColor(rgb, RGBLED_COLOR_RED);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    
    // green.
    RGBled_SetAllOneColor(rgb, RGBLED_COLOR_GREEN);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // blue.
    RGBled_SetAllOneColor(rgb, RGBLED_COLOR_BLUE);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // off.
    RGBled_SetAllOff(rgb);

    // Filled with white.
    RGBled_ShowFillIn(rgb, RGBLED_COLOR_WHITE, 200);

    while(1){   
        for (uint32_t i=0; i<10; i++) {
            RGBled_ShowColorRoll(rgb, Rainbow_Color, 100);
        }
        RGBled_ShowGradient(rgb, 10);
        printf("running......\n");
    } 
}


