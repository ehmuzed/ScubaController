/** \file scuba.c

 $COPYRIGHT: $
 -    Copyright 2017 Barr Group, LLC. All rights reserved.
**/

#include <stdlib.h>  // NULL
#include <stdio.h>   // sprintf()
#include <stdint.h>
#include "os.h"
#include "project.h"
#include "GUIDEMO_API.h"  // write to LCD
#include "scuba.h"
#include "alarm.h"

#define BUF_SIZE 64
#define RMV   (1200UL)        /**< Respiratory minute volume = 1200 centiLitres / minute */
#define RHSV (RMV / 120UL)    /**< Respiratory half second volume = 10 centiLitres / half_second */

// Reset Values
Scuba_t g_scuba_data = {
  .state = STATE_SURFACE,
  .b_is_metric = IS_METRIC,
  .edt = 0,
  .dive_rate_mm = -60000,
  .depth_mm = 0,
  .air_volume = 50 * 100,
  .gas_to_surface = 0,
  .alarm = ALARM_NONE
};

/**
 FUNCTION: gas_rate_in_cl

 DESCRIPTION:
 This computes how much gas is consumed in a half second at a certain depth.

 PARAMETERS:
 -    The current depth in meters

 RETURNS:
 -    The number of centilitres in gas.

 NOTES:


**/
uint32_t gas_rate_in_cl(uint32_t depth_in_mm)
{
  uint32_t  depth_in_m = (depth_in_mm / 1000UL);    

   // 10m of water = 1 bar = 100 centibar
   uint16_t ambient_pressure_in_cb = 100UL + (10UL * depth_in_m);    
            
   // Gas consumed at STP = RHSV * ambient pressure / standard pressure
   return ((RHSV * ambient_pressure_in_cb) / 100UL);
}

/**
 FUNCTION: gas_to_surface_in_cl

 DESCRIPTION:
 This computes how much gas at STP it would take to surface from the current
 depth, assuming no decompression stops and an ascent rate of ASCENT_RATE_LIMIT.

 It does this via numerical integration. The step size is 1 m.

 PARAMETERS:
 -    The current depth in meters

 RETURNS:
 -    The number of centilitres of gas at STP required to make it to the surface.

 NOTES:


**/
uint32_t gas_to_surface_in_cl(uint32_t depth_in_mm)
{
  uint32_t    gas = 0UL;
  uint16_t    halfsecs_to_ascend_1m = (2UL * 60UL) / ASCENT_RATE_LIMIT;;
  uint16_t    ambient_pressure_in_cb;        /* Ambient pressure in centiBar */

  for (uint32_t depth_in_m = depth_in_mm / 1000UL; depth_in_m > 0UL; depth_in_m--)
  {
    ambient_pressure_in_cb = 100UL + ((depth_in_m * 100UL) / 10UL);
    gas += ((RHSV * halfsecs_to_ascend_1m * ambient_pressure_in_cb) / 100UL);
  }

  return (gas);
}

void display_task(void * p_arg)
{
  OS_ERR            err;        
  
  char  p_str[BUF_SIZE];
  uint32_t is_metric;
  (void)p_arg;    // NOTE: Silence compiler warning about unused param.
  for(;;)
  {
    OS_FLAGS flags = OSFlagPend(&g_data_dirty, DATA_DIRTY, 0UL,
      OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_BLOCKING,
      (CPU_TS *)0UL, &err);
   // my_assert(OS_ERR_NONE == err);   
        // Acquire the mutex.
    OSMutexPend(&g_mutex_scuba_data, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    //my_assert(OS_ERR_NONE == err);
    int32_t dive_rate_mm = g_scuba_data.dive_rate_mm;
    uint32_t edt_s = g_scuba_data.edt / 2;   
    uint32_t air_cl = g_scuba_data.air_volume;    
    uint32_t gas_cl = g_scuba_data.gas_to_surface;    
    is_metric = g_scuba_data.b_is_metric;
    int32_t depth_mm = g_scuba_data.depth_mm;
    uint32_t alarm = g_scuba_data.alarm;
    // Release the mutex.
    OSMutexPost(&g_mutex_scuba_data, OS_OPT_POST_NONE, &err);
    
    GUIDEMO_API_writeLine(0, "DIVE BUDDY(tm)");
    
    // I'm sorry
    snprintf(p_str, BUF_SIZE, 
             depth_mm == 0 ? "Depth: Surface" :"Depth: %c%d %s",
             depth_mm > 999 ? '-': ' ', 
              is_metric == IS_METRIC ? depth_mm / 1000  :
                                          MM2FT(depth_mm) ,
             is_metric == IS_METRIC ? "m" : 
                                      "ft"); 
    GUIDEMO_API_writeLine(2, p_str);
    snprintf(p_str, BUF_SIZE, depth_mm == 0 ? 
                          "Ascent Rate:  At Surface" : 
                          "Ascent Rate: %d %s/min", 
             is_metric == IS_METRIC ? dive_rate_mm / 1000 : 
                                      MM2FT(dive_rate_mm), 
             is_metric == IS_METRIC ? "m" : 
                                      "ft"); 
    GUIDEMO_API_writeLine(3, p_str);
    snprintf(p_str, BUF_SIZE, "Air: %d L", air_cl/100); 
    GUIDEMO_API_writeLine(4, p_str);
    snprintf(p_str, BUF_SIZE, "EDT: %02d:%02d:%02d",
             edt_s / 3600,
             (edt_s / 60 )% 60,
             edt_s % 60); 
    GUIDEMO_API_writeLine(5, p_str);
    //snprintf(p_str, BUF_SIZE, "Gas to Surface: %d L", gas_cl/100); 
    //GUIDEMO_API_writeLine(5, p_str); 
    snprintf(p_str, BUF_SIZE, "Alarm: %s", 
             alarm == ALARM_HIGH   ? "HIGH"     : 
             alarm == ALARM_MEDIUM ? "MEDIUM"   : 
             alarm == ALARM_LOW    ? "LOW"      :
                                     "NONE"); 
    GUIDEMO_API_writeLine(7, p_str); 
    //my_assert(OS_ERR_NONE == err);
  }
}

void edt_task(void * p_arg)
{
  OS_ERR            err;        
  (void)p_arg;    // NOTE: Silence compiler warning about unused param.
  for(;;)
  {
        OSTimeDlyHMSM(10, 0, 0, 0, 0, &err);        
  }
}


void TimerCallback ( OS_TMR *p_tmr, void *p_arg)
{
  OS_ERR            err; 
  uint32_t alarm_level = ALARM_NONE;
  uint32_t prev_alarm;
  OSMutexPend(&g_mutex_scuba_data, 0, OS_OPT_PEND_BLOCKING, 0, &err);
  prev_alarm = g_scuba_data.alarm;
  g_scuba_data.depth_mm-=depth_change_in_mm(g_scuba_data.dive_rate_mm/1000);
  if(g_scuba_data.depth_mm < 0)
  {
    g_scuba_data.depth_mm = 0;
  }
  if(g_scuba_data.depth_mm > 0)
  {
    g_scuba_data.gas_to_surface = gas_to_surface_in_cl(g_scuba_data.depth_mm);
    g_scuba_data.air_volume-=gas_rate_in_cl(g_scuba_data.depth_mm);
    if(g_scuba_data.air_volume <= 0)
    {
      g_scuba_data.air_volume=0;
      //you are dead
    }
  }
  if(g_scuba_data.depth_mm > (40 * 1000))
  {
    alarm_level = ALARM_LOW;
  }  
  if(g_scuba_data.dive_rate_mm > (15 * 1000) && g_scuba_data.depth_mm > 0)
  {
    alarm_level = ALARM_MEDIUM;
  }
  if(g_scuba_data.air_volume < g_scuba_data.gas_to_surface)
  {
    alarm_level = ALARM_HIGH ;
  }
  if(g_scuba_data.depth_mm != 0 )
  {
     g_scuba_data.edt++;
  }
  if(g_scuba_data.depth_mm == 0 && g_scuba_data.dive_rate_mm < 0)
  {
    g_scuba_data.edt = 0;
  }
  g_scuba_data.alarm = alarm_level;
  OSMutexPost(&g_mutex_scuba_data, OS_OPT_POST_NONE, &err);      
  OSFlagPost(&g_data_dirty, DATA_DIRTY, OS_OPT_POST_FLAG_SET, &err);
  if(prev_alarm != alarm_level)
  {
    OSFlagPost(&g_alarm_flags, alarm_level, OS_OPT_POST_FLAG_SET, &err);
  }
}


