/*****************************************************************************
 *                                                                           *
 *  Copyright 2021 upahead PTE LTD                                           *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *      http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/
/**
  * @file           wifi_driver.h
  * @version        1.0
  * @date           2021-6-23
  */

#ifndef __WIFI_DRIVER_H
#define __WIFI_DRIVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/*-------------AP-------------*/
#define ESP_WIFI_CHANNEL_AP         1                   // AP wifi channel.
#define MAX_STA_CONN_AP             5                   // AP maximum number of connections.

/*-------------STA------------*/
#define DEFAULT_SCAN_LIST_SIZE      15                  // Maximum number of scans.

#define WIFI_CONNECTED_BIT          BIT0                // Connection successful event flag.
#define WIFI_FAIL_BIT               BIT1                // STA connection failure event flag bit.
#define WIFI_SCAN_DONE_BIT          BIT2                // wifi scan completed flag.
#define WIFI_ESPTOUCH_DONE_BIT      BIT3                // wifiSC configuration completed.

// The external file can obtain the success or failure status of the wifi connection by obtaining the event flag group.
EventGroupHandle_t                  s_wifi_event_group;        // Event flag group.

typedef struct {
    char SSID[33];
    int8_t RSSI;
}wifi_scan_list_t;                    // wifi scan returns data structure.

/**
   * @brief initialize wifi and turn it on.
   * @param void.
   * @retval void.
   */
void WIFI_Init(void);

/**
   * @brief initialize ESP32 wifi to AP mode.
   * @param[in]  APSSID  softap ID.
   * @param[in]  APPASS  softap password, it will be ignored if it is less than 8 digits.
   * @retval  void
   * @note  Judge the wifi connection status by obtaining the s_wifi_event_group event group flag bit.
   * @note  This function will automatically turn on wifi, it is best to turn off wifi before use.
   */
void WIFI_Init_Softap(const char* APSSID, const char* APPASS);

/**
   * @brief  ESP32 wifi scan wifiap.
   * @param[out]  ap_list  pointer returns the scanned ap information list.
   * @param[in]  list_size  Maximum number of scan APs.
   * @retval  The number of APs actually scanned.
   * @note  You need to view more information about the scan, you need to modify wifi_scan_list_t and modify the code to view more information.
   * @note  This function will automatically open and close the wifi, it is best to close the wifi before use.
   */
uint16_t WIFI_Scan(wifi_scan_list_t *ap_list, uint8_t list_size);

/**
   * @brief  initialize ESP32 wifi to STA mode.
   * @param  STASSID  sta target ID.
   * @param  STAPASS  sta target password.
   * @retval  void.
   * @note  Judge the wifi connection status by obtaining the s_wifi_event_group event group flag bit.
   * @note  This function will automatically turn on wifi, it is best to turn off wifi before use.
   */
void WIFI_Init_Sta(const char* STASSID, const char* STAPASS);

/**
   * @brief  initializes ESP32 wifi to APSTA (bridged).
   * @param  APSSID  softap ID.
   * @param  APPASS  softap password, it will be ignored if it is less than 8 digits.
   * @param  STASSID sta target ID.
   * @param  STAPASS sta target password.
   * @retval  void.
   * @note  Judge the wifi connection status by obtaining the s_wifi_event_group event group flag bit.
   * @note  This function will automatically turn on wifi, it is best to turn off wifi before use.
   */
void WIFI_Init_Apsta(const char* APSSID, const char* APPASS, const char* STASSID, const char* STAPASS);

/**
   * @brief  ESP32 start SmartConfig.
   * @param  void.
   * @retval  void.
   * @note  supports the ESP TOUCH mode by default, and the wifi is in the sta networking state after the network configuration is successful.
   * @note  This function will automatically turn on wifi, it is best to turn off wifi before use.
   */
void WIFI_Smart_Config(void);

/**
   * @brief  ESP32 turn on wifi.
   * @param  void.
   * @retval  void.
   * @note  has automatically turned on wifi in the mode initialization function, this function is not required to be called.
   */
void WIFI_On(void);

/**
   * @brief  ESP32 turn off wifi.
   * @param  void.
   * @retval  void.
   * @note  Disconnect if wifista is already connected.
   */
void WIFI_Off(void);

#endif /* __WIFI_DRIVER_H */