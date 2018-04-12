
/** \file protectedled.c
*
* @brief Reentrant LED driver.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Barr Group, LLC.
* All rights reserved.
*/

#include "os.h"
#include "project.h"
#include "protected_led.h"
#include <stdint.h>


// Private mutex for LED hardware.
static OS_MUTEX  g_led_mutex;

/*!
* @brief Initialize the reentrant LED driver.
*/
void protectedLED_Init(void)
{
    OS_ERR err;

   // Create the mutex that protects the hardware from race conditions.
   OSMutexCreate(&g_led_mutex, "LED Mutex", &err);
   my_assert(OS_ERR_NONE == err);
}

/*!
* @brief Toggle an LED, safely.
* @param[in] led The LED to toggle.
*/
void protectedLED_Toggle(BOARD_LED_ID led)
{
    OS_ERR   err;

    // Acquire the mutex.
    OSMutexPend(&g_led_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    my_assert(OS_ERR_NONE == err);

    // Safely inside the critical section, call non-reentrant driver.
    BSP_LED_Toggle(led);

    // Release the mutex.
    OSMutexPost(&g_led_mutex, OS_OPT_POST_NONE, &err);
    my_assert(OS_ERR_NONE == err);
}

