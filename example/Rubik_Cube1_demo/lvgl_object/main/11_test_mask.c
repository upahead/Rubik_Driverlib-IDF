#include "11_test_mask.h"

static lv_obj_t * om;

static lv_objmask_mask_t* msk;

static lv_draw_mask_radius_param_t r1;


static void mask_change_area_anim(lv_anim_t * a, lv_anim_value_t v);


static void mask_area_animation()
{
    lv_anim_t a;
    lv_anim_init(&a); 
    lv_anim_set_var(&a, om);
    lv_anim_set_time(&a, 500);
    lv_anim_set_delay(&a, 0);

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)mask_change_area_anim);
    lv_anim_set_values(&a, 0, 120);


    lv_anim_start(&a); 
}

void lv_test_mask(void)
{
    /*Set a very visible color for the screen to clearly see what happens*/
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex3(0xf00));

    om = lv_objmask_create(lv_scr_act(), NULL);
    lv_obj_set_size(om, 320, 240);
    lv_obj_align(om, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * label = lv_label_create(om, NULL);
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(label, 180);
    lv_label_set_text(label, "This label will be masked out. See how it works.");
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

    lv_obj_t * cont = lv_cont_create(om, NULL);
    lv_obj_set_size(cont, 240, 240);
    lv_obj_set_drag(cont, true);
    lv_obj_align(cont, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

    lv_obj_t * btn = lv_btn_create(cont, NULL);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "Button");
    uint32_t t;

    // lv_refr_now(NULL);
    // t = lv_tick_get();
    // while(lv_tick_elaps(t) < 1000);

    lv_area_t a;
    
    a.x1 = 60;
    a.y1 = 20;
    a.x2 = 260;
    a.y2 = 220;
    lv_draw_mask_radius_init(&r1, &a, LV_RADIUS_CIRCLE, false);
    msk = lv_objmask_add_mask(om, &r1);

    mask_area_animation();

}


static void mask_change_area_anim(lv_anim_t * a, lv_anim_value_t v)
{
    lv_area_t area;
    area.x1 = 60+v/2;
    area.y1 = 20+v/2;
    area.x2 = 260-v/2;
    area.y2 = 220-v/2;
    lv_draw_mask_radius_init(&r1, &area, LV_RADIUS_CIRCLE, false);
    lv_objmask_update_mask(om, msk, &r1);
}