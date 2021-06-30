#include "8_test_tabview.h"

lv_style_t* style1;

lv_obj_t* tabview1;

lv_obj_t* tab_page1;
lv_obj_t* tab_page2;
lv_obj_t* tab_page3;

void lv_test_tabview(void)
{
    // Get screen object.
    lv_obj_t* screen = lv_scr_act(); 

    // Create a tabview object.
    tabview1 = lv_tabview_create(screen, NULL);
    lv_obj_set_size(tabview1, 320, 240);

    // Add three pages, each page is a page.
    tab_page1 = lv_tabview_add_tab(tabview1, "TAB1");
    lv_obj_t* label1 = lv_label_create(tab_page1, NULL);
    lv_label_set_text(label1, "HELLO I'm table 1.");
    tab_page2 = lv_tabview_add_tab(tabview1, "TAB2");
    lv_obj_t* label2 = lv_label_create(tab_page2, NULL);
    lv_label_set_text(label2, "HELLO I'm table 2.");
    tab_page3 = lv_tabview_add_tab(tabview1, "TAB3");
    lv_obj_t* label3 = lv_label_create(tab_page3, NULL);
    lv_label_set_text(label3, "HELLO I'm table 3.");

    //lv_tabview_set_btns_pos(tabview1, LV_TABVIEW_TAB_POS_NONE);     // Hide button options bar.
    //lv_tabview_set_btns_pos(tabview1, LV_TABVIEW_TAB_POS_BOTTOM);   // Set button press position.
    lv_tabview_set_anim_time(tabview1, 200);                       // Set animation duration.
    lv_tabview_set_tab_act(tabview1, 1, LV_ANIM_ON);                // Set tab2 as the selected page and start the animation.

    //int tabcount = lv_tabview_get_tab_count(tabview1);                // Get the number of pages.
    //int tabnum = lv_tabview_get_tab_act(tabview1);                    // Get the page number in advance.
}
