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

// TODO: "extern" the two switch semaphores here

void  debounce_task_init(void);
void  debounce_task(void * p_arg);
extern OS_SEM g_sem_sw1;
extern OS_SEM g_sem_sw2;
#endif /* _PUSHBUTTON_H */
