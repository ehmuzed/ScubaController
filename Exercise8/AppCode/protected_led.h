
/** \file protectedled.h
*
* @brief Reentrant LED driver API.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Barr Group, LLC.
* All rights reserved.
*/

#include "bsp_led.h"

extern void protectedLED_Init(void);
extern void protectedLED_Toggle(BOARD_LED_ID led);

