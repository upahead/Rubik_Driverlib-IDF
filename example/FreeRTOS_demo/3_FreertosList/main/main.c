#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/list.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void DemoTask_1(void* pvParameter);

List_t DemoList;
ListItem_t DemoItem_1;
ListItem_t DemoItem_2;
ListItem_t DemoItem_3;

void app_main(void)
{
    printf("FreeRTOS list demo.\n");
    xTaskCreate(DemoTask_1, "DemoTask_1", 1024*2, NULL, 1, NULL);

    while(1){        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void DemoTask_1(void* pvParameter)
{
    printf("Task 1 created.\n");

    printf("\nInit list adn list item.\n");
    vListInitialise(&DemoList);
    vListInitialiseItem(&DemoItem_1);
    DemoItem_1.xItemValue = 10;
    vListInitialiseItem(&DemoItem_2);
    DemoItem_2.xItemValue = 20;
    vListInitialiseItem(&DemoItem_3);
    DemoItem_3.xItemValue = 30;

    printf("\nInsert list item to list.\n");
    vListInsert(&DemoList, &DemoItem_1);
    vListInsert(&DemoList, &DemoItem_2);
    vListInsert(&DemoList, &DemoItem_3);

    printf("\nprint list item addr and item value.\n");
    printf("List item 1 addr = %x val = %d \n", (int)&DemoItem_1, DemoItem_1.xItemValue);
    printf("List item 1 prev = %x next = %x \n", (int)(DemoItem_1.pxPrevious), (int)(DemoItem_1.pxNext));
    printf("List item 2 addr = %x val = %d \n", (int)&DemoItem_2, DemoItem_2.xItemValue);
    printf("List item 2 prev = %x next = %x \n", (int)(DemoItem_2.pxPrevious), (int)(DemoItem_2.pxNext));
    printf("List item 3 addr = %x val = %d \n", (int)&DemoItem_3, DemoItem_3.xItemValue);
    printf("List item 3 prev = %x next = %x \n", (int)(DemoItem_3.pxPrevious), (int)(DemoItem_3.pxNext));

    printf("\nprint list info.\n");
    printf("List addr = %x pxIndex = %x ListEnd = %x \n", (int)&DemoList, (int)(&DemoList.pxIndex), (int)(&DemoList.xListEnd));

    printf("\nremove list item 2.\n");
    uxListRemove(&DemoItem_2);

    printf("\nprint list item addr and item value.\n");
    printf("List item 1 addr = %x val = %d \n", (int)&DemoItem_1, DemoItem_1.xItemValue);
    printf("List item 1 prev = %x next = %x \n", (int)(DemoItem_1.pxPrevious), (int)(DemoItem_1.pxNext));
    printf("List item 2 addr = %x val = %d \n", (int)&DemoItem_2, DemoItem_2.xItemValue);
    printf("List item 2 prev = %x next = %x \n", (int)(DemoItem_2.pxPrevious), (int)(DemoItem_2.pxNext));
    printf("List item 3 addr = %x val = %d \n", (int)&DemoItem_3, DemoItem_3.xItemValue);
    printf("List item 3 prev = %x next = %x \n", (int)(DemoItem_3.pxPrevious), (int)(DemoItem_3.pxNext));

    printf("\nAdd item 2 to the end of the list.\n");
    vListInsertEnd(&DemoList, &DemoItem_2);

    printf("\nprint list item addr and item value.\n");
    printf("List item 1 addr = %x val = %d \n", (int)&DemoItem_1, DemoItem_1.xItemValue);
    printf("List item 1 prev = %x next = %x \n", (int)(DemoItem_1.pxPrevious), (int)(DemoItem_1.pxNext));
    printf("List item 3 addr = %x val = %d \n", (int)&DemoItem_3, DemoItem_3.xItemValue);
    printf("List item 3 prev = %x next = %x \n", (int)(DemoItem_3.pxPrevious), (int)(DemoItem_3.pxNext));
    printf("List item 2 addr = %x val = %d \n", (int)&DemoItem_2, DemoItem_2.xItemValue);
    printf("List item 2 prev = %x next = %x \n", (int)(DemoItem_2.pxPrevious), (int)(DemoItem_2.pxNext));

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("Task 1 delete.\n");
    vTaskDelete(NULL);
}
