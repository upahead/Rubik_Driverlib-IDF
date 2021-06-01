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

#include "combi_keys_driver.h"

void app_main(void)
{
    CombiKey_Value_t key_value = 0;

    CombiKey_handle_t keys = CombiKeys_Init(36, 37, 38);

    CombiKeys_SetLongPress(keys, 1000);

    while(1){
        key_value = CombiKeys_GetValue(keys);
        switch(key_value){
            case COMBI_KEY_UP_VALUE_PRESS: 
                printf("key up press.\n");
                break;
            case COMBI_KEY_UP_VALUE_SHORT_PRESS: 
                printf("key up short press.\n");
                break;
            case COMBI_KEY_UP_VALUE_LONG_PRESS: 
                printf("key up long press.\n");
                break;
            case COMBI_KEY_MID_VALUE_PRESS: 
                printf("key mid press.\n");
                break;
            case COMBI_KEY_MID_VALUE_SHORT_PRESS: 
                printf("key mid short press.\n");
                break;
            case COMBI_KEY_MID_VALUE_LONG_PRESS: 
                printf("key mid long press.\n");
                break;
            case COMBI_KEY_DOUN_VALUE_PRESS: 
                printf("key down press.\n");
                break;
            case COMBI_KEY_DOUN_VALUE_SHORT_PRESS: 
                printf("key down short press.\n");
                break;
            case COMBI_KEY_DOUN_VALUE_LONG_PRESS: 
                printf("key down long press.\n");
                break;
            default :
                printf("no keys press.\n");
                break;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
        printf("running......\n");
    }
}


