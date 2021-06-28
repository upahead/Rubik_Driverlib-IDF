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
  * @file           tcp_driver.h
  * @version        1.0
  * @date           2021-6-28
  */

#ifndef __TCP_DRIVER_H
#define __TCP_DRIVER_H

#include <stdio.h>
#include <string.h>
#include <lwip/netdb.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

// /*需要连接服务器的地址*/
// #define CLIENT_TCP_SERVER_IP            "192.168.43.73"
// #define CLIENT_TCP_SERVER_PORT          9999

// /*创建服务器的端口*/
// #define SERVER_TCP_SERVER_PORT          9999

/**
   * @brief  creates a TCP client to connect to the server.
   * @param[out] rt_socket returns the successfully connected socket through the pointer.
   * @param[in] server_ip server ipv4 address, string format.
   * @param[in] server_port server port (0-65535).
   * @retval  ESP_OK or ESP_ERR.
   * @note  If it fails, it should be created in a loop.
   */
esp_err_t TCP_CreateClient(uint32_t *rt_socket, char* server_ip, uint16_t server_port);

/**
   * @brief  Create TCP server listening port.
   * @param[out]  rt_socket returns the monitored socket through the pointer.
   * @param[in]  server_port server port (0-65535).
   * @retval  ESP_OK or ESP_ERR.
   * @note  If you use wifiAP mode, the server IP is 192.168.4.1 If it fails, you should create it in a loop.
   */
esp_err_t TCP_CreateServer(uint32_t *rt_socket, uint16_t server_port);

/**
   * @brief  socket sends a string (encapsulated printing error message).
   * @param[in]  socket  The socket to send data.
   * @param[in]  buf  string pointer to be sent.
   * @param[in]  len  The length of the string to be sent.
   * @retval  ESP_OK or ESP_ERR.
   * @note  does not have to use this function.
   */
esp_err_t TCP_SocketSendString(uint32_t socket, char *buf, uint32_t len);

/**
   * @brief  socket receives a string (encapsulates the printing error message), and adds 0 at the end of the string.
   * @param[in]  socket  The socket to accept data.
   * @param[out]  buf  string pointer to be accepted.
   * @param[in]  len  The length of the string to be received.
   * @retval  ESP_OK or ESP_ERR.
   * @note  does not have to use this function.
   */
esp_err_t TCP_SocketRecvString(uint32_t socket, char *buf, uint32_t len);

#endif /* __TCP_DRIVER_H */