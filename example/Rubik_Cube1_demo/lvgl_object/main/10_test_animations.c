#include "10_test_animations.h"
#include <stdio.h>


lv_obj_t* btn1;
lv_obj_t* btn2;
lv_obj_t* image1;

lv_style_t btn1_style;
lv_style_t btn2_style;
lv_style_t image1_style;


LV_IMG_DECLARE(setting_icon);

static void anim_set_size_cb(lv_anim_t * a, lv_anim_value_t v);
static void anim_change_color_cb(lv_anim_t * a, lv_anim_value_t v);

//********************************************************************************************

static void lv_btn1_click_anim(lv_obj_t * obj, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);                                       // Initialize animation variables.
    lv_anim_set_var(&a, obj);                               // Set the object to be processed by the animation variable.
    lv_anim_set_time(&a, 1000);                             // Set animation duration.
    lv_anim_set_delay(&a, delay);                           // Set the delay before the animation starts.

#if 0
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_height);  // Set the function of animation, the function that can be executed during the animation The built-in function of LittelvGL can be used and can be customized.
    lv_anim_set_values(&a, 100, 200);                          // Set the start and end values of the animation.
    // lv_anim_set_start_cb(&a, lv_obj_del_anim_ready_cb);     // The action performed before the animation is executed, here is to delete the object, which can be defined as a function created by yourself.
    // lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);     // The action executed after the animation is executed, here is to delete the object, which can be defined as a function created by yourself.
#endif

#if  1
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_set_size_cb);
    lv_anim_set_values(&a, 20, 300);
#endif

    lv_anim_path_t path;
    lv_anim_path_init(&path);                               // Initialize an animation path.
    lv_anim_path_set_cb(&path, lv_anim_path_bounce);        // Set the effect of the path, here is set to bounce a little from the final value.
    // lv_anim_path_set_user_data(&path, &foo);             /*Optional for custom functions*/
    lv_anim_set_path(&a, &path);                            // Set path for animation

    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);                       // Set the number of animation repeats.

    lv_anim_t b;
    lv_anim_init(&b);                                       
    lv_anim_set_var(&b, obj);                               
    lv_anim_set_time(&b, 1000);                             
    lv_anim_set_delay(&b, delay);

    lv_anim_set_exec_cb(&b, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&b, 10, 120);     

    lv_anim_set_repeat_count(&b, LV_ANIM_REPEAT_INFINITE);

    lv_anim_t c;
    lv_anim_init(&c);                                       
    lv_anim_set_var(&c, obj);                               
    lv_anim_set_time(&c, 800);                             
    lv_anim_set_delay(&c, delay+200);

    lv_anim_set_exec_cb(&c, (lv_anim_exec_xcb_t)anim_change_color_cb);
    lv_anim_set_values(&c, 0, 255);         

    lv_anim_set_repeat_count(&c, LV_ANIM_REPEAT_INFINITE);         

    lv_anim_start(&a);                                      // Start animation.
    lv_anim_start(&b);   
    lv_anim_start(&c);                                    
}

//********************************************************************************************

static void lv_btn2_click_anim(lv_obj_t * obj, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a); 
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 500);
    lv_anim_set_delay(&a, delay);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, 10, 170);

    lv_anim_t b;
    lv_anim_init(&b); 
    lv_anim_set_var(&b, obj);
    lv_anim_set_time(&b, 500);
    lv_anim_set_delay(&b, delay);

    lv_anim_set_exec_cb(&b, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&b, (320/2)-15, 10);

    lv_anim_t c;
    lv_anim_init(&c); 
    lv_anim_set_var(&c, obj);
    lv_anim_set_time(&c, 500);
    lv_anim_set_delay(&c, delay);

    lv_anim_set_exec_cb(&c, (lv_anim_exec_xcb_t)lv_obj_set_width);
    lv_anim_set_values(&c, 30, 300);

    lv_anim_start(&a); 
    lv_anim_start(&b); 
    lv_anim_start(&c);
}

//********************************************************************************************
static void Img1_Change_Piovt(lv_anim_t * a)
{
    // lv_img_set_pivot(image1, lv_obj_get_x(image1)+80, lv_obj_get_y(image1)+80);
}

static void lv_img1_click_anim(lv_obj_t * obj, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a); 
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 500);
    lv_anim_set_delay(&a, delay);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&a, 230, 120);

    lv_anim_t b;
    lv_anim_init(&b); 
    lv_anim_set_var(&b, obj);
    lv_anim_set_time(&b, 500);
    lv_anim_set_delay(&b, delay);

    lv_anim_set_exec_cb(&b, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&b, 10, 100);

    lv_anim_t c;
    lv_anim_init(&c); 
    lv_anim_set_var(&c, obj);
    lv_anim_set_time(&c, 500);
    lv_anim_set_delay(&c, delay);

    lv_anim_set_exec_cb(&c, (lv_anim_exec_xcb_t)lv_img_set_zoom);
    lv_anim_set_values(&c, 128, 512);

    lv_anim_t d;
    lv_anim_init(&d); 
    lv_anim_set_var(&d, obj);
    lv_anim_set_time(&d, 500);
    lv_anim_set_delay(&d, delay+500);           // Perform the first three before starting to execute this animation.
    lv_anim_set_start_cb(&d, Img1_Change_Piovt);    // Set the center of rotation before execution.
    lv_anim_set_repeat_count(&d, LV_ANIM_REPEAT_INFINITE);   // Set the number of animation repeats.

    lv_anim_set_exec_cb(&d, (lv_anim_exec_xcb_t)lv_img_set_angle);
    lv_anim_set_values(&d, 0, 3600);

    lv_anim_start(&a);
    lv_anim_start(&b); 
    lv_anim_start(&c); 
    lv_anim_start(&d); 
}

//********************************************************************************************


static void Btn_Event_Handle(lv_obj_t* obj, lv_event_t event);

void lv_test_animations(void)
{
    lv_obj_t* screen = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(screen, 320, 240);

    // Create button 1.
    btn1 = lv_btn_create(screen, NULL);
    lv_obj_set_size(btn1, 100, 100);
    lv_obj_align(btn1, screen, LV_ALIGN_IN_TOP_LEFT, 10, 10);

    lv_style_init(&btn1_style);
    lv_style_set_bg_color(&btn1_style, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_style_set_bg_color(&btn1_style, LV_STATE_PRESSED, LV_COLOR_RED);
    lv_style_set_border_width(&btn1_style, LV_STATE_DEFAULT, 0);
    lv_style_set_outline_width(&btn1_style, LV_STATE_DEFAULT, 0);

    lv_obj_add_style(btn1, LV_OBJ_PART_MAIN, &btn1_style);
    lv_obj_set_event_cb(btn1, Btn_Event_Handle);

    lv_obj_fade_in(btn1, 200, 200);  

    // Create button 2.
    btn2 = lv_btn_create(screen, NULL);
    lv_obj_set_size(btn2, 30, 60);
    lv_obj_align(btn2, screen, LV_ALIGN_IN_TOP_MID, 0, 10);

    lv_style_init(&btn2_style);
    lv_style_set_bg_color(&btn2_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_style_set_border_color(&btn2_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_style_set_border_width(&btn2_style, LV_STATE_DEFAULT, 0);
    lv_style_set_outline_width(&btn2_style, LV_STATE_DEFAULT, 0);

    lv_obj_add_style(btn2, LV_OBJ_PART_MAIN, &btn2_style);
    lv_obj_set_event_cb(btn2, Btn_Event_Handle);

    lv_obj_fade_in(btn2, 200, 200);

    // Create picture.
    image1 = lv_img_create(screen, NULL);
    lv_style_init(&image1_style);
    lv_style_set_clip_corner(&image1_style, LV_STATE_DEFAULT, true);
    lv_style_set_radius(&image1_style, LV_STATE_DEFAULT, 20);
    lv_obj_add_style(image1, LV_OBJ_PART_MAIN, &image1_style);
    lv_img_set_src(image1, &setting_icon);
    lv_obj_align(image1, screen, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
    lv_obj_set_event_cb(image1, Btn_Event_Handle);
    // lv_obj_set_size(image1, 120, 120);
    // lv_img_set_auto_size(image1, true);
    // lv_img_set_zoom(image1, 512);

    lv_obj_fade_in(image1, 500, 200);
}

static void Btn_Event_Handle(lv_obj_t* obj, lv_event_t event)
{
    if(obj == btn1){
        if(event == LV_EVENT_SHORT_CLICKED){
            lv_btn1_click_anim(obj, 0);lv_btn2_click_anim(btn2, 0);
        }
    }else if(obj == btn2){
        if(event == LV_EVENT_SHORT_CLICKED){
            // lv_btn2_click_anim(obj, 0);
            lv_img1_click_anim(image1, 0);
        }
    }else if(obj == image1){
        if(event == LV_EVENT_SHORT_CLICKED){
            // lv_img1_click_anim(obj, 0);
        }
    }
}

//********************************************************************************************

// Custom animation function, change obj size.
static void anim_set_size_cb(lv_anim_t * a, lv_anim_value_t v)
{
    lv_obj_set_size(btn1, v, v);
}

// Custom animation function, from red to blue animation.
static void anim_change_color_cb(lv_anim_t * a, lv_anim_value_t v)
{
    uint8_t red = 255-v;
    uint8_t blue = 0+v;
    lv_color_t new_color = lv_color_hex((red<<16) + (blue));
    lv_style_set_bg_color(&btn1_style, LV_STATE_DEFAULT, new_color);
}

//********************************************************************************************