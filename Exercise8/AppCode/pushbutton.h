/** \file pushbutton.h
*
* @brief Button Debouncer
*
* @par
* COPYRIGHT NOTICE: (C) 2017 Barr Group
* All rights reserved.
*/

#ifndef _PUSHBUTTON_H
#define _PUSHBUTTON_H

#include "os.h"

extern OS_SEM g_sw1_sem;
extern OS_SEM g_sw2_sem;

void  debounce_task_init(void);
void  debounce_task(void * p_arg);

#endif /* _PUSHBUTTON_H */
