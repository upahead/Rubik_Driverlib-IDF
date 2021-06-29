file(GLOB_RECURSE SOURCES ./*.c)

idf_component_register(SRCS ${SOURCES}
		INCLUDE_DIRS include 		
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_LVGL_H_INCLUDE_SIMPLE")