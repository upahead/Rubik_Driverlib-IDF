#include "2_test_label.h"

void lv_test_label(void)
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