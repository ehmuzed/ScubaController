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
#include "alarm.h"
#include "GUIDEMO_API.h"

OS_FLAG_GRP g_alarm_flags;

/*!
*
* @brief Alarm Task
*/
void alarm_task(void * p_arg)
{
  OS_ERR err;
  // TODO: This task should pend on events from adc_task(),
  //       and then update the LCD background color accordingly.
  for(;;)
  {
    OS_FLAGS alarm = OSFlagPend(
                &g_alarm_flags, 
                ALARM_NONE + ALARM_LOW + ALARM_MEDIUM + ALARM_HIGH + ALARM_EXTREME, 100,
                OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME + OS_OPT_PEND_BLOCKING,
                NULL, &err);
    if(err == OS_ERR_NONE)
    {
      switch(alarm)
      {
        case ALARM_NONE   : GUIDEMO_SetColorBG(BG_COLOR_GREEN); break;
        case ALARM_LOW    : GUIDEMO_SetColorBG(BG_COLOR_BLUE);  break;
        case ALARM_MEDIUM : GUIDEMO_SetColorBG(BG_COLOR_YELLOW);break;
        case ALARM_HIGH   : GUIDEMO_SetColorBG(BG_COLOR_ORANGE);   break;
        case ALARM_EXTREME: GUIDEMO_SetColorBG(BG_COLOR_RED);   break;
        default:
           break;
      }
    }
  }
}

