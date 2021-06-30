#include "9_test_img.h"

lv_style_t style1;

lv_obj_t* img1;

// Declare the image to be added.
LV_IMG_DECLARE(true_color);
LV_IMG_DECLARE(true_color_alpha);       // Picture without background.
LV_IMG_DECLARE(true_color_chroma);      // To show chroma and hide the background color, you need to set the LV_COLOR_TRANSP macro color.
LV_IMG_DECLARE(image_file);

// extern lv_img_dsc_t image_file;

void lv_test_img(void)
{
    // Get screen object.
    lv_obj_t* screen = lv_scr_act(); 

    // Create style objects.
    lv_style_init(&style1);
    lv_style_set_image_recolor(&style1, LV_STATE_DEFAULT, LV_COLOR_RED);
    //lv_style_set_image_recolor_opa(&style1, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    //lv_style_set_image_opa(&style1, LV_STATE_DEFAULT, 128);     // Set transparency.

    img1 = lv_img_create(screen, NULL);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 20, 0);

    lv_obj_add_style(img1, LV_IMG_PART_MAIN, &style1);
    //lv_img_set_src(img1, LV_SYMBOL_WIFI" wi-fi");           // Display icon characters and text. Separate display text should be preceded by the LV_SYMBOL_DUMMY macro.

    lv_img_set_src(img1, &true_color_chroma);

    // Limit the display size so that his pictures are automatically filled.
    //lv_img_set_auto_size(img1, false);
    //lv_obj_set_size(img1, 320, 240);

    lv_img_set_zoom(img1, 1024);            // Scale the image, 256 is the benchmark
    lv_img_set_angle(img1, 900);            // Rotate the image, the accuracy is 0.1 so *10.

}
