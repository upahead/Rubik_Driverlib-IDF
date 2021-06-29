#include "3_test_style.h"

lv_style_t style1;
lv_obj_t* btn1 = NULL;

void btn1_event_cb_handle(lv_obj_t* obj, lv_event_t* event)
{
    if(obj == btn1){
        if(event == LV_EVENT_PRESSED){
            lv_style_set_border_width(&style1, LV_STATE_PRESSED, 10);
            lv_obj_refresh_style(btn1,LV_OBJ_PART_ALL,LV_STYLE_PROP_ALL);         // Notification button refresh style.
            //lv_obj_report_style_mod(&style1);                                   // Notification style refresh style.
        }
    }
}

void lv_test_style(void)
{
    // Get screen object.
    lv_obj_t* screen = lv_scr_act(); 

    // Create a new color.
    lv_color16_t color1 = LV_COLOR_MAKE(29,189,21);

    // Create a new style.
    lv_style_init(&style1);

    // Background color property style setting.
    lv_style_set_radius(&style1, LV_STATE_DEFAULT, 20);                 // Set the corner radian.
    lv_style_set_bg_color(&style1, LV_STATE_DEFAULT, LV_COLOR_RED);     // Set background color.
    lv_style_set_bg_color(&style1, LV_STATE_PRESSED, LV_COLOR_BLUE);

    lv_style_set_bg_opa(&style1, LV_STATE_DEFAULT, LV_OPA_50);          // Specify the opacity of the background.
    lv_style_set_bg_color(&style1, LV_STATE_DEFAULT, color1);           // Set the background color when pressed.
    lv_style_set_bg_grad_color(&style1, LV_STATE_DEFAULT, LV_COLOR_BLUE);// Specify the color of the background gradient.
    lv_style_set_bg_grad_dir(&style1, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);// Specify the direction of the gradient.
    // Gradient position 0: the leftmost/top position, 255: the rightmost/bottom position. Default value: 0.
    //lv_style_set_bg_main_stop(&style1, LV_STATE_DEFAULT, 128);          // Specify where the gradient should start.
    //lv_style_set_bg_grad_stop(&style1, LV_STATE_DEFAULT, 192);          // Specify where the gradient should stop.

    // Border attribute style setting (the border is inside the object).
    lv_style_set_border_color(&style1, LV_STATE_DEFAULT, LV_COLOR_CYAN);// Specify the color of the border.
    lv_style_set_border_opa(&style1, LV_STATE_DEFAULT, LV_OPA_50);      // Specify the opacity of the border.
    lv_style_set_border_width(&style1, LV_STATE_DEFAULT, 5);            // Set the width of the border.
    lv_style_set_border_side(&style1, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT);// Specify which side of the border to draw.

    // Outline attribute style setting (the outline is outside the object) The attributes that can be set are the same as the border.
    lv_style_set_outline_color(&style1, LV_STATE_PRESSED, LV_COLOR_BLACK);// Specify the color of the outline.
    lv_style_set_outline_opa(&style1, LV_STATE_PRESSED, LV_OPA_COVER);    // Specify the opacity of the outline.
    lv_style_set_outline_width(&style1, LV_STATE_PRESSED, 5);             // Set the width of the outline.
    lv_style_set_outline_pad(&style1, LV_STATE_PRESSED, 5);               // Set the space between the object and the outline.

    /* For the rest of the style settings, see the official document style chapter.
    Also includes: shadow attributes, pattern attributes, text attributes, text attributes, 
    line attributes, picture attributes, transition attributes, and scale attributes. */

    // Create object 1 demo.
    btn1 = lv_btn_create(screen, NULL);
    lv_obj_set_size(btn1, 100, 80);
    lv_obj_set_pos(btn1, 110, 80);
    lv_obj_add_style(btn1, LV_STATE_DEFAULT, &style1);

    // Create button callback event.
    lv_obj_set_event_cb(btn1, btn1_event_cb_handle);
}