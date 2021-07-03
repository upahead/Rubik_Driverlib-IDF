#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void DemoTask_1(void* pvParameter);
void DemoTask_2(void* pvParameter);

SemaphoreHandle_t MutexSemaphore_1;

void app_main(void)
{
    printf("FreeRTOS semaphore mutex demo.\n");

    printf("create mutex semaphore.\n");
    MutexSemaphore_1 = xSemaphoreCreateMutex();

    xTaskCreate(DemoTask_1, "DemoTask_1", 1024*2, NULL, 1, NULL);
    xTaskCreate(DemoTask_2, "DemoTask_2", 1024*2, NULL, 1, NULL);
    while(1){        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void DemoTask_1(void* pvParameter)
{
    printf("Task 1 created.\n");
    
    while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        xSemaphoreTake(MutexSemaphore_1, portMAX_DELAY);
        printf("Task 1 Take mutex semaphore.\n");
        printf("Task 1 ......\n");
        xSemaphoreGive(MutexSemaphore_1);
        printf("Task 1 Give mutex semaphore.\n");
    }
}

void DemoTask_2(void* pvParameter)
{
    printf("Task 2 created.\n");

    while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        xSemaphoreTake(MutexSemaphore_1, portMAX_DELAY);
        printf("Task 2 Take mutex semaphore.\n");
        printf("Task 2 ......\n");
        xSemaphoreGive(MutexSemaphore_1);
        printf("Task 2 Give mutex semaphore.\n");
    }
}
