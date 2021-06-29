#include "6_test_btn.h"
#include <stdio.h>

lv_obj_t* btn1;
lv_obj_t* btn2;
lv_obj_t* label1;

// Define custom event types.
// The system's own LV_EVENT_PRESSED and other event types occupy 0-19, and the user-defined range is 20-255
#define LV_EVENT_CUSTOM1    30

//LV_INDEV_LONG_PRESS_TIME Macro to set the length of the press.

void lv_btn_cb(lv_obj_t* obj, lv_event_cb_t event)
{
    if(obj == btn1){
        if(event == LV_EVENT_PRESSED){                  // Press.
            lv_label_set_text(label1, "PRESSED");
        }else if(event == LV_EVENT_RELEASED ){          // Release, conflict with LV_EVENT_PRESS_LOST.
            lv_label_set_text(label1, "RELEASED");
        }else if(event == LV_EVENT_PRESS_LOST ){        // Draw out the visible range of the object.
            lv_label_set_text(label1, "PRESS_LOST");
        }else if(event == LV_EVENT_SHORT_CLICKED ){     // Short press (there is a problem).
            lv_label_set_text(label1, "SHORT_CLICKED");
        }else if(event == LV_EVENT_LONG_PRESSED  ){     // Long press.
            lv_label_set_text(label1, "LONG_PRESSED");
        }else if(event == LV_EVENT_LONG_PRESSED_REPEAT  ){  // Long press to trigger repeatedly.
            lv_label_set_text(label1, "LONG_PRESSED_REPEAT");
        }else if(event == LV_EVENT_CLICKED  ){              // Click, release without dragging conflicts with LV_EVENT_PRESS_LOST.
            lv_label_set_text(label1, "CLICKED");
        }else if(event == LV_EVENT_DRAG_BEGIN ){            // Dragging starts, provided that dragging is turned on and LV_EVENT_PRESS_LOST conflicts.
            lv_label_set_text(label1, "DRAG_BEGIN");
        }else if(event == LV_EVENT_DRAG_END ){              // Dragging ends, provided that dragging is enabled and LV_EVENT_PRESS_LOST conflicts.
            lv_label_set_text(label1, "DRAG_END");
        }else if(event == LV_EVENT_DRAG_THROW_BEGIN ){      // Drag drift starts, the premise is that drag is turned on and LV_EVENT_PRESS_LOST conflicts.
            lv_label_set_text(label1, "DRAG_THROW_BEGIN");
        }else if(event == LV_EVENT_CUSTOM1 ){               // Trigger custom event type 1.
            //lv_label_set_text(label1, "CUSTOM1");
            int data = (int)lv_event_get_data();            // Obtain user data carried by triggering the current event.
            char src[20];
            sprintf(src, "CUSTOM1 %d", data);
            lv_label_set_text(label1, src);                 // print it out.
        }
    }
    if(obj == btn2){
        if(event == LV_EVENT_PRESSED){
            lv_event_send(btn1, LV_EVENT_PRESSED, (void*)123);    // Press event forwarded to button 1.
        }else if(event == LV_EVENT_RELEASED ){
            lv_event_send(btn1, LV_EVENT_CUSTOM1, (void*)123);    // Send custom data type to button 1.
            //lv_event_send_func(lv_btn_cb, btn1, LV_EVENT_CUSTOM1, NULL);    // Specify callback function and object sending.
        }
    }
}

void lv_test_btn(void)
{
    // Get screen object.
    lv_obj_t* screen = lv_scr_act(); 

    btn1 = lv_btn_create(screen, NULL);
    lv_obj_align(btn1, screen, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_drag(btn1, true);                        // Open drag.
    lv_obj_set_drag_throw(btn1, true);                  // Set object 3 to turn on inertial sliding.

    label1 = lv_label_create(btn1, NULL);
    lv_label_set_text(label1, "BUTTON");

    // Set key event callback.
    lv_obj_set_event_cb(btn1, lv_btn_cb);

    // Create button 2.
    btn2 = lv_btn_create(screen, NULL);
    lv_obj_set_event_cb(btn2, lv_btn_cb);       // Set to key callback.
}