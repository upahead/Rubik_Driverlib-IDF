#include "5_test_cont.h"

static lv_style_t cont_style1;
static lv_style_t btn_style1;

lv_obj_t* btn1;
lv_obj_t* btn2;
lv_obj_t* btn3;
lv_obj_t* btn4;

void lv_test_cont(void)
{
    // Get screen object.
    lv_obj_t* screen = lv_scr_act(); 

    // Create a container object.
    lv_obj_t* cont1 = lv_cont_create(screen, NULL);
    lv_obj_set_pos(cont1, 0, 40);
    // lv_obj_align(cont1, screen, LV_ALIGN_CENTER, 0, 0);  // Set the alignment of the container in the parent object.
    //lv_cont_set_fit(cont1, LV_FIT_TIGHT);                   // Set container size adaptive.
    //lv_cont_set_layout(cont1, LV_LAYOUT_CENTER);            // Set the layout of sub-objects in the container to center alignment.
    lv_obj_set_size(cont1, 320, 200);                         // Set container size.


    // Create a cont style.
    lv_style_init(&cont_style1);
    lv_cont_set_layout(cont1, LV_LAYOUT_GRID);
    lv_style_set_pad_inner(&cont_style1, LV_STATE_DEFAULT, 20);   // Set the margin padding of the child object.
    // lv_style_set_pad_left(&cont_style1, LV_STATE_DEFAULT, 25);
    // lv_style_set_pad_right(&cont_style1, LV_STATE_DEFAULT, 10);
    lv_obj_add_style(cont1,LV_OBJ_PART_MAIN, &cont_style1);

    // Create a btn style.
    lv_style_init(&btn_style1);

    // Add four buttons to the container.
#if 1
    lv_cont_set_layout(cont1, LV_LAYOUT_PRETTY_MID);          // Set the layout of sub-objects in the container.
    btn1 = lv_btn_create(cont1, NULL);
    lv_obj_set_size(btn1, 300, 80);
    lv_style_set_radius(&btn_style1, LV_STATE_DEFAULT, 20);
    lv_obj_add_style(btn1, LV_OBJ_PART_MAIN, &btn_style1);
    btn2 = lv_btn_create(cont1, NULL);
    lv_obj_set_size(btn2, 80, 80);
    lv_style_set_radius(&btn_style1, LV_STATE_DEFAULT, 20);
    lv_obj_add_style(btn2, LV_OBJ_PART_MAIN, &btn_style1);
    btn3 = lv_btn_create(cont1, btn2);
    btn4 = lv_btn_create(cont1, btn2);
#else
    lv_cont_set_layout(cont1, LV_LAYOUT_PRETTY_MID);
    lv_style_set_radius(&btn_style1, LV_STATE_DEFAULT, 20);
    btn1 = lv_btn_create(cont1, NULL);
    lv_obj_set_size(btn1, 80, 80);
    lv_obj_add_style(btn1,LV_OBJ_PART_MAIN, &btn_style1);
    btn2 = lv_btn_create(cont1, btn1);
    btn3 = lv_btn_create(cont1, btn1);
    btn4 = lv_btn_create(cont1, btn1);
    btn4 = lv_btn_create(cont1, btn1);
#endif
}


