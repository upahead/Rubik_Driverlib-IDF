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
  * @file           tcp_driver.c
  * @version        1.0
  * @date           2021-6-28
  */

#include "tcp_driver.h"

static char TAG[] = "TCP driver";

/**
   * @brief  creates a TCP client to connect to the server.
   * @param[out] rt_socket returns the successfully connected socket through the pointer.
   * @param[in] server_ip server ipv4 address, string format.
   * @param[in] server_port server port (0-65535).
   * @retval  ESP_OK or ESP_ERR.
   * @note  If it fails, it should be created in a loop.
   */
esp_err_t TCP_CreateClient(uint32_t *rt_socket, char* server_ip, uint16_t server_port)
{
    ESP_LOGI(TAG, "will connect to the server.");
    
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(server_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(server_port);

    int32_t connect_socket = socket(dest_addr.sin_family, SOCK_STREAM, IPPROTO_IP);         // Create socket.
    if(connect_socket < 0) {                                            // Creation failed.
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", server_ip, server_port);

    int32_t err = connect(connect_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr)); // connect to the server.
    if (err != 0) {                                                     // Connection failed.
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(connect_socket);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Successfully connected");

    *rt_socket = connect_socket;                                        // Return the successfully created socket through the pointer.
    return ESP_OK;
}

/**
   * @brief  Create TCP server listening port.
   * @param[out]  rt_socket returns the monitored socket through the pointer.
   * @param[in]  server_port server port (0-65535).
   * @retval  ESP_OK or ESP_ERR.
   * @note  If you use wifiAP mode, the server IP is 192.168.4.1 If it fails, you should create it in a loop.
   */
esp_err_t TCP_CreateServer(uint32_t *rt_socket, uint16_t server_port)
{
    ESP_LOGI(TAG, "will create server.");

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(server_port);

    int32_t listen_socket = socket(dest_addr.sin_family, SOCK_STREAM, IPPROTO_IP);      // Create socket.
    if(listen_socket < 0) {                                             // Creation failed.
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Server socket created succeed");

    int32_t err = bind(listen_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));  // Port binding.
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        close(listen_socket);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", server_port);

    err = listen(listen_socket, 1);                                      // Listening socket.
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        close(listen_socket);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Socket listening...");

    struct sockaddr_in source_addr;                                     // Save client information.
    uint addr_len = sizeof(source_addr);
    int32_t connect_socket = accept(listen_socket, (struct sockaddr *)&source_addr, &addr_len);
    if (connect_socket < 0) {
        ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
        close(listen_socket);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Socket accepted succeed");

    *rt_socket = connect_socket;
    return ESP_OK; 
}

/**
   * @brief  socket sends a string (encapsulated printing error message).
   * @param[in]  socket  The socket to send data.
   * @param[in]  buf  string pointer to be sent.
   * @param[in]  len  The length of the string to be sent.
   * @retval  ESP_OK or ESP_ERR.
   * @note  does not have to use this function.
   */
esp_err_t TCP_SocketSendString(uint32_t socket, char *buf, uint32_t len)
{
   int32_t ret = send(socket, buf, len, 0);            
   if (ret < 0) {                                           
       ESP_LOGE(TAG, "Socket send errno : %d", errno);
       close(socket);
       return ESP_FAIL;
   }
   return ESP_OK;
}

/**
   * @brief  socket receives a string (encapsulates the printing error message), and adds 0 at the end of the string.
   * @param[in]  socket  The socket to accept data.
   * @param[out]  buf  string pointer to be accepted.
   * @param[in]  len  The length of the string to be received.
   * @retval  ESP_OK or ESP_ERR.
   * @note  does not have to use this function.
   */
esp_err_t TCP_SocketRecvString(uint32_t socket, char *buf, uint32_t len)
{
    int32_t ret = recv(socket, buf, len-1, 0);
    if(ret < 0) {
        ESP_LOGE(TAG, "Socket recv errno : %d", errno);
        close(socket);
        return ESP_FAIL;
    }
    buf[ret] = 0;
    return ESP_OK;
}

