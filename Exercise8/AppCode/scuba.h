/** \file scuba.h
*
* @brief Dive computer helper functions.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Barr Group
* All rights reserved.
*/

#ifndef _SCUBA_H
#define _SCUBA_H

#define DATA_DIRTY    (1UL << (0ul))
enum { MAX_AIR_IN_CL = (2000 * 100) };  // Compressed air capacity (in cL).
enum { MAX_DEPTH_IN_M = 40 };             // Maximum safe depth (in meters).
enum { ASCENT_RATE_LIMIT = 15 };          // Maximum safe ascent rate (in m/min).
enum { STATE_SURFACE = 0, STATE_DIVE = 1};
enum { IS_IMPERIAL = 0, IS_METRIC= 1};
#define MM2FT(mm)   ((mm) / 305)    // NOTE: It's actually 304.8 mm to a 1 ft.

#define ADC2RATE(adc)  (((adc) >= 524) ? (((adc) - 523) / 10) : \
                       (((adc) >= 500) ? 0 : (((adc) - 500) / 10)))

#define depth_change_in_mm(ascent_rate_in_m) \
  (((ascent_rate_in_m) * 1000) / (2 * 60))
#define depth_rate_in_mm(ascent_rate_in_m) \
  (((ascent_rate_in_m) * 1000))
uint32_t gas_rate_in_cl(uint32_t depth_in_mm);
uint32_t gas_to_surface_in_cl(uint32_t depth_in_mm);

void TimerCallback ( OS_TMR *p_tmr, void *p_arg); 
typedef struct {
  uint32_t state;
  uint32_t b_is_metric;
  uint32_t edt;
  int32_t dive_rate_mm;
  uint32_t air_volume;
} Scuba_t;

extern Scuba_t g_scuba_data;

extern OS_MUTEX g_mutex_scuba_data;
extern OS_FLAG_GRP g_data_dirty;
extern OS_TMR g_edt_timer;

extern void display_task(void * p_arg);
extern void edt_task(void * p_arg);
#endif /* _SCUBA_H */

