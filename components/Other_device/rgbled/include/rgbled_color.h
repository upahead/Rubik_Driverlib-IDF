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
  * @file           rghled_color.h
  * @version        1.0
  * @date           2021-5-24
  */

#ifndef _RGBLED_COLOR_H_
#define _RGBLED_COLOR_H_

#include "rgbled_driver.h"

#define RGBLED_COLOR_RED        0xFF0000
#define RGBLED_COLOR_GREEN      0x00FF00
#define RGBLED_COLOR_BLUE       0x0000FF
#define RGBLED_COLOR_BLACK      0x000000
#define RGBLED_COLOR_WHITE      0xFFFFFF

RGBled_Color_t Rainbow_Color[10] = {0xFF0000, 0xFF8000, 0xFFFF00, 0x99FF00, 0x00FF00, 0x00FFB3, 0x00FFFF, 0x0000FF, 0xFF00FF, 0xFF0080};

#endif /* _RGBLED_COLOR_H_ */

