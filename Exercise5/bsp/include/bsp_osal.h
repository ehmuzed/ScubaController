/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*
*                                       uC/OS ABSTRACTION LAYER
*
* Filename      : bsp_osal.h
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_OSAL_H_
#define  BSP_OSAL_H_


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <os.h>
#include  <cpu_core.h>


/*
*********************************************************************************************************
*                                     EXTERNAL C LANGUAGE LINKAGE
*
* Note(s) : (1) C++ compilers MUST 'extern'ally declare ALL C function prototypes & variable/object
*               declarations for correct C language linkage.
*********************************************************************************************************
*/

#ifdef __cplusplus
extern  "C" {                                                   /* See Note #1.                                         */
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

#if (OS_VERSION >= 30000u)                                      /* --------------- uC/OS-III DATA TYPES --------------- */
typedef  OS_ERR       BSP_OSAL_ERR;                             /*  - OS ERROR -                                        */

typedef  OS_TASK_PTR  BSP_OSAL_TASK;                            /*  - OS TASK -                                         */
typedef  OS_TCB       BSP_OSAL_TCB;
typedef  CPU_STK      BSP_OSAL_STK;

typedef  OS_MUTEX     BSP_OSAL_MUTEX;                           /*  - MUTEX -                                           */

typedef  OS_SEM       BSP_OSAL_SEM;                             /*  - SEMAPHORES -                                      */
typedef  OS_SEM_CTR   BSP_OSAL_SEM_VAL;
#else                                                           /* ---------------- uC/OS-II DATA TYPES --------------- */
typedef  CPU_INT08U   BSP_OSAL_ERR;                             /*  - OS ERROR -                                        */

typedef  void        *BSP_OSAL_TASK;                            /*  - OS TASK -                                         */
typedef  CPU_INT08U   BSP_OSAL_TCB;                             /*      Note: TCBs don't exist w/ OS2, this is not used.*/
typedef  OS_STK       BSP_OSAL_STK;

typedef  OS_EVENT    *BSP_OSAL_MUTEX;                           /*  - MUTEX -                                           */

typedef  OS_EVENT    *BSP_OSAL_SEM;                             /*  - SEMAPHORES -                                      */
typedef  CPU_INT16U   BSP_OSAL_SEM_VAL;
#endif


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/
                                                                /* ------------- OS TASK CREATE FUNCTIONS ------------- */
CPU_BOOLEAN  BSP_OSAL_TaskCreate  (BSP_OSAL_TCB    *p_os_tcb,
                                   BSP_OSAL_STK    *p_task_stk,
                                   BSP_OSAL_TASK    p_task,
                                   CPU_CHAR        *p_task_name,
                                   CPU_INT32U       task_prio,
                                   CPU_INT32U       task_stk_size);

                                                                /* ---------------- OS MUTEX FUNCTIONS ---------------- */
CPU_BOOLEAN  BSP_OSAL_MutexCreate (BSP_OSAL_MUTEX  *p_mutex,
                                   CPU_INT08U       mutex_prio,
                                   CPU_CHAR        *p_mutex_name);

CPU_BOOLEAN  BSP_OSAL_MutexPend   (BSP_OSAL_MUTEX  *p_mutex,
                                   CPU_INT32U       dly_ms);

CPU_BOOLEAN  BSP_OSAL_MutexPost   (BSP_OSAL_MUTEX  *p_mutex);

                                                                /* -------------- OS SEMAPHORE FUNCTIONS -------------- */
CPU_BOOLEAN  BSP_OSAL_SemCreate   (BSP_OSAL_SEM      *p_sem,
                                   BSP_OSAL_SEM_VAL   sem_val,
                                   CPU_CHAR          *p_sem_name);

CPU_BOOLEAN  BSP_OSAL_SemPend     (BSP_OSAL_SEM      *p_sem,
                                   CPU_INT32U         dly_ms);

CPU_BOOLEAN  BSP_OSAL_SemPost     (BSP_OSAL_SEM      *p_sem);

                                                                /* --------------- OS TIME DLY FUNCTIONS -------------- */
void         BSP_OSAL_TimeDlyMs   (CPU_INT32U         dly_ms);

void         BSP_OSAL_TimeDly     (CPU_INT32U         dly_tick);


/*
*********************************************************************************************************
*                                   EXTERNAL C LANGUAGE LINKAGE END
*********************************************************************************************************
*/

#ifdef __cplusplus
}                                                               /* End of 'extern'al C lang linkage.                    */
#endif


/*
*********************************************************************************************************
*                                          CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
