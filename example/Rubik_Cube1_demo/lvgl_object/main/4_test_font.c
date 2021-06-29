#include "4_test_font.h"

// Two font objects can also be merged (combined and generated when generating the font file).
LV_FONT_DECLARE(my_font);
LV_FONT_DECLARE(my_font_icon_30);

// After using the ttf file to generate the font, the unicode encoding of the characters needs to be converted to utf-8 encoding.
#define LV_ICON_SOUND_OFF   "\xee\x98\xa3"  //0xe623 Volume off icon.
#define LV_ICON_SOUND_ON    "\xee\x98\xa4"  //0xe624 Volume on icon.
#define LV_ICON_SD_CARD     "\xee\x9b\x85"  //0xe6c5 SD card icon.
#define LV_ICON_BLUETOOTH   "\xee\xa1\xb3"  //0xe873 Bluetooth icon.
#define LV_ICON_WIFI        "\xee\x9e\xbf"  //0xe7bf WIFI icon.

static lv_style_t style1;
static lv_obj_t* label1;

void lv_test_font(void)
{
    // Get screen object.
    lv_obj_t* screen = lv_scr_act(); 

    // Set your own font.
    lv_style_init(&style1);
    // lv_style_copy(&style1, LV_FONT_MONTSERRAT_16);
    lv_style_set_text_font(&style1, LV_STATE_DEFAULT, &my_font_icon_30);

    // Create label and add font to print text.
    label1 = lv_label_create(screen, NULL);
    lv_obj_add_style(label1, LV_OBJ_PART_MAIN, &style1);
    lv_label_set_text(label1, LV_ICON_SOUND_OFF LV_ICON_SOUND_ON LV_ICON_SD_CARD LV_ICON_BLUETOOTH LV_ICON_WIFI LV_SYMBOL_WARNING);
}





