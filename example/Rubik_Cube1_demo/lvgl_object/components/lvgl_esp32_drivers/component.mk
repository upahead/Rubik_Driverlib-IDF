# LVGL esp32 drivers 

COMPONENT_SRCDIRS := .

COMPONENT_ADD_INCLUDEDIRS := .
COMPONENT_ADD_INCLUDEDIRS += lvgl_tft
COMPONENT_ADD_INCLUDEDIRS += lvgl_touch

CFLAGS += -DLV_LVGL_H_INCLUDE_SIMPLE