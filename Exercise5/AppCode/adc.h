/** \file adc.h
*
* @brief ADC Task interface
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Barr Group, LLC.
* All rights reserved.
*/
#ifndef _ADC_H
#define _ADC_H
extern OS_Q g_adc_q;
extern void     adc_task (void * p_arg);
extern void     ADC_IRQHandler(void);

#endif /* _ADC_H */
