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
  * @file           rghled_show.h
  * @version        1.0
  * @date           2021-5-24
  */

#ifndef _RGBLED_SHOW_H_
#define _RGBLED_SHOW_H_

#include "rgbled_driver.h"

/**
  * @brief  RGBled RGB Gradient.
  * @param[in]  rgb_handle RGBled operation handle.
  * @param[in]  time  Intervals time. (Fastest is 1)
  * @retval void
  * @note   It is recommended to use a separate task to call this function. 
  *         In order to be compatible with task scheduling, it is not recommended 
  *         to set a too low gradient time, which may affect the operation of 
  *         other tasks.
  */
void RGBled_ShowGradient(RGBled_handle_t rgb_handle, uint32_t time);

/**
  * @brief  RGBled Color bar scroll.
  * @param[in]  rgb_handle RGBled operation handle.
  * @param[in]  color_list Color data array, the same length as the number of lights.
  * @param[in]  delay_time Time between each scroll.
  * @retval void
  */
void RGBled_ShowColorRoll(RGBled_handle_t rgb_handle, RGBled_Color_t* color_list, uint32_t delay_time);

/**
  * @brief  RGBled Color fill enter.
  * @param[in]  rgb_handle  RGBled operation handle.
  * @param[in]  color  RGB888 color data.
  * @param[in]  delay_time  Filling interval time.
  * @retval void
  */
void RGBled_ShowFillIn(RGBled_handle_t rgb_handle, RGBled_Color_t color, uint32_t delay_time);

#endif /* _RGBLED_SHOW_H_ */