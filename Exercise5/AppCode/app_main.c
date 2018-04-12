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
*                                            EXAMPLE CODE
*
*                                         STM32F746G-DISCO
*                                         Evaluation Board
*
* Filename      : app_main.c
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  "stdarg.h"
#include  "stdio.h"
#include  "stm32f7xx_hal.h"

#include  "cpu.h"
#include  "lib_math.h"
#include  "lib_mem.h"
#include  "os.h"
#include  "os_app_hooks.h"

#include  "app_cfg.h"
#include  "bsp.h"
#include  "bsp_led.h"
#include  "bsp_clock.h"
#include  "bsp_pb.h"
#include  "bsp_test.h"
#include  "GUI.h"
#include  "GUIDEMO_API.h"
#include  "pushbutton.h"
#include  "adc.h"
#include  "alarm.h"

// ------------- led TASK PRIO & STK SIZE -------------
#define  APP_CFG_TASK_LED_PRIO                (14u)
#define  APP_CFG_TASK_LED_STK_SIZE            (192u)

// *****************************************************************
// Define storage for each Task Control Block (TCB) and task stacks
// *****************************************************************
static  OS_TCB   AppTaskGUI_TCB;
static  OS_TCB   AppTaskADC_TCB;
static  OS_TCB   AppTaskSTART_TCB;
static  OS_TCB   AppTaskAlarm_TCB;
static  OS_TCB   AppTaskLED1_TCB;
static  OS_TCB   AppTaskLED2_TCB;
static  OS_TCB   AppTaskSW1_TCB;
static  OS_TCB   AppTaskSW2_TCB;
static  OS_TCB   AppTaskDebounce_TCB;
static  CPU_STK  AppTaskGUI_Stk[APP_CFG_TASK_GUI_STK_SIZE];
static  CPU_STK  AppTaskADC_Stk[APP_CFG_TASK_LED_STK_SIZE];
static  CPU_STK  AppTaskStartup_Stk[APP_CFG_TASK_GUI_STK_SIZE];
static  CPU_STK  AppTaskLed1_Stk[APP_CFG_TASK_LED_STK_SIZE];
static  CPU_STK  AppTaskLed2_Stk[APP_CFG_TASK_LED_STK_SIZE];
static  CPU_STK  AppTaskSW1_Stk[APP_CFG_TASK_LED_STK_SIZE];
static  CPU_STK  AppTaskSW2_Stk[APP_CFG_TASK_LED_STK_SIZE];
static  CPU_STK  AppTaskDebounce_Stk[APP_CFG_TASK_LED_STK_SIZE];
static  CPU_STK  AppTaskAlarm_Stk[APP_CFG_TASK_LED_STK_SIZE];
OS_MUTEX g_mutex_led;

typedef struct{
  BOARD_LED_ID id;
  int ms;
} LED_task_t;

typedef struct{
  int id;
  int cnt;
  OS_SEM* p_sem;
} sw_task_t;

// *****************************************************************
// Flash LED
// *****************************************************************
static void led_task(void * p_arg)
{
    OS_ERR  err;
    LED_task_t * _args = ((LED_task_t *) p_arg); 
    // Task main loop
    for (;;)
    {
        OSMutexPend(&g_mutex_led, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
        BSP_LED_Toggle(_args->id);
        OSMutexPost(&g_mutex_led, OS_OPT_POST_NONE, &err);
        OSTimeDlyHMSM(0, 0, 0, _args->ms, 0, &err);        
    }
}
static void sw_task(void * p_arg)
{
    OS_ERR  err;
    char buf[64] = "";
    sw_task_t * _args = ((sw_task_t *) p_arg);
    _args->cnt = 0;
    for (;;)
    {
        OSSemPend(_args->p_sem, 1, OS_OPT_PEND_BLOCKING, 0, &err);
        if(err != OS_ERR_TIMEOUT)
        { 
          _args->cnt++;
          snprintf(buf, 64, "SW%u: %u", _args->id + 1, _args->cnt);
          GUIDEMO_API_writeLine(_args->id, buf);
        }
    }
}
// *****************************************************************
// Startup Task
// *****************************************************************
static void startup_task(void * p_arg)
{
    OS_ERR  err;

    // Initialize BSP
    BSP_Init();
    debounce_task_init();
    OSMutexCreate(&g_mutex_led, "'tects LED", &err);
    OSFlagCreate(&g_alarm_flags, "Alarm Flags", (OS_FLAGS) 0, &err);
  
    LED_task_t t_led1 = {LED1, 250};
    LED_task_t t_led2 = {LED2, 166};
    sw_task_t t_sw1 = {0, 0, &g_sem_sw1};
    sw_task_t t_sw2 = {1, 0, &g_sem_sw2};
#if OS_CFG_STAT_TASK_EN > 0u
    // Compute CPU capacity with no other task running
    OSStatTaskCPUUsageInit(&err);
    my_assert(OS_ERR_NONE == err);
    OSStatReset(&err);
    my_assert(OS_ERR_NONE == err);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    OSTaskCreate(&AppTaskGUI_TCB, "uC/GUI Task", (OS_TASK_PTR ) GUI_DemoTask,
               0, APP_CFG_TASK_GUI_PRIO,
               &AppTaskGUI_Stk[0], (APP_CFG_TASK_GUI_STK_SIZE / 10u),
                APP_CFG_TASK_GUI_STK_SIZE, 0u, 0u, 0,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &err);
    my_assert(OS_ERR_NONE == err);
                
   OSTaskCreate(&AppTaskLED1_TCB, 
                "Blinky1 Task", 
                (OS_TASK_PTR ) led_task,
                &t_led1, 
                14, //Priority
                &AppTaskLed1_Stk[0], 
                (APP_CFG_TASK_LED_STK_SIZE / 10u),
                APP_CFG_TASK_LED_STK_SIZE, 
                0u, 0u, 0,
                (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                &err);
    my_assert(OS_ERR_NONE == err);
    OSTaskCreate(&AppTaskLED2_TCB, 
                 "Blinky2 Task", 
                 (OS_TASK_PTR ) led_task,
                 &t_led2,
                 13, //Priority
                 &AppTaskLed2_Stk[0], 
                 (APP_CFG_TASK_LED_STK_SIZE / 10u),
                 APP_CFG_TASK_LED_STK_SIZE, 
                 0u, 0u, 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                 &err);
        OSTaskCreate(&AppTaskDebounce_TCB, 
                 "SW1 Task", 
                 (OS_TASK_PTR ) debounce_task,
                 NULL,
                 12, //Priority
                 &AppTaskDebounce_Stk[0], 
                 (APP_CFG_TASK_LED_STK_SIZE / 10u),
                 APP_CFG_TASK_LED_STK_SIZE, 
                 0u, 0u, 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                 &err);
    my_assert(OS_ERR_NONE == err);
    OSTaskCreate(&AppTaskSW1_TCB, 
                 "SW1 Task", 
                 (OS_TASK_PTR ) sw_task,
                 &t_sw1,
                 15, //Priority
                 &AppTaskSW1_Stk[0], 
                 (APP_CFG_TASK_LED_STK_SIZE / 10u),
                 APP_CFG_TASK_LED_STK_SIZE, 
                 0u, 0u, 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                 &err);
    my_assert(OS_ERR_NONE == err);
    OSTaskCreate(&AppTaskSW2_TCB, 
                 "SW2 Task", 
                 (OS_TASK_PTR ) sw_task,
                 &t_sw2,
                 16, //Priority
                 &AppTaskSW2_Stk[0], 
                 (APP_CFG_TASK_LED_STK_SIZE / 10u),
                 APP_CFG_TASK_LED_STK_SIZE, 
                 0u, 0u, 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                 &err);
    my_assert(OS_ERR_NONE == err);
    OSTaskCreate(&AppTaskADC_TCB, 
               "ADC Task", 
               (OS_TASK_PTR ) adc_task,
               NULL,
               10, //Priority
               &AppTaskADC_Stk[0], 
               (APP_CFG_TASK_LED_STK_SIZE / 10u),
               APP_CFG_TASK_LED_STK_SIZE, 
               0u, 0u, 0,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
               &err);
    my_assert(OS_ERR_NONE == err);
    OSTaskCreate(&AppTaskAlarm_TCB, 
                "Alarm Task", 
                (OS_TASK_PTR ) alarm_task,
                NULL,
                10, //Priority
                &AppTaskAlarm_Stk[0], 
                (APP_CFG_TASK_LED_STK_SIZE / 10u),
                APP_CFG_TASK_LED_STK_SIZE, 
                0u, 0u, 0,
                (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                &err);
   my_assert(OS_ERR_NONE == err);
    GUIDEMO_SetColorBG(BG_COLOR_GREEN);
    while(1)
    {
        OSTimeDlyHMSM(1, 0, 0, 0, 0, &err);        
    }
}

// *****************************************************************
xint main(void)
{
    OS_ERR   err;

    HAL_Init();
    BSP_SystemClkCfg();   // Init. system clock frequency to 200MHz
    CPU_Init();           // Initialize the uC/CPU services
    Mem_Init();           // Initialize Memory Managment Module
    Math_Init();          // Initialize Mathematical Module
    CPU_IntDis();         // Disable all Interrupts.

    // TODO: Init uC/OS-III.
    OSInit(&err);
    // Create the GUI task

 //Create Startup task
    OSTaskCreate(&AppTaskSTART_TCB, 
                 "Startup Task", 
                 (OS_TASK_PTR ) startup_task,
                 0, 3, // Args, Priority
                 &AppTaskStartup_Stk[0], (APP_CFG_TASK_GUI_STK_SIZE / 10u),
                 APP_CFG_TASK_GUI_STK_SIZE, 0u, 0u, 0, 
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &err);
    my_assert(OS_ERR_NONE == err);
    
    // TODO: Start multitasking (i.e. give control to uC/OS-III)
    OSStart(&err);
    // Should never get here
    my_assert(0);
}

