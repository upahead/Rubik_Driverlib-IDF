#include "7_test_page.h"

static lv_style_t page_style1;
static lv_style_t btn_style1;

lv_obj_t* page1;

lv_obj_t* label1;
lv_obj_t* label2;
lv_obj_t* label3;

lv_obj_t* btn1;
lv_obj_t* btn2;
lv_obj_t* btn3;
lv_obj_t* btn4;
lv_obj_t* btn5;
lv_obj_t* btn6;

void event_handle(lv_obj_t* obj, lv_event_t event)
{
    if(obj == btn1){
        if(event == LV_EVENT_PRESSED){
            lv_page_focus(page1, btn6, true);               // Focus on button 6, start the animation.
        }
    }
}

void lv_test_page(void)
{
    // Get screen object.
    lv_obj_t* screen = lv_scr_act(); 

    //创建page1对象
    page1 = lv_page_create(screen, NULL);
    lv_obj_set_size(page1, 320, 200);
    lv_obj_align(page1, screen, LV_ALIGN_CENTER, 0, 0);
 
    lv_page_set_scrl_layout(page1, LV_LAYOUT_PRETTY_MID);     // Set the layout of the scrollable part of the page.
    lv_page_set_scrollbar_mode(page1, LV_SCROLLBAR_MODE_OFF );// Set hidden scroll bar.
    lv_page_set_edge_flash(page1, true);                      // Turn on the semi-circular edge flash effect.
    lv_page_set_anim_time(page1,100);                         // Set the animation time ms.

    // Use label to demonstrate page basic scrolling properties.
    /*
    label1 = lv_label_create(page1, NULL);
    lv_obj_set_width(label1, lv_page_get_width_fit(page1));
    lv_label_set_text(label1, "hello world ! hello world ! hello world ! hello world ! hello world ! hello world ! hello world ! \nhello wor\nl\nd ! h\nel\nlo\n wo\nrld\n ! \nhel\nlo \nwo\nrld\n !");
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);              // Set the long text mode BREAK, it will wrap automatically after limiting the width.
    lv_obj_set_width(label1, 320);                                     // Set the width, then set.
    */


    btn1 = lv_btn_create(page1, btn1);
    lv_obj_set_size(btn1, 80, 80);
    lv_style_init(&btn_style1);
    lv_style_set_radius(&btn_style1, LV_STATE_DEFAULT, 20);                 // Set the corner radian.
    lv_obj_add_style(btn1, LV_OBJ_PART_MAIN, &btn_style1);
    lv_obj_set_event_cb(btn1, event_handle);                                // Set button one callback.

    label2 = lv_label_create(btn1, NULL);
    lv_label_set_text(label2, "BTN1");

    btn2 = lv_btn_create(page1, btn1);
    btn3 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);
    btn5 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);
    btn6 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);
    btn4 = lv_btn_create(page1, btn1);

    label3 = lv_label_create(btn6, NULL);
    lv_label_set_text(label3, "BTN6");

}
