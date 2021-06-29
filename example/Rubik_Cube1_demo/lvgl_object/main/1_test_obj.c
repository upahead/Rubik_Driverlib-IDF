#include "1_test_obj.h"

lv_style_t parent_style;
lv_style_t child_style;

void lv_test_obj(void)
{
    // Get screen object.
    lv_obj_t* screen = lv_scr_act(); 

    // Create a parent object, and set its size, position, and background.
    lv_obj_t* parent_obj = lv_obj_create(screen, NULL);
    lv_obj_set_size(parent_obj, 200, 200);
    lv_obj_align(parent_obj, screen, LV_ALIGN_CENTER, 0, 0);
    lv_style_init(&parent_style);
    lv_style_set_bg_color(&parent_style, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_style_set_bg_opa(&parent_style, LV_STATE_DEFAULT, LV_OPA_30);
    lv_obj_add_style(parent_obj, LV_OBJ_PART_MAIN, &parent_style);

    // Create a child object in the parent object, set its border style, and enable dragging.
    lv_obj_t* child_obj = lv_obj_create(parent_obj, NULL);
    lv_obj_set_size(child_obj, 80, 80);
    lv_obj_set_pos(child_obj, 20, 20);
    lv_style_init(&child_style);
    lv_style_set_outline_width(&child_style, LV_STATE_DEFAULT, 5);
    lv_style_set_outline_color(&child_style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_obj_add_style(child_obj, LV_OBJ_PART_MAIN, &child_style);
    lv_obj_set_drag(child_obj, true);
}