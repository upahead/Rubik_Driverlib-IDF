file(GLOB_RECURSE SOURCES ./*.c)
idf_component_register(SRCS ${SOURCES}
		INCLUDE_DIRS include
		"${IDF_PATH}/components/fatfs/vfs"
		"${IDF_PATH}/components/fatfs/src"
		"${IDF_PATH}/components/wear_levelling/include"
		"${IDF_PATH}/components/spi_flash/include"
		"${IDF_PATH}/components/sdmmc/include" 		
)