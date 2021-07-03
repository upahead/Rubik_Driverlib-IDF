#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void DemoTask_1(void* pvParameter);

void DemoTask_2(void* pvParameter);
TaskHandle_t TaskHandle_2;

SemaphoreHandle_t ConutSemaphore_1;

void app_main(void)
{
    printf("FreeRTOS semaphore connting demo.\n");

    printf("create connting semaphore.\n");
    ConutSemaphore_1 = xSemaphoreCreateCounting(255, 0);

    xTaskCreate(DemoTask_1, "DemoTask_1", 1024*2, NULL, 1, NULL);
    while(1){        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void DemoTask_1(void* pvParameter)
{
    printf("Task 1 created.\n");
    
    printf("Now, create Task 2 to core 0.\n");
    xTaskCreatePinnedToCore(DemoTask_2, "DemoTask_2", 1024*2, NULL, 1, &TaskHandle_2, 0);

    for(uint32_t i=0; i<3; i++){
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        printf("Task 1 Give connting semaphore once.\n");
        xSemaphoreGive(ConutSemaphore_1);
    }   
    printf("Task 1 delete.\n");
    vTaskDelete(NULL);
}

void DemoTask_2(void* pvParameter)
{
    printf("Task 2 created to core 0 \n");

    printf("Task 2 wait connting semaphore thrice...\n");
    xSemaphoreTake(ConutSemaphore_1, portMAX_DELAY);
    printf("Task 2 take connting semaphore.\n");
    xSemaphoreTake(ConutSemaphore_1, portMAX_DELAY);
    printf("Task 2 take connting semaphore.\n");
    xSemaphoreTake(ConutSemaphore_1, portMAX_DELAY);
    printf("Task 2 take connting semaphore.\n");

    printf("Task 2 delete.\n");
    vTaskDelete(NULL);
}
