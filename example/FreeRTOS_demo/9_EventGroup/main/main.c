#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void DemoTask_1(void* pvParameter);

void DemoTask_2(void* pvParameter);
TaskHandle_t TaskHandle_2;

void DemoTask_3(void* pvParameter);
TaskHandle_t TaskHandle_3;

EventGroupHandle_t EventGroupDemo_1;
#define EVENT_BIT_0	       (0x01<<0)
#define EVENT_BIT_1	       (0x01<<1)

void app_main(void)
{
    printf("FreeRTOS event group demo.\n");
    xTaskCreate(DemoTask_1, "DemoTask_1", 1024*2, NULL, 1, NULL);
    while(1){        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void DemoTask_1(void* pvParameter)
{
    printf("Task 1 created.\n");

    printf("Create event group demo 1.\n");
    EventGroupDemo_1 = xEventGroupCreate();
    
    printf("Now, create Task 2 to core 0, create Task 3 to core 1.\n");
    xTaskCreatePinnedToCore(DemoTask_2, "DemoTask_2", 1024*2, NULL, 2, &TaskHandle_2, 0);
    xTaskCreatePinnedToCore(DemoTask_3, "DemoTask_3", 1024*2, NULL, 2, &TaskHandle_3, 1);

    printf("Task 1 delete.\n");
    vTaskDelete(NULL);
}

void DemoTask_2(void* pvParameter)
{
    printf("Task 2 created to core 0 \n");
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("Task 2 set event group bit-0.\n");
    xEventGroupSetBits(EventGroupDemo_1, EVENT_BIT_0);
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("Task 2 set event group bit-1.\n");
    xEventGroupSetBits(EventGroupDemo_1, EVENT_BIT_1);

    printf("Task 2 delete.\n");
    vTaskDelete(NULL);
}

void DemoTask_3(void* pvParameter)
{   
    EventBits_t event_val;
    printf("Task 3 create to core 1 \n");
    
    printf("Task 3 event group wait bit-0 and bit-1.\n");
    event_val = xEventGroupWaitBits(EventGroupDemo_1, EVENT_BIT_0|EVENT_BIT_1, pdTRUE, pdTRUE, portMAX_DELAY);
    printf("waited event gruop all bit, event val = %d \n", event_val);

    printf("Task 3 delete.\n");
    vTaskDelete(NULL);
}