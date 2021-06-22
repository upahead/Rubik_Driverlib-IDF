
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
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "sd_spi_vfs.h"

static const char *TAG = "SD_SPI_VFS";

/**
  * @brief  The SD card uses SPI to initialize and mount the vfs/fat file system.
  * @param[in]  sd_miso_num  SPI miso port.
  * @param[in]  sd_mosi_num  SPI mosi port.
  * @param[in]  sd_clk_num  SPI clk port.
  * @param[in]  sd_cs_num  SPI cs port.
  * @param[in]  mount_path  Mount path name, add'/' in front.
  * @param[in]  card_formatted  Whether the card should be formatted, after formatting, it is easier to hang in success.
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  * @note   You can also use mmc communication, see the official process sd_card routine for details.
  */
SD_Spi_Vfs_handle_t SD_Spi_Vfs_Init(gpio_num_t sd_miso_num, gpio_num_t sd_mosi_num, gpio_num_t sd_clk_num, 
                              gpio_num_t sd_cs_num, const char* mount_path, bool card_formatted)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing SD card");

    SD_Spi_Vfs_handle_t sd = malloc(sizeof(SD_Spi_Vfs_t));
    if(NULL == sd){
        ESP_LOGE(TAG, "Driver info malloc memory failed.\n");
        return NULL;
    }

    // Mount path.
    strncpy(sd->sd_mount_path, mount_path, strlen(mount_path));

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = card_formatted,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sd->sd_host = host.slot;
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = sd_mosi_num,
        .miso_io_num = sd_miso_num,
        .sclk_io_num = sd_clk_num,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    err = spi_bus_initialize(host.slot, &bus_cfg, 1);
    if(err != ESP_OK){
        ESP_LOGE(TAG, "Failed to initialize sd spi bus.");
        free(sd);
        return NULL;
    }
    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = sd_cs_num;
    slot_config.host_id = host.slot;
    err = esp_vfs_fat_sdspi_mount(mount_path, &host, &slot_config, &mount_config, &sd->sd_card);
    if (err != ESP_OK) {
        if (err == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(err));
        }
        free(sd);
        return NULL;
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, sd->sd_card);

    ESP_LOGI(TAG, "Init successful.");
    return sd;
}

/**
  * @brief  SD SPI to initialize, unmount
  * @param[in]  sd  sd spi driver handle pointer
  * @retval 
  *         - ESP_OK    successful.
  *         - ESP_FAIL  failed.
  */         
esp_err_t SD_Spi_Vfs_Deinit(SD_Spi_Vfs_handle_t* sd)
{   
    // Check whether the sd card has been initialized before deinitialization.
    if(NULL == (*sd)){
        ESP_LOGE(TAG, "SD card no init.");
        return ESP_FAIL;
    }

    esp_vfs_fat_sdcard_unmount((*sd)->sd_mount_path, (*sd)->sd_card);
    ESP_LOGI(TAG, "Card unmounted");
    spi_bus_free((*sd)->sd_host);
    free(*sd);
    (*sd) = NULL;
    return ESP_OK;
}
