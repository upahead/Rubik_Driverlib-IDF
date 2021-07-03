#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void DemoTask_1(void* pvParameter);

TimerHandle_t  ReloadTimer;
void ReloadTimer_cb(TimerHandle_t xTimer);

TimerHandle_t  OnceTimer;
void OnceTimer_cb(TimerHandle_t xTimer);

void app_main(void)
{
    printf("FreeRTOS timer demo.\n");
    xTaskCreate(DemoTask_1, "DemoTask_1", 1024*2, NULL, 1, NULL);
    while(1){        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void DemoTask_1(void* pvParameter)
{
    printf("Task 1 created.\n");
    
    printf("create reload timer and once timer!\n");
    ReloadTimer = xTimerCreate("ReloadTimer", 500 / portTICK_PERIOD_MS, pdTRUE, 1, ReloadTimer_cb);
    OnceTimer = xTimerCreate("OnceTimer", 1000 / portTICK_PERIOD_MS, pdFALSE, 2, OnceTimer_cb);
    xTimerStart(ReloadTimer, 0);
    xTimerStart(OnceTimer, 0);

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    printf("Reload timer stop.\n");
    xTimerStop(ReloadTimer, 0);

    printf("Task 1 delete.\n");
    vTaskDelete(NULL);
}

void ReloadTimer_cb(TimerHandle_t xTimer)
{
    printf("Reload timer over!\n");
}

void OnceTimer_cb(TimerHandle_t xTimer)
{
    printf("Once timer over!\n");
}