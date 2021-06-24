#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/uart.h"

#include "wifi_driver.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#define STA_SSID  "Rubik"
#define STA_PASS  "123456789"

void app_main(void)
{
    printf("wifi STA Examples\n");

    esp_err_t ret = nvs_flash_init();              
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());   
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    WIFI_Init();
    WIFI_Init_Sta(STA_SSID, STA_PASS);

    printf("wifi STA SSID : %s .\n", STA_SSID);
    printf("wifi STA PASS : %s .\n", STA_PASS);
    
    while(1){
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        printf("task running...\n");
    }
}

