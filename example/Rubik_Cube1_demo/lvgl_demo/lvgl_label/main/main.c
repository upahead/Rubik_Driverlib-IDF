#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include <driver/adc.h>

/* Littlevgl specific */
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "LVGL_demo"
#define LV_TICK_PERIOD_MS 1

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);

void Gui_Main_Create(void);

/**********************
 *   APPLICATION MAIN
 **********************/
void app_main() 
{   
    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    xTaskCreatePinnedToCore(guiTask, "gui", 4096*2, NULL, 0, NULL, 1);
}

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;

static void guiTask(void *pvParameter) {

    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    lv_color_t* buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);
    lv_color_t* buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* Create main gui interface */
    Gui_Main_Create();

    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
       }
    }

    /* A task should NEVER return */
    free(buf1);
    free(buf2);
    vTaskDelete(NULL);
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}


void Gui_Main_Create(void)
{
    // Get screen object.
    lv_obj_t* screen = lv_scr_act(); 

    // set label Automatic label expansion.
    lv_obj_t* label_1 = lv_label_create(screen, NULL);
    lv_obj_set_pos(label_1, 10, 10);
    lv_label_set_text(label_1, "Automatic label expansion.");
    lv_label_set_long_mode(label_1, LV_LABEL_LONG_EXPAND);

    // set label Label limited width wrap.
    lv_obj_t* label_2 = lv_label_create(screen, NULL);
    lv_obj_align(label_2, label_1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_label_set_text(label_2, "Label limited width wrap.");
    lv_label_set_long_mode(label_2, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label_2, 100);

    // set label Use ... to replace long text.
    lv_obj_t* label_3 = lv_label_create(screen, NULL);
    lv_obj_align(label_3, label_2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_label_set_text(label_3, "Use ... to replace long text.");
    lv_label_set_long_mode(label_3, LV_LABEL_LONG_DOT);
    lv_obj_set_width(label_3, 100);

    // set label Text scrolling display.
    lv_obj_t* label_4 = lv_label_create(screen, NULL);
    lv_obj_align(label_4, label_3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_label_set_text(label_4, "Text scrolling display.");
    lv_label_set_long_mode(label_4, LV_LABEL_LONG_SROLL);
    lv_label_set_anim_speed(label_4, 80);
    lv_obj_set_width(label_4, 100);

    // set label recolor.
    lv_obj_t* label_5 = lv_label_create(screen, NULL);
    lv_obj_align(label_5, label_4, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_label_set_recolor(label_5, true);  
    lv_label_set_text(label_5, "#ff0000 RED ##00ff00 BLUE ##0000ff GREEN#");

    // label insert and cut.
    lv_obj_t* label_6 = lv_label_create(screen, NULL);
    lv_obj_align(label_6, label_5, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10); 
    lv_label_set_text(label_6, "This is color text.");
    lv_label_ins_text(label_6, LV_LABEL_POS_LAST, " ----- ");
    lv_label_cut_text(label_6, 0, 4);
}