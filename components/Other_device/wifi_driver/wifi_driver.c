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
  * @file           wifi_driver.c
  * @version        1.0
  * @date           2021-6-23
  */

#include "wifi_driver.h"
#include "esp_smartconfig.h"

static char TAG[] = "WIFI message";

static int s_retry_num = 0;

static esp_netif_t *netif_ap = NULL;
static esp_netif_t *netif_sta = NULL;

static bool sta_connect = false;

/**
   * @brief  wifi event processing callback function.
   * @param[in]  arg  Incoming parameter, here is NULL.
   * @param[in]  event_base  The unique pointer to the subsystem that exposes the event.
   * @param[in]  event_id  event ID, check esp_wifi_types.h.
   * @param[in]  event_data  event data.
   * @retval  void.
   * @note  Set the event group flag according to the wifi connection event.
   */
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {               // AP-device connection event.
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);    // Set the connection success event flag bit.
    }else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {      // AP-device disconnect event.
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);  // Clear the connection success event flag bit.
    }else if (event_id == WIFI_EVENT_STA_START) {               // STA-start connection.
        esp_wifi_connect();
    }else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {        // STA-disconnect.
        if (s_retry_num < 5) {                                  // Error reconnect.
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {                                                        // Exceeded exit.
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);      // Set the wrong connection flag.
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);// Clear the connection success flag.
        }
    }else if (event_id == WIFI_EVENT_SCAN_DONE){
        xEventGroupSetBits(s_wifi_event_group, WIFI_SCAN_DONE_BIT);     // Send scan complete flag.
    }
}

/**
   * @brief  ip event processing callback function.
   * @param[in]  arg  Incoming parameter, here is NULL.
   * @param[in]  event_base  The unique pointer to the subsystem that exposes the event.
   * @param[in]  event_id  event ID, check esp_wifi_types.h.
   * @param[in]  event_data  event data.
   * @retval  void.
   * @note  Set the event group flag according to the wifi connection event.
   */
static void ip_event_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    if (event_id == IP_EVENT_STA_GOT_IP) {               // STA-connected successfully.
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        sta_connect = true;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);     // Set the connection success flag.
    }
}

/**
   * @brief  smart config event processing callback function.
   * @param[in]  arg  Incoming parameter, here is NULL.
   * @param[in]  event_base  The unique pointer to the subsystem that exposes the event.
   * @param[in]  event_id  event ID.
   * @param[in]  event_data  event data.
   * @retval  void.
   * @note  print sc process information.
   */
static void sc_event_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    if(event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE){                       // sc scan successfully.
        ESP_LOGI(TAG, "SC  Scan done");
    }else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL){            // sc find channel.
        ESP_LOGI(TAG, "SC  Found channel");
    }else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD){            // sc get password.
        ESP_LOGI(TAG, "SC  Got SSID and password");
        // Extracted or obtained sc account password information.
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);

        ESP_ERROR_CHECK(esp_wifi_disconnect());                                     // wifi disconnect.
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));        // STA mode configured as target ap information.
        ESP_ERROR_CHECK(esp_wifi_connect());                                        // reconnect.
    }else if(event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE){         // After setting of sc, send ack to mobile phone.
        xEventGroupSetBits(s_wifi_event_group, WIFI_ESPTOUCH_DONE_BIT);
    }
}

/**
   * @brief initialize wifi and turn it on.
   * @param void.
   * @retval void.
   */
void WIFI_Init(void)
{
    s_wifi_event_group = xEventGroupCreate();               // Get event flag group.
    ESP_ERROR_CHECK(esp_netif_init());                      // Initialize the underlying TCP/IP stack.
    ESP_ERROR_CHECK(esp_event_loop_create_default() );      // Create a default event loop.
    netif_ap = esp_netif_create_default_wifi_ap();          // Create a default WIFI AP. In case of any init error, this API aborts.
    assert(netif_ap);
    netif_sta = esp_netif_create_default_wifi_sta();        // Create a default WIFI STA. In case of any init error, this API aborts.
    assert(netif_sta);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();    // The WiFi default stack configuration parameters are passed to the esp_wifi_init call.
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));                   // Init WiFi Alloc resources for the WiFi driver, such as WiFi control structure, RX/TX buffer, WiFi NVS structure, etc. This WiFi also starts WiFi tasks.
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));       // Register all wifi events to system events.
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL));           // Register all ip events to system events.
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &sc_event_handler, NULL) );          // Register all sc events to system events.
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));// Set WiFi API configuration storage type.
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));     // Don’t set it to any mode.
    sta_connect = false;
}

/**
   * @brief initialize ESP32 wifi to AP mode.
   * @param[in]  APSSID  softap ID.
   * @param[in]  APPASS  softap password, it will be ignored if it is less than 8 digits.
   * @retval  void
   * @note  Judge the wifi connection status by obtaining the s_wifi_event_group event group flag bit.
   * @note  This function will automatically turn on wifi, it is best to turn off wifi before use.
   */
void WIFI_Init_Softap(const char* APSSID, const char* APPASS)
{
    ESP_ERROR_CHECK(esp_wifi_start());
    wifi_config_t wifi_config = {                       // Configure wifi settings.
        .ap = {
            .ssid = "",                                         // wifi name.
            .ssid_len = 0,                                      // Optional SSID field length.        
            .channel = ESP_WIFI_CHANNEL_AP,                     // wifi channel.
            .password = "",                                     // wifi password.
            .max_connection = MAX_STA_CONN_AP,                  // The maximum number of stations allowed to connect, the default is 4, and the maximum is 10.
            .authmode = WIFI_AUTH_WPA_WPA2_PSK                  // ESP32 soft ap authentication mode.
        },
    };
    strlcpy((char*)wifi_config.ap.ssid, APSSID, sizeof(wifi_config.ap.ssid));
    if(strlen(APPASS) < 8){                           // If the password is empty, change the mode.
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
        ESP_LOGI(TAG, "password less than 8, The pattern is converted to auth open.");
    }else{
        strlcpy((char*)wifi_config.ap.password, APPASS, sizeof(wifi_config.ap.password));
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));                   // Set wifi operation mode.                
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config)); // Configure wifi to ap mode according to the parameters.                            
    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",      // Print wifi information.
             APSSID, APPASS, ESP_WIFI_CHANNEL_AP);
}

/**
   * @brief  ESP32 wifi scan wifiap.
   * @param[out]  ap_list  pointer returns the scanned ap information list.
   * @param[in]  list_size  Maximum number of scan APs.
   * @retval  The number of APs actually scanned.
   * @note  You need to view more information about the scan, you need to modify wifi_scan_list_t and modify the code to view more information.
   * @note  This function will automatically open and close the wifi, it is best to close the wifi before use.
   */
uint16_t WIFI_Scan(wifi_scan_list_t *ap_list, uint8_t list_size)
{
    ESP_ERROR_CHECK(esp_wifi_start());
    uint16_t number = list_size;
    wifi_ap_record_t ap_info[list_size];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    wifi_config_t wifi_config = {                       // Configure wifi STA settings.
        .sta = {
            .ssid = "",                                         // Target ID.
            .password = "",                                     // Target password.
            .scan_method = WIFI_ALL_CHANNEL_SCAN,               // Scan mode.
        },
    };
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));    // Set up wifi.
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, false));
    ESP_LOGI(TAG, "scan begin\n");
    xEventGroupWaitBits(s_wifi_event_group,             // Waiting for the scan completion flag.
                        WIFI_SCAN_DONE_BIT, 
                        pdFALSE, 
                        pdFALSE, 
                        portMAX_DELAY);
    xEventGroupClearBits(s_wifi_event_group, WIFI_SCAN_DONE_BIT);        // Clear scan completed flag.
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
    for (int i = 0; (i < list_size) && (i < ap_count); i++) {
        strcpy(ap_list[i].SSID, (char*)ap_info[i].ssid);
        ap_list[i].RSSI = ap_info[i].rssi;
    }
    WIFI_Off();
    return ap_count;
}

/**
   * @brief  initialize ESP32 wifi to STA mode.
   * @param  STASSID  sta target ID.
   * @param  STAPASS  sta target password.
   * @retval  void.
   * @note  Judge the wifi connection status by obtaining the s_wifi_event_group event group flag bit.
   * @note  This function will automatically turn on wifi, it is best to turn off wifi before use.
   */
void WIFI_Init_Sta(const char* STASSID, const char* STAPASS)
{
    ESP_ERROR_CHECK(esp_wifi_start());
    wifi_config_t wifi_config = {                       // Configure wifi STA settings.
        .sta = {
            .ssid = "",                                         // Target ID.
            .password = "",                                     // Target password.
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,           // ESP32 soft ap authentication mode.
            .pmf_cfg = {                                        // Configuration structure of the protected management framework.
                .capable = true,
                .required = false
            },
        },
    };
    strlcpy((char*)wifi_config.sta.ssid, STASSID, sizeof(wifi_config.sta.ssid));
    strlcpy((char*)wifi_config.sta.password, STAPASS, sizeof(wifi_config.sta.password));

    s_retry_num = 0;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));                      // Set mode.
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));    // Set up wifi.
    ESP_ERROR_CHECK(esp_wifi_connect());
    ESP_LOGI(TAG, "wifi init sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,              // Waiting for event flag group.
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, 
                                           pdFALSE, 
                                           pdFALSE, 
                                           portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT){                                     // connection succeeded.
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 STASSID, STAPASS);
        sta_connect = true;
    }else if(bits & WIFI_FAIL_BIT){                                     // Connection failed.
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 STASSID, STAPASS);
        sta_connect = false;
    }else{                                                              // Unexpected error.
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        sta_connect = false;
    }
}

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
void WIFI_Init_Apsta(const char* APSSID, const char* APPASS, const char* STASSID, const char* STAPASS)
{
    ESP_ERROR_CHECK(esp_wifi_start());
    wifi_config_t wifi_config = {                       // Configure wifi settings.
        .ap = {
            .ssid = "",                                             // wifi name.
            .ssid_len = 0,                                          // Optional SSID field length.         
            .channel = ESP_WIFI_CHANNEL_AP,                         // wifi channel.
            .password = "",                                         // wifi password.
            .max_connection = MAX_STA_CONN_AP,                      // The maximum number of stations allowed to connect, the default is 4, and the maximum is 10.
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,                     // ESP32 soft ap authentication mode.
        },
        .sta = {
            .ssid = "",                                             // Target ID.
            .password = "",                                         // Target password.
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,               // ESP32 soft ap authentication mode.
            .pmf_cfg = {                                            // Configuration structure of the protected management framework.
                .capable = true,
                .required = false
            },
        },
    };

    strlcpy((char*)wifi_config.ap.ssid, APSSID, sizeof(wifi_config.ap.ssid));
    if(strlen(APPASS) < 8){                                         // If the password is empty, change the mode.    
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
        ESP_LOGI(TAG, "password less than 8, The pattern is converted to auth open.");
    }else{
        strlcpy((char*)wifi_config.ap.password, APPASS, sizeof(wifi_config.ap.password));
    }
    strlcpy((char*)wifi_config.sta.ssid, STASSID, sizeof(wifi_config.sta.ssid));
    strlcpy((char*)wifi_config.sta.password, STAPASS, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));                    // Set mode.
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));    // Set up wifi.
    ESP_ERROR_CHECK(esp_wifi_connect());
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,              // Waiting for event flag group.
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, 
                                           pdFALSE, 
                                           pdFALSE, 
                                           portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT){                                         // connection succeeded.
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 STASSID, STAPASS);
        sta_connect = true;
    }else if(bits & WIFI_FAIL_BIT){                                         // Connection failed.
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 STASSID, STAPASS);
        sta_connect = false;
    }else{                                                                  // Unexpected error.
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

/**
   * @brief  ESP32 start SmartConfig.
   * @param  void.
   * @retval  void.
   * @note  supports the ESP TOUCH mode by default, and the wifi is in the sta networking state after the network configuration is successful.
   * @note  This function will automatically turn on wifi, it is best to turn off wifi before use.
   */
void WIFI_Smart_Config(void)
{
    ESP_ERROR_CHECK(esp_wifi_start());
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));                      // Set mode.
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));            // Set the protocol type of SmartConfig.
    smartconfig_start_config_t sc_cfg = SMARTCONFIG_START_CONFIG_DEFAULT(); // Make smartconfig log (for the time being there is only one).
    ESP_ERROR_CHECK(esp_smartconfig_start(&sc_cfg));                        // Start SmartConfig, config ESP device to connect to AP, need to broadcast information through mobile phone APP.
    while(1){                                                               // Waiting for event flag group.
        uxBits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);    
        if(uxBits & WIFI_CONNECTED_BIT){                        // Connected to ap successfully.
            ESP_LOGI(TAG, "WiFi Connected to ap");
            sta_connect = true;
        }
        if(uxBits & WIFI_ESPTOUCH_DONE_BIT){                    // sc configuration is complete.
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();                             // Stop sc.
            return;
        }
    }
}

/**
   * @brief  ESP32 turn on wifi.
   * @param  void.
   * @retval  void.
   * @note  has automatically turned on wifi in the mode initialization function, this function is not required to be called.
   */
void WIFI_On(void)
{
    ESP_ERROR_CHECK(esp_wifi_start());
}

/**
   * @brief  ESP32 turn off wifi.
   * @param  void.
   * @retval  void.
   * @note  Disconnect if wifista is already connected.
   */
void WIFI_Off(void)
{   
    if(true == sta_connect){
        s_retry_num = 5;
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        xEventGroupWaitBits(s_wifi_event_group,                         // Waiting for disconnection event flag group.
                            WIFI_FAIL_BIT, 
                            pdFALSE, 
                            pdFALSE, 
                            portMAX_DELAY);
        xEventGroupClearBits(s_wifi_event_group, WIFI_FAIL_BIT);        // Clear the connection failure flag.
        sta_connect = false;
        ESP_LOGI(TAG, "sta disconnect\n");
    }

    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
}
