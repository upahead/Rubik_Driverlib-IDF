
#ifndef __SD_SPI_VFS_H
#define __SD_SPI_VFS_H

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "driver/gpio.h"

// sd spi驱动句柄
typedef struct{
    char sd_mount_path[20];
    sdmmc_card_t* sd_card;
    spi_host_device_t sd_host;
}SD_Spi_Vfs_t;
typedef SD_Spi_Vfs_t *SD_Spi_Vfs_handle_t;

/**
  * @brief  SD卡使用SPI初始化，挂载vfs/fat文件系统
  * @param  sd_miso_num：SPI miso端口
  * @param  sd_mosi_num: SPI mosi端口
  * @param  sd_clk_num：SPI clk端口
  * @param  sd_cs_num：
  * @param  mount_path：挂载路径名，前面要加'/'
  * @param  card_formatted：卡是否要格式化，格式化后更容易挂在成功
  * @retval ESP_OK或ESP_ERR
  * @note   也可以使用mmc通讯，具体见官方历程sd_card例程
  */
SD_Spi_Vfs_handle_t SD_Spi_Vfs_Init(gpio_num_t sd_miso_num, gpio_num_t sd_mosi_num, gpio_num_t sd_clk_num, 
                              gpio_num_t sd_cs_num, const char* mount_path, bool card_formatted);

/**
  * @brief  SD SPI 去初始化，解除挂载
  * @param  sd ： sd spi 驱动句柄指针
  * @retval ESP_OK或ESP_ERR
  */                              
esp_err_t SD_Spi_Vfs_Deinit(SD_Spi_Vfs_handle_t* sd);

#endif /* __SD_SPI_VFS_H */