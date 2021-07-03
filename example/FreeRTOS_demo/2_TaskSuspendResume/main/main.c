#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void DemoTask_1(void* pvParameter);

void DemoTask_2(void* pvParameter);
TaskHandle_t TaskHandle_2;

void DemoTask_3(void* pvParameter);
TaskHandle_t TaskHandle_3;

void app_main(void)
{
    printf("FreeRTOS task suspend and resume demo.\n");
    xTaskCreate(DemoTask_1, "DemoTask_1", 1024*2, NULL, 1, NULL);

    while(1){        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void DemoTask_1(void* pvParameter)
{
    printf("Task 1 created.\n");
    
    printf("Now, create Task 2 to core 0, create Task 3 to core 1.\n");
    xTaskCreatePinnedToCore(DemoTask_2, "DemoTask_2", 1024*2, NULL, 2, &TaskHandle_2, 0);
    xTaskCreatePinnedToCore(DemoTask_3, "DemoTask_3", 1024*2, NULL, 2, &TaskHandle_3, 1);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("Suspend Task 2.\n");
    vTaskSuspend(TaskHandle_2);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("Resume Task 2.\n");
    vTaskResume(TaskHandle_2);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("Task 1 delete.\n");
    vTaskDelete(NULL);
}

void DemoTask_2(void* pvParameter)
{
    printf("Task 2 created to core 0 \n");
    while(1){
        printf("Task 2 running... \n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void DemoTask_3(void* pvParameter)
{   
    printf("Task 3 create to core 1 \n");
    while(1){
        printf("Task 3 running... \n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}