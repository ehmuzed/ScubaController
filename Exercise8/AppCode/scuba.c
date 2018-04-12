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


#define RMV   (1200UL)        /**< Respiratory minute volume = 1200 centiLitres / minute */
#define RHSV (RMV / 120UL)    /**< Respiratory half second volume = 10 centiLitres / half_second */

// Reset Values
Scuba_t g_scuba_data = {
  .state = STATE_SURFACE,
  .b_is_metric = IS_METRIC,
  .edt = 0,
  .dive_rate_mm = -60000,
  .air_volume = 50
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
  char  p_str[24];
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
    uint32_t dive_rate_mm = g_scuba_data.dive_rate_mm;
    uint32_t edt_s = g_scuba_data.edt / 2;    
    is_metric = g_scuba_data.b_is_metric;
    // Release the mutex.
    OSMutexPost(&g_mutex_scuba_data, OS_OPT_POST_NONE, &err);
    
    snprintf(p_str, 24, "Dive Rate: %d %s/min", 
             is_metric == IS_METRIC ? dive_rate_mm / 1000 : 
                                      MM2FT(dive_rate_mm), 
             is_metric == IS_METRIC ? "m" : 
                                      "ft"); 
    GUIDEMO_API_writeLine(2, p_str);
    snprintf(p_str, 24, "EDT: %02d:%02d:%02d",
             edt_s / 3600,
             (edt_s / 60 )% 60,
             edt_s % 60); 
    GUIDEMO_API_writeLine(0, p_str);
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
  OSMutexPend(&g_mutex_scuba_data, 0, OS_OPT_PEND_BLOCKING, 0, &err);
  g_scuba_data.edt++;
  OSMutexPost(&g_mutex_scuba_data, OS_OPT_POST_NONE, &err);      
  OSFlagPost(&g_data_dirty, DATA_DIRTY, OS_OPT_POST_FLAG_SET, &err);
}


