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
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "cJSON.h"

#include "rgbled_driver.h"
#include "rgbled_color.h"
#include "rgbled_show.h"
#include "wifi_driver.h"
#include "tcp_driver.h"

#define WIFI_AP_SSID        "Rubik wifi"
#define WIFI_AP_PASS        "88888888"

#define TCP_SERVER_PORT     9999

RGBled_Color_t rgb_json_get_color(char* json_buf);

void app_main(void)
{
    char recv_buf[1024] = { 0 };

    esp_err_t ret = nvs_flash_init();               
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) { 
      ESP_ERROR_CHECK(nvs_flash_erase());             
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    RGBled_handle_t rgb_bar = RGBled_Init(RGBLED_DEVICE_TYPE_WS2812, 27, 60, RMT_CHANNEL_0);
    if(NULL == rgb_bar){
        printf("ws2812 init failed.");
    }

    WIFI_Init();
    WIFI_Init_Softap(WIFI_AP_SSID, WIFI_AP_PASS);

    uint32_t tcp_socket;
    TCP_CreateServer(&tcp_socket, TCP_SERVER_PORT);
    
    while(1){   
        TCP_SocketRecvString(tcp_socket, recv_buf, 1024);
        printf("recv buf: \n %s \n", recv_buf);
        RGBled_SetAllOneColor(rgb_bar, rgb_json_get_color(recv_buf));
        vTaskDelay(100 / portTICK_PERIOD_MS);
        printf("running......\n");
    } 
}

RGBled_Color_t rgb_json_get_color(char* json_buf)
{
    uint8_t r,g,b;

    cJSON* p_root = cJSON_Parse(json_buf);
    cJSON* p_color = cJSON_GetObjectItem(p_root, "Color");
    cJSON* p_red = cJSON_GetObjectItem(p_color, "Red");
    r = p_red->valueint;
    cJSON* p_green = cJSON_GetObjectItem(p_color, "Green");
    g = p_green->valueint;
    cJSON* p_blue = cJSON_GetObjectItem(p_color, "Blue");
    b = p_blue->valueint;

    return RGBled_GetRgb888(r,g,b);
}
