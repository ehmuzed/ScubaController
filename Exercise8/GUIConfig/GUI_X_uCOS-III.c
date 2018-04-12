/*
*********************************************************************************************************
*                                             uC/GUI V5.34
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2014, Micrium Inc., Weston, FL
*                       (c) Copyright 2014, SEGGER Microcontroller GmbH & Co. KG
*
*               All rights reserved. Protected by international copyright laws.
*
*               uC/GUI is protected by international copyright laws. Knowledge of the
*               source code may not be used to write a similar product. This file may
*               only be used in accordance with a license and should not be redistributed
*               in any way. We appreciate your understanding and fairness.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    uC/GUI OPERATING SYSTEM LAYER
*
*                                          Micrium uC/OS-III
*
* Filename      : GUI_X_uCOS-III.c
* Programmer(s) : SL
*                 OD
*********************************************************************************************************
* Note(s)       : (1) Assumes uC/OS-III V3.01.0 (or more recent version) is included in the project build.
*
*                 (2) REQUIREs the following uC/OS-III features to be ENABLED :
*
*                         --------- FEATURE ----------    ---- MINIMUM CONFIGURATION FOR GUI/OS PORT ----
*
*                     (a) Semaphores
*                         (1) OS_CFG_SEM_EN               Enabled
*
*                     (b) Queues
*                         (1) OS_CFG_Q_EN                 Enabled
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <os.h>                                                /* See this 'GUI_X_uCOS-III.c  Note #1'.                */

#include  "GUI_Private.h"

//#include  "stdio.h"


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

                                                                            /* --------------- SIGNALS ---------------- */
static  OS_SEM       GUI_DispSignalObj;
static  OS_SEM       GUI_KeySignalObj;

                                                                            /* ---------------- QUEUES ---------------- */
static  OS_Q         GUI_EventMsgObj;


static  CPU_INT16U   KeyPressed;
static  CPU_BOOLEAN  KeyIsInited;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                /* See this 'GUI_X_uCOS-III.c  Note #1'.                */
#if     (OS_VERSION < 3010u)
#error  "OS_VERSION     [SHOULD be >= V3.01.0]"
#endif



                                                                /* See this 'GUI_X_uCOS-III.c  Note #2a'.               */
#if     (OS_CFG_SEM_EN < 1u)
#error  "OS_CFG_SEM_EN  illegally #define'd in 'os_cfg.h'                  "
#error  "               [MUST be  > 0, (see 'GUI_X_uCOS-III.c  Note #2a1')]"
#endif



                                                                /* See this 'GUI_X_uCOS-III.c  Note #2b'.               */
#if     (OS_CFG_Q_EN < 1u)
#error  "OS_CFG_Q_EN    illegally #define'd in 'os_cfg.h'                  "
#error  "               [MUST be  > 0, (see 'GUI_X_uCOS-III.c  Note #2b1')]"
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          GUI FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           GUI_X_GetTime()
*
* Description : Get the current time value.
*
* Argument(s) : none.
*
* Return(s)   : Number of clock ticks elapsed since OS startup.
*
* Caller(s)   : GUI_GetTime().
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  GUI_X_GetTime (void)
{
    OS_TICK  time_cur;
    OS_ERR   os_err;


    time_cur = 0u;

    time_cur = OSTimeGet(&os_err);
   (void)&os_err;

    return ((int)time_cur);
}


/*
*********************************************************************************************************
*                                           GUI_X_Delay()
*
* Description : Delay for specified time.
*
* Argument(s) : time_dly_ms     Time delay value, in milliseconds.
*
* Return(s)   : none.
*
* Caller(s)   : GUI_Delay(),
*               GUI_X_ExecIdle().
*
*               This function is an INTERNAL GUI function & SHOULD NOT be called by application
*               function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  GUI_X_Delay (int time_dly_ms)
{
    OS_TICK  dly_ticks;
    OS_ERR   os_err;


                                                                /* Calc dly in ticks.                                   */
    dly_ticks = (time_dly_ms * OSCfg_TickRate_Hz) / DEF_TIME_NBR_mS_PER_SEC;
    OSTimeDly(dly_ticks,
              OS_OPT_TIME_DLY,
             &os_err);
   (void)&os_err;
}


/*
*********************************************************************************************************
*                                          GUI_X_ExecIdle()
*
* Description : Delay for 1 millisecond.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : GUI_WaitEvent().
*
*               This function is an INTERNAL GUI function & SHOULD NOT be called by application
*               function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void GUI_X_ExecIdle (void)
{
    GUI_X_Delay(1);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                    MULTITASKING INTERFACE FUNCTIONS
*
* Note(s)       : (1) The following routines are required only if uC/GUI is used in a true multi task
*                     environment, which means you have more than one thread using the uC/GUI API.  In
*                     this case, the following statement needs to be in GUIConf.h :
*
*                     #define GUI_OS 1
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           GUI_X_InitOS()
*
* Description : (1) Perform GUI/OS initialization :
*
*                   (a) Implement uC/GUI initialization signal by creating a counting semaphore.
*
*                   (b) Implement a event message queue.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : GUITASK_Init.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  GUI_X_InitOS (void)
{
    OS_ERR  os_err;


                                                                /* ----------------- INIT GUI SIGNAL ----------------- */
    OSSemCreate(&GUI_DispSignalObj,                             /* Create GUI initialization signal ...                */
                "GUI Init Signal",
                 1u,                                            /* ... with a single signal by dflt.                   */
                &os_err);

    OSQCreate(&GUI_EventMsgObj,                                 /* Create GUI event msg q ...                          */
              "GUI Event Message",
               1u,                                              /* ... with a single msg by dflt.                      */
              &os_err);
   (void)&os_err;
}


/*
*********************************************************************************************************
*                                           GUI_X_Lock()
*
* Description : Acquire mutually exclusive access to the GUI.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : GUI_Lock().
*
* Note(s)     : (1) GUI signal MUST be acquired--i.e. MUST wait for access; do NOT timeout.
*
*                   (a) GUI access MUST be acquired exclusively by only a single task at any one time.
*
*                   (b) Failure to acquire signal will prevent GUI task(s) from running.
*********************************************************************************************************
*/

void  GUI_X_Lock (void)
{
    OS_ERR  os_err;


    OSSemPend(         &GUI_DispSignalObj,                      /* Acquire exclusive GUI access (see Note #1a) ...      */
                        0u,                                     /* ... without timeout.                                 */
                        OS_OPT_PEND_BLOCKING,
              (CPU_TS *)0,
                       &os_err);
   (void)&os_err;                                               /* See Note #1b.                                        */
}


/*
*********************************************************************************************************
*                                           GUI_X_Unlock()
*
* Description : Release mutually exclusive access to GUI.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : GUI_Unlock().
*
* Note(s)     : (1) GUI access MUST be released--i.e. MUST unlock access without failure.
*
*                   (a) Failure to release GUI access will prevent GUI task(s)/operation(s) from
*                       functioning.  Thus, GUI access is assumed to be successfully released since
*                       NO uC/OS-III error handling could be performed to counteract failure.
*********************************************************************************************************
*/
void  GUI_X_Unlock (void)
{
    OS_ERR  os_err;


    OSSemPost(&GUI_DispSignalObj,                               /* Release exclusive GUI access.                        */
               OS_OPT_POST_1,
              &os_err);
   (void)&os_err;                                               /* See Note #1a.                                        */
}


/*
*********************************************************************************************************
*                                            GUI_X_GetTaskId()
*
* Description : Release mutually exclusive access to GUI.
*
* Argument(s) : none.
*
* Return(s)   : Task id.
*
* Caller(s)   : GUI_Lock().
*
* Note(s)     : none.
*********************************************************************************************************
*/

U32  GUI_X_GetTaskId (void)
{
    CPU_INT16U  id;


    id = (CPU_INT16U)OSTCBCurPtr->Prio;                         /* Set task prio as task id.                            */
    return ((U32)id);
}


/*
*********************************************************************************************************
*                                           GUI_X_WaitEvent()
*
* Description : Wait for an event on the GUI event message queue.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) If timeouts NOT desired, wait on GUI message queue until signaled (i.e. do NOT exit).
*********************************************************************************************************
*/

void GUI_X_WaitEvent (void)
{
    OS_MSG_SIZE  os_msg_size;
    OS_ERR       os_err;


    OSQPend(         &GUI_EventMsgObj,                          /* Wait on GUI msg q ...                                */
                      0u,                                       /* ... preferably without timeout (see Note #1).        */
                      OS_OPT_PEND_BLOCKING,
                     &os_msg_size,
            (CPU_TS *)0,
                     &os_err);
   (void)&os_msg_size;                                          /* Msg size ignored.                                    */
   (void)&os_err;
}


/*
*********************************************************************************************************
*                                           GUI_X_SignalEvent()
*
* Description : Signal an event on the GUI event message queue.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void GUI_X_SignalEvent (void)
{
    OS_MSG_SIZE  os_msg_size;
    OS_ERR       os_err;


    os_msg_size = 1u;
    OSQPost(       &GUI_EventMsgObj,                            /* Post event on the GUI event msg q.                   */
            (void *)1,
                    os_msg_size,
                    OS_OPT_POST_LIFO,
                   &os_err);
   (void)&os_err;
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      KEYBOARD INTERFACE FUNCTIONS
*
* Note(s)       : (1) The keyboard routines are required only by some widgets. If widgets are not used,
*                     they may be eliminated.
*
*                 (2) If uC/OS-III is used, characters typed into the log window will be placed in the
*                     keyboard buffer. This is a neat feature which allows you to operate your target
*                     system without having to use or even to have a keyboard connected to it.
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           CheckInit()
*
* Description : Initialize the GUI keyboard if it is not already done.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : GUI_X_WaitKey().
*               GUI_X_GetKey().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  CheckInit (void)
{
    if (KeyIsInited == DEF_FALSE) {
        KeyIsInited =  DEF_TRUE;
        GUI_X_Init();
    }
}


/*
*********************************************************************************************************
*                                           GUI_X_Init()
*
* Description : (1) Perform keyboard initialization :
*
*                   (a) Implement keyboard initialization signal by creating a counting semaphore.
*
*                       (1) Initialize keyboard initialization signal with no signal by setting the
*                           semaphore count to 0 to block the semaphore.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CheckInit().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void GUI_X_Init (void)
{
    OS_ERR  os_err;


                                                                /* ---------------- INITI KBD SIGNAL ----------------- */
    OSSemCreate(&GUI_KeySignalObj,                              /* Create kbd initialization signal ...                */
                "GUI Key Signal",
                 0u,                                            /* ... with NO tasks signaled (see Note #1a1).         */
                &os_err);
   (void)&os_err;
}


/*
*********************************************************************************************************
*                                           GUI_X_GetKey()
*
* Description : Get the pressed key.
*
* Argument(s) : none.
*
* Return(s)   : Pressed key.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  GUI_X_GetKey (void)
{
    int  rtn_key;


    rtn_key = KeyPressed;
    CheckInit();
    KeyPressed = 0;
    return (rtn_key);
}


/*
*********************************************************************************************************
*                                           GUI_X_WaitKey()
*
* Description : Wait for a key to be pressed and return it.
*
* Argument(s) : none.
*
* Return(s)   : Pressed key.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/
int  GUI_X_WaitKey (void)
{
    int     rtn_key;
    OS_ERR  os_err;


    CheckInit();
    if (KeyPressed == 0) {
    OSSemPend(         &GUI_KeySignalObj,
                        0u,
                        OS_OPT_PEND_BLOCKING,
              (CPU_TS *)0,
                       &os_err);
    }

    rtn_key    = KeyPressed;
    KeyPressed = 0u;
    return (rtn_key);
}


/*
*********************************************************************************************************
*                                          GUI_X_StoreKey()
*
* Description : Store the pressed key.
*
* Argument(s) : Pressed key.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  GUI_X_StoreKey (int k)
{
    OS_ERR  os_err;


    KeyPressed = k;
    OSSemPost(&GUI_KeySignalObj,
               OS_OPT_POST_1,
              &os_err);
   (void)&os_err;
}


/*
*********************************************************************************************************
*                                                Logging
* Note(s)     : Logging is used in higher debug levels only. The typical target
*               build does not use logging and does therefore not require any of
*               the logging routines below. For a release build without logging
*               the routines below may be eliminated to save some space.
*               (If the linker is not function aware and eliminates unreferenced
*               functions automatically)
*********************************************************************************************************
*/

void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }
