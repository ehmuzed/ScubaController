/** \file alarm.c
*
* @brief Alarm Manager
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Barr Group, LLC.
* All rights reserved.
*/

#include <stdint.h>
#include <stdio.h>
            
#include "project.h"
#include "os.h"
#include "GUIDEMO_API.h"

#include "alarm.h"
#include "scuba.h"


/*!
*
* @brief Alarm Task
*/
void alarm_task(void * p_arg)
{
  OS_ERR            err;        

  (void)p_arg;    // NOTE: Silence compiler warning about unused param.

  BG_COLOR  const bg_noalarm = BG_COLOR_GREEN;
  BG_COLOR  background_color = bg_noalarm;

  // Start off assuming no alarm
  GUIDEMO_SetColorBG(background_color);

  // Task infinite loop...
  for (;;)    
  {
    // Wait for a signal from another task.
    OS_FLAGS flags = OSFlagPend(&g_alarm_flags, 0x0FUL, 0UL,
      OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_BLOCKING,
      (CPU_TS *)0UL, &err);
    my_assert(OS_ERR_NONE == err);    
      
    // Ensure the proper alarm is indicated.
    if (flags & ALARM_HIGH)
    {
      // High priority alarm should be indicated.
      if (background_color != BG_COLOR_RED)
      {
        background_color = BG_COLOR_RED;
        GUIDEMO_SetColorBG(background_color);
      }
    }
    else if (flags & ALARM_MEDIUM)
    {
      // Medium priority alarm should be indicated.
      if (background_color != BG_COLOR_YELLOW)
      {
        background_color = BG_COLOR_YELLOW;
        GUIDEMO_SetColorBG(background_color);
      }
    }
    else if (flags & ALARM_LOW)
    {
      // Low priority alarm should be indicated.
      if (background_color != BG_COLOR_BLUE)
      {
        background_color = BG_COLOR_BLUE;
        GUIDEMO_SetColorBG(background_color);
      }
    }
    else if (flags & ALARM_NONE)
    {
      // No alarm should be indicated!
      if (background_color != bg_noalarm)
      {
        background_color = bg_noalarm;
        GUIDEMO_SetColorBG(background_color);
      }
    }
    else
    {
      // We should never get here.
      my_assert(0);
    }
  }
}


