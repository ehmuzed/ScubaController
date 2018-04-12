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
* Filename      : bsp_osal.c
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  "bsp_osal.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
**                                         OS TASK FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        BSP_OSAL_MutexCreate()
*
* Description : Creates a Task with either uC/OS-II or uC/OS-III
*
* Argument(s) : p_os_tcb        Pointer to the OS_TCB Block
*
*               p_task_stk      Pointer the Task Stack Buffer
*
*               p_task          Pointer to the Task (Function that defines the Task)
*
*               p_task_name     Pointer to the Task Name.
*
*               task_prio       Priority of the created Task
*
*               task_stk_size   Stack Size of the created Task
*
* Return(s)   : DEF_OK      If the OS Task was Created.
*               DEF_FAIL    If the OS Task could not be Created.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) The OS TCB Block is only used with uC/OS-III. For uC/OS-II, this argument is not
*                   used.
*
*               (2) For uC/OS-II the Task Priority MUST be UNIQUE.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_OSAL_TaskCreate  (BSP_OSAL_TCB    *p_os_tcb,
                                   BSP_OSAL_STK    *p_task_stk,
                                   BSP_OSAL_TASK    p_task,
                                   CPU_CHAR        *p_task_name,
                                   CPU_INT32U       task_prio,
                                   CPU_INT32U       task_stk_size)
{
    BSP_OSAL_ERR  os_err;
    
    
#if (OS_VERSION >= 30000u)                                      /* --------------- uC/OS-III TASK CREATE -------------- */
    
    OSTaskCreate( p_os_tcb,
                  p_task_name,
                  p_task,
                  0u,
                  task_prio,
                  p_task_stk,
                 (task_stk_size / 10u),
                  task_stk_size,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

#else                                                           /* --------------- uC/OS-II TASK CREATE --------------- */
    INT8U  tsk_err;
    
    
    tsk_err = OSTaskCreateExt( p_task,
                               0u,
                              &p_task_stk[task_stk_size - 1u],
                               task_prio,
                               task_prio,
                               p_task_stk,
                               0u,
                              (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    
    if tsk_err != OS_ERR_NONE) {
        return (DEF_FAIL);
    }
    
    #if (OS_TASK_NAME_EN > 0u)
    OSTaskNameSet(         task_prio,
                  (INT8U *)p_task_name,
                          &os_err);
    #else
    os_err == OS_ERR_NONE;
    #endif
#endif
    
    if (os_err != OS_ERR_NONE) {
        return (DEF_FAIL);
    }
    
    return (DEF_OK);
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         OS MUTEX FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        BSP_OSAL_MutexCreate()
*
* Description : Creates a Mutex to protect a shared resource for either uC/OS-II or uC/OS-III
*
* Argument(s) : p_sem       Pointer to a BSP_OSAL_MUTEX Structure
*
*               sem_val     Initial value of the Semaphore.
*
*               p_sem_name  Pointer to the Semaphore Name.
*
* Return(s)   : DEF_OK      If the Mutex was Created.
*               DEF_FAIL    If the Mutex could not be Created.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_OSAL_MutexCreate (BSP_OSAL_MUTEX  *p_mutex,
                                   CPU_INT08U       mutex_prio,
                                   CPU_CHAR        *p_mutex_name)
{
    BSP_OSAL_ERR   os_err;
#if (OS_VERSION < 30000u)
    OS_EVENT      *p_event;
#endif
    
    
#if (OS_VERSION >= 30000u)                                      /* -------------- uC/OS-III MUTEX CREATE -------------- */
    (void)&mutex_prio;                                          /* Unused Argument, to prevent warning.                 */
    
    OSMutexCreate( p_mutex,
                   p_mutex_name,
                  &os_err);
    
    if (os_err != OS_ERR_NONE) {
        return (DEF_FAIL);
    }
    
#else                                                           /* --------------- uC/OS-II MUTEX CREATE -------------- */
    p_event = OSMutexCreate( mutex_prio,
                            &os_err);

    if (p_event == (OS_EVENT *)0) {
        return (DEF_FAIL);
    }

   *p_sem = (BSP_OSAL_MUTEX)(p_event);
    
    #if (OS_EVENT_NAME_EN > 0u)
    OSEventNameSet(         p_event,
                   (INT8U *)p_mutex_name,
                           &os_err);
    #endif
#endif
    
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                         BSP_OSAL_MutexPend()
*
* Description : Wait on a Mutex to become available for uC/OS-II or uC/OS-III.
*
* Argument(s) : p_mutex     Mutex Handler
*
*               dly_ms      Delay in Miliseconds to wait on the mutex
*
* Return(s)   : DEF_OK      If the Mutex was Acquire
*               DEF_FAIL    If the Mutex could not be Acquire
*
* Caller(s)   : Application.
*
* Note(s)     : (1) A Delay of '0' ms, signifies an infinite time delay (AKA Blocking Mutex).
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_OSAL_MutexPend   (BSP_OSAL_MUTEX  *p_mutex,
                                   CPU_INT32U       dly_ms)
{
    BSP_OSAL_ERR  os_err;
    CPU_INT32U    dly_ticks;
    
    
#if (OS_VERSION >= 30000u)                                      /* --------------- uC/OS-III MUTEX PEND --------------- */
    dly_ticks  = ((dly_ms * DEF_TIME_NBR_mS_PER_SEC) / OSCfg_TickRate_Hz);

    OSMutexPend( p_mutex,
                 dly_ticks,
                 OS_OPT_PEND_BLOCKING,
                 0u,
                &os_err);

#else                                                           /* ---------------- uC/OS-II MUTEX PEND --------------- */
    dly_ticks  = ((dly_ms * DEF_TIME_NBR_mS_PER_SEC) / OS_TICKS_PER_SEC);

    OSMutexPend(*p_mutex,
                 dly_ticks,
                &os_err);
#endif
    
    if (os_err != OS_ERR_NONE) {
       return (DEF_FAIL);
    }
    
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                         BSP_OSAL_MutexPost()
*
* Description : Post to a Mutex for uC/OS-II or uC/OS-III.
*
* Argument(s) : p_mutex     Mutex Handler
*
* Return(s)   : DEF_OK      If the Mutex was Posted.
*               DEF_FAIL    If the Mutex could not be Posted.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_OSAL_MutexPost   (BSP_OSAL_MUTEX  *p_mutex)
{
    BSP_OSAL_ERR  os_err;
    
    
#if (OS_VERSION >= 30000u)                                      /* --------------- uC/OS-III MUTEX POST --------------- */
    OSMutexPost( p_mutex,
                 OS_OPT_POST_NONE,
                &os_err);
#else                                                           /* ---------------- uC/OS-II MUTEX POST --------------- */
    os_err = OSMutexPost(*p_mutex);
#endif
    
    if (os_err != OS_ERR_NONE) {
       return (DEF_FAIL);
    }
    
    return (DEF_OK);
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                       OS SEMAPHORE FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BSP_OSAL_SemCreate()
*
* Description : Wait on a Semaphore to become available for uC/OS-II or uC/OS-III.
*
* Argument(s) : p_sem       Pointer to a BSP_OSAL_SEM Structure
*
*               sem_val     Initial value of the Semaphore.
*
*               p_sem_name  Pointer to the Semaphore Name.
*
* Return(s)   : DEF_OK      If the Semaphore was Created.
*               DEF_FAIL    If the Sempahore could not be Created.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_OSAL_SemCreate (BSP_OSAL_SEM      *p_sem,
                                 BSP_OSAL_SEM_VAL   sem_val,
                                 CPU_CHAR          *p_sem_name)
{
#if (OS_VERSION >= 30000u)
    BSP_OSAL_ERR  os_err;
#else
    OS_EVENT  *p_event;
    #if (OS_EVENT_NAME_EN > 0u)
    BSP_OSAL_ERR  os_err;
    #endif
#endif
    

#if (OS_VERSION >= 30000u)                                      /* ------------ uC/OS-III SEMAPHORE CREATE ------------ */
    OSSemCreate( p_sem,
                 p_sem_name,
                 sem_val,
                &os_err);

    if (os_err != OS_ERR_NONE) {
        return (DEF_FAIL);
    }
#else                                                           /* ------------- uC/OS-II SEMAPHORE CREATE ------------ */
    p_event = OSSemCreate(sem_val);

    if (p_event == (OS_EVENT *)0) {
        return (DEF_FAIL);
    }

   *p_sem = (BSP_OSAL_SEM)(p_event);

    #if (OS_EVENT_NAME_EN > 0u)
    OSEventNameSet(         p_event,
                   (INT8U *)p_sem_name,
                           &os_err);
    #endif
#endif

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                          BSP_OSAL_SemPend()
*
* Description : Wait on a Semaphore to become available for uC/OS-II or uC/OS-III.
*
* Argument(s) : p_sem       Sempahore Handler
*
*               dly_ms      Delay in Miliseconds to wait on the semaphore
*
* Return(s)   : DEF_OK      If the Semaphore was Acquire
*               DEF_FAIL    If the Sempahore could not be Acquire
*
* Caller(s)   : Application.
*
* Note(s)     : (1) A Delay of '0' ms, signifies an infinite time delay (AKA Blocking Semaphore).
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_OSAL_SemPend (BSP_OSAL_SEM  *p_sem,
                               CPU_INT32U     dly_ms)
{
    CPU_INT32U    dly_ticks;
    BSP_OSAL_ERR  os_err;
    

#if (OS_VERSION >= 30000u)                                      /* ------------- uC/OS-III SEMAPHORE PEND ------------- */
    dly_ticks  = ((dly_ms * DEF_TIME_NBR_mS_PER_SEC) / OSCfg_TickRate_Hz);

    OSSemPend( p_sem,
               dly_ticks,
               OS_OPT_PEND_BLOCKING,
               0u,
              &os_err);
#else                                                           /* -------------- uC/OS-II SEMAPHORE PEND ------------- */
    dly_ticks  = ((dly_ms * DEF_TIME_NBR_mS_PER_SEC) / OS_TICKS_PER_SEC);

    OSSemPend(*p_sem,
               dly_ticks,
              &os_err);

#endif
    
    if (os_err != OS_ERR_NONE) {
       return (DEF_FAIL);
    }
    
    return (DEF_OK);
}

/*
*********************************************************************************************************
*                                          BSP_OSAL_SemPost()
*
* Description : Post to a Semaphore for either uC/OS-II or uC/OS-III.
*
* Argument(s) : p_sem       Semaphore Handler
*
* Return(s)   : DEF_OK      If the Semaphore was Posted.
*               DEF_FAIL    If the Sempahore could not be Posted.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_OSAL_SemPost (BSP_OSAL_SEM  *p_sem)
{
    BSP_OSAL_ERR  os_err;
    

#if (OS_VERSION >= 30000u)                                      /* ------------- uC/OS-III SEMAPHORE POST ------------- */
    OSSemPost( p_sem,
               OS_OPT_POST_1,
              &os_err);
#else                                                           /* -------------- uC/OS-II SEMAPHORE POST ------------- */
    os_err = OSSemPost(*p_sem);
#endif

    if (os_err != OS_ERR_NONE) {
        return (DEF_FAIL);
    }

    return (DEF_OK);
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                       OS TIME DLY FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         BSP_OSAL_TimeDlyMs()
*
* Description : Delay the exceution of a Task or Object for a Specific amount of milliseconds.
*
* Argument(s) : dly_ms      Delay in milliseconds
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void   BSP_OSAL_TimeDlyMs (CPU_INT32U  dly_ms)
{
    CPU_INT16U    os_ms;
    CPU_INT16U    os_sec;
    BSP_OSAL_ERR  os_err;


    if (dly_ms > 10000u) {                                       /* Limit Delays to 10 Seconds.                        */
        dly_ms = 10000u;
    }

    if (dly_ms >= 1000u) {
        os_sec = dly_ms / 1000u;
        os_ms  = dly_ms % 1000u;
    } else {
        os_sec = 0u;
        os_ms  = dly_ms;
    }

#if (OS_VERSION >= 30000u)                                      /* -------------- uC/OS-III TIME DLY (MS) ------------- */
    OSTimeDlyHMSM(0u, 0u, os_sec, os_ms,
                  OS_OPT_TIME_HMSM_STRICT, &os_err);
#else                                                           /* -------------- uC/OS-II TIME DLY (MS) -------------- */
    (void)&os_err;                                              /* Unused Argument, to prevent warning.                 */
        
    OSTimeDlyHMSM(0u, 0u, os_sec, os_ms);
#endif
    
}


/*
*********************************************************************************************************
*                                          BSP_OSAL_TimeDly()
*
* Description : Delay the exceution of a Task or Object for a Specific amount of CLK Ticks.
*
* Argument(s) : dly_tick    Delay in CLK Ticks
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void   BSP_OSAL_TimeDly (CPU_INT32U  dly_tick)
{
    BSP_OSAL_ERR  os_err;
    
    
#if (OS_VERSION >= 30000u)                                      /* ---------------- uC/OS-III TIME DLY ---------------- */
    OSTimeDly( dly_tick,
               OS_OPT_TIME_DLY,
              &os_err);
#else                                                           /* ----------------- uC/OS-II TIME DLY ---------------- */
    OSTimeDly(dly_tick);
    
   (void)&os_err;                                               /* Unused Argument, to prevent warning.                 */
#endif
}
