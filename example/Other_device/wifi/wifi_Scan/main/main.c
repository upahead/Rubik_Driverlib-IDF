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

void app_main(void)
{
    printf("wifi Scan Examples\n");
    
    wifi_scan_list_t ap_list[10];
    uint8_t ap_count=0;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) { 
      ESP_ERROR_CHECK(nvs_flash_erase());   
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    WIFI_Init();
    ap_count = WIFI_Scan(ap_list, 15);
    for(int i=0; i<ap_count; i++){
        printf("%s\t\t\t\t%d\n", ap_list[i].SSID, ap_list[i].RSSI);
    }
    
    while(1){
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        printf("task running...\n");
    }
}
