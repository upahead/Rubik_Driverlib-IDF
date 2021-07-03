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

SemaphoreHandle_t BinarySemaphore_1;

void app_main(void)
{
    printf("FreeRTOS semaphore binary demo.\n");

    printf("create binary semaphore.\n");
    BinarySemaphore_1 = xSemaphoreCreateBinary();

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

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("Task 1 Give binary semaphore.\n");
    xSemaphoreGive(BinarySemaphore_1);
    printf("Task 1 delete.\n");
    vTaskDelete(NULL);
}

void DemoTask_2(void* pvParameter)
{
    printf("Task 2 created to core 0 \n");

    printf("Task 2 wait binary semaphore...\n");
    xSemaphoreTake(BinarySemaphore_1, portMAX_DELAY);
    printf("Task 2 take binary semaphore.\n");
    printf("Task 2 delete.\n");
    vTaskDelete(NULL);
}
