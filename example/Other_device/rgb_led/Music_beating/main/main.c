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
#include "driver/adc.h"

#include "rgbled_driver.h"
#include "rgbled_color.h"
#include "rgbled_show.h"

uint8_t new_light_num = 0;
uint8_t cur_light_num = 0;

static void MicCollectTask(void *pvParameter);

void app_main(void)
{
    RGBled_Color_t light_color = 0xff00;
    uint8_t rgb_range = 0;
    uint8_t color_loop = 20;

    RGBled_handle_t rgbled = RGBled_Init(RGBLED_DEVICE_TYPE_WS2812, 27, 60, RMT_CHANNEL_0);
    if(NULL == rgbled){
        printf("rgbled init failed.");
    }
    RGBled_handle_t rgbled_1 = RGBled_Init(RGBLED_DEVICE_TYPE_WS2812, 32, 60, RMT_CHANNEL_0);
    RGBled_handle_t rgbled_2 = RGBled_Init(RGBLED_DEVICE_TYPE_WS2812, 33, 60, RMT_CHANNEL_0);

    xTaskCreatePinnedToCore(MicCollectTask, "MicCollectTask", 4096*2, NULL, 0, NULL, 1);

    while(1){  
        color_loop--; 
        if(color_loop == 0) {
            rgb_range = rand()%9;
            if(rgb_range == 0)
                light_color = 0xFF0000;
            else if(rgb_range == 1)
                light_color = 0xFF8000;
            else if(rgb_range == 2)
                light_color = 0xFFFF00;
            else if(rgb_range == 3)
                light_color = 0x99FF00;
            else if(rgb_range == 4)
                light_color = 0x00FF00;
            else if(rgb_range == 5)
                light_color = 0x00FFB3;
            else if(rgb_range == 6)
                light_color = 0x00FFFF;
            else if(rgb_range == 7)
                light_color = 0x0000FF;
            else if(rgb_range == 8)
                light_color = 0x8000FF;
            else
                light_color = 0xFF0080;
            color_loop = 100;    
        }
#if 0
        if(cur_light_num < new_light_num){
            RGBled_SetLenColor(rgbled, light_color, cur_light_num++);
        }else if(cur_light_num > new_light_num){
            RGBled_SetLenColor(rgbled, light_color, cur_light_num--);
        }
#elif 1
        if(cur_light_num < new_light_num){
            RGBled_SetLenColorMid(rgbled, light_color, cur_light_num++);
            RGBled_SetLenColorMid(rgbled_1, light_color, cur_light_num-5);
            RGBled_SetLenColorMid(rgbled_2, light_color, cur_light_num-5);
        }else if(cur_light_num > new_light_num){
            RGBled_SetLenColorMid(rgbled, light_color, cur_light_num--);
            RGBled_SetLenColorMid(rgbled_1, light_color, cur_light_num-5);
            RGBled_SetLenColorMid(rgbled_2, light_color, cur_light_num-5);
        }
            
#endif 
        vTaskDelay(10 / portTICK_PERIOD_MS);
    } 
}

// 麦克风信号采集任务
static void MicCollectTask(void *pvParameter)
{
    uint16_t mic_val = 0;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    while(1){
        vTaskDelay(30 / portTICK_PERIOD_MS);
        mic_val = adc1_get_raw(ADC1_CHANNEL_6)-1500;
        // printf("mic val = %d .\n", mic_val);

        // vTaskDelay(10 / portTICK_PERIOD_MS);
        // mic_val = 0;
        // for(uint8_t i=0; i<5; i++){
        //     mic_val += adc1_get_raw(ADC1_CHANNEL_6)-1500;
        //     vTaskDelay(20 / portTICK_PERIOD_MS);
        // }
        // mic_val /= 5;

        if(mic_val > 2595 || mic_val <= 0){
            new_light_num = 0;
            continue;
        }
        new_light_num = (mic_val/((4095-1500)/60)*2) - 10;
        if(new_light_num > 60){
            new_light_num = 0;
        }
        // printf("light num = %d .\n", new_light_num);
    }
}




