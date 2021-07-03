#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void DemoTask_1(void* pvParameter);

void DemoTask_2(void* pvParameter);
TaskHandle_t TaskHandle_2;

void DemoTask_3(void* pvParameter);
TaskHandle_t TaskHandle_3;

QueueHandle_t MessageQueue_1;

void app_main(void)
{
    printf("FreeRTOS queue demo.\n");
    xTaskCreate(DemoTask_1, "DemoTask_1", 1024*2, NULL, 1, NULL);
    while(1){        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void DemoTask_1(void* pvParameter)
{
    uint8_t msg_buf;
    printf("Task 1 created.\n");

    printf("create message queue.\n");
    MessageQueue_1 = xQueueCreate(4, sizeof(uint8_t));
    
    printf("Now, create Task 2 to core 0, create Task 3 to core 1.\n");
    xTaskCreatePinnedToCore(DemoTask_2, "DemoTask_2", 1024*2, NULL, 2, &TaskHandle_2, 0);
    xTaskCreatePinnedToCore(DemoTask_3, "DemoTask_3", 1024*2, NULL, 2, &TaskHandle_3, 1);

    while(1){
        // It blocks until it reads the message, reads the data and deletes it.
        xQueueReceive(MessageQueue_1, &msg_buf, portMAX_DELAY);
        printf("Receive msg! value = %d.\n", msg_buf);
    }
}

void DemoTask_2(void* pvParameter)
{
    uint8_t msg = 1;

    printf("Task 2 created to core 0 \n");
    while(1){
        msg += 2;
        printf("send val to message queue.\n");
        xQueueSend(MessageQueue_1, &msg, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void DemoTask_3(void* pvParameter)
{   
    uint8_t msg = 2;
    printf("Task 3 create to core 1 \n");
    while(1){
        msg += 2;
        printf("send val to message queue.\n");
        xQueueSend(MessageQueue_1, &msg, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}