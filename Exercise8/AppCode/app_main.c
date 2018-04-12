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
#include  "protected_led.h"
#include  "pushbutton.h"
#include  "adc.h"
#include  "alarm.h"
#include "scuba.h"

// Task priorities and stack sizes
#define TASK_DEBOUNCE_PRIO    (7UL)
#define TASK_SW1_PRIO         (8UL)
#define TASK_ADC_PRIO         (9UL)
#define TASK_ALARM_PRIO      (11UL)
#define TASK_SW2_PRIO        (12UL)
#define TASK_LED1_PRIO       (14UL)
#define TASK_LED2_PRIO       (13UL)
#define TASK_DISPLAY_PRIO       (15UL)
#define TASK_EDT_PRIO       (15UL)

#define TASK_DEBOUNCE_STK_SIZE    (192UL)
#define TASK_SW1_STK_SIZE         (192UL)
#define TASK_ADC_STK_SIZE         (192UL)
#define TASK_ALARM_STK_SIZE       (192UL)
#define TASK_SW2_STK_SIZE         (192UL)
#define TASK_LED_STK_SIZE         (192UL)
#define TASK_DISPLAY_STK_SIZE         (192UL)
#define TASK_EDT_STK_SIZE         (192UL)


static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTaskGUI_TCB;
static  CPU_STK  AppTaskGUI_Stk[APP_CFG_TASK_GUI_STK_SIZE];

static  OS_TCB   TaskLED1_TCB;
static  CPU_STK  TaskLED1_Stk[TASK_LED_STK_SIZE];

static  OS_TCB   TaskLED2_TCB;
static  CPU_STK  TaskLED2_Stk[TASK_LED_STK_SIZE];

static  OS_TCB   TaskDebounce_TCB;
static  CPU_STK  TaskDebounce_Stk[TASK_DEBOUNCE_STK_SIZE];

static  OS_TCB   TaskSW1_TCB;
static  CPU_STK  TaskSW1_Stk[TASK_SW1_STK_SIZE];

static  OS_TCB   TaskSW2_TCB;
static  CPU_STK  TaskSW2_Stk[TASK_SW2_STK_SIZE];

static  OS_TCB   TaskADC_TCB;
static  CPU_STK  TaskADC_Stk[TASK_ADC_STK_SIZE];

static  OS_TCB   TaskAlarm_TCB;
static  CPU_STK  TaskAlarm_Stk[TASK_ALARM_STK_SIZE];

static  OS_TCB   TaskDisplay_TCB;
static  CPU_STK  TaskDisplay_Stk[TASK_DISPLAY_STK_SIZE];

static  OS_TCB   TaskEdt_TCB;
static  CPU_STK  TaskEdt_Stk[TASK_EDT_STK_SIZE];

// Flag group for alarms
OS_FLAG_GRP     g_alarm_flags;
OS_FLAG_GRP     g_data_dirty;

OS_MUTEX        g_mutex_scuba_data;
OS_TMR          g_edt_timer;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart (void  *p_arg);
static  void  AppTaskCreate(void);
static  void  AppObjCreate (void);


// *****************************************************************
int main(void)
{
    OS_ERR   err;


    HAL_Init();
    BSP_SystemClkCfg();   // Init. system clock frequency to 200MHz
    CPU_Init();           // Initialize the uC/CPU services
    Mem_Init();           // Initialize Memory Managment Module
    Math_Init();          // Initialize Mathematical Module
    CPU_IntDis();         // Disable all Interrupts.

    // Init uC/OS-III.
    OSInit(&err);
    my_assert(err == OS_ERR_NONE);

    // Create the start task
    OSTaskCreate(&AppTaskStartTCB,
                  "App Task Start",
                  AppTaskStart,
                  0u,
                  APP_CFG_TASK_START_PRIO,
                 &AppTaskStartStk[0u],
                  (APP_CFG_TASK_START_STK_SIZE / 10u),
                  APP_CFG_TASK_START_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &err);
    my_assert(err == OS_ERR_NONE);
 
    // Start multitasking (i.e. give control to uC/OS-III)
    OSStart(&err);
    my_assert(err == OS_ERR_NONE);

    // Should never get here!
    while (DEF_ON) {
        ;
    }
}


// *****************************************************************
static  void  AppTaskStart (void *p_arg)
{
    OS_ERR  err;

    (void)p_arg;   // Suppress compiler warning about unused parameter

    // Set up the Board Support Package
    BSP_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    // Compute CPU capacity with no other task running
    OSStatTaskCPUUsageInit(&err);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    // Create Application kernel objects
    AppObjCreate();

    // Create Application tasks
    AppTaskCreate();

    // Simply return.
    // ** WITH THIS RTOS **, tasks that return to their maker
    // are cleanly killed.  This behavior is RTOS-specific.
}

// *****************************************************************
static void LedBlinkTask(void * p_arg)
{
    OS_ERR  err;

    BOARD_LED_ID my_led = (BOARD_LED_ID)(((uint32_t)p_arg) & 0xFFFFUL);
    uint32_t  dly_in_ms = (((uint32_t)p_arg) >> 16UL);

    for (;;)
    {
        // Flash LED at requested frequency.
        protectedLED_Toggle(my_led);
        OSTimeDlyHMSM(0, 0, 0, dly_in_ms, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

/*!
* @brief Button SW1 Catcher Task
*/
void sw1_task(void * p_arg)
{
  uint16_t    sw1_counter = 0;
  OS_ERR      err;

  (void)p_arg;    // NOTE: Silence compiler warning about unused param.

  for (;;)
  {
    // Display the switch count
    //sprintf(p_str, "SW1: % 4u", sw1_counter);
    //GUIDEMO_API_writeLine(0, p_str);

    // Wait for a signal from the button debouncer.
    OSSemPend(&g_sw1_sem, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    OSMutexPend(&g_mutex_scuba_data, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    my_assert(OS_ERR_NONE == err);
    g_scuba_data.b_is_metric = g_scuba_data.b_is_metric == 0 ? 1 : 0;
    OSMutexPost(&g_mutex_scuba_data, OS_OPT_POST_NONE, &err);
    my_assert(OS_ERR_NONE == err);
    OSFlagPost(&g_data_dirty, DATA_DIRTY, OS_OPT_POST_FLAG_SET, &err);
    my_assert(OS_ERR_NONE == err);
    // Check for errors.
    my_assert(OS_ERR_NONE == err);
        
    // Increment button press counter.
    sw1_counter++;
  }
}

/*!
* @brief Button SW2 Catcher Task
*/
void sw2_task(void * p_arg)
{
  uint16_t    sw2_counter = 0;
  OS_ERR      err;

  (void)p_arg;    // NOTE: Silence compiler warning about unused param.

  for (;;)
  {
    // Display the switch count
    //sprintf(p_str, "SW2: % 4u", sw2_counter);
    //GUIDEMO_API_writeLine(1, p_str);

    // Wait for a signal from the button debouncer.
    OSSemPend(&g_sw2_sem, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    OSMutexPend(&g_mutex_scuba_data, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    my_assert(OS_ERR_NONE == err);
    g_scuba_data.air_volume += (g_scuba_data.depth_mm ==0)?2000:0;
    if(g_scuba_data.air_volume > 200000)
    {
      g_scuba_data.air_volume = 200000;
    }
    OSMutexPost(&g_mutex_scuba_data, OS_OPT_POST_NONE, &err);
    my_assert(OS_ERR_NONE == err);
    OSFlagPost(&g_data_dirty, DATA_DIRTY, OS_OPT_POST_FLAG_SET, &err);
    my_assert(OS_ERR_NONE == err);
        
    // Increment button press counter.
    sw2_counter++;
  }
}


// *****************************************************************
static  void  AppTaskCreate (void)
{
    OS_ERR  os_err;

    // *****************************************************************
    // Create the GUI Demo Task
    // *****************************************************************
    OSTaskCreate(&AppTaskGUI_TCB, "uC/GUI Task", (OS_TASK_PTR ) GUI_DemoTask,
                 0, APP_CFG_TASK_GUI_PRIO,
                 &AppTaskGUI_Stk[0], (APP_CFG_TASK_GUI_STK_SIZE / 10u),
                  APP_CFG_TASK_GUI_STK_SIZE, 0u, 0u, 0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    my_assert(OS_ERR_NONE == os_err);

    // *****************************************************************
    // Create the LED1 Blinker task - 1 Hz, LED1
    // *****************************************************************
    OSTaskCreate(&TaskLED1_TCB, "LED1 Task", (OS_TASK_PTR ) LedBlinkTask,
                 (void *)((160UL << 16UL) | LED1), TASK_LED1_PRIO,
                 &TaskLED1_Stk[0], (TASK_LED_STK_SIZE / 10u),
                  TASK_LED_STK_SIZE, 0u, 0u, 0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    my_assert(OS_ERR_NONE == os_err);

    // *****************************************************************
    // Create the LED2 Blinker task - 3 Hz, LED2
    // *****************************************************************
    OSTaskCreate(&TaskLED2_TCB, "LED2 Task", (OS_TASK_PTR ) LedBlinkTask,
                 (void *)((167UL << 16UL) | LED2), TASK_LED2_PRIO,
                 &TaskLED2_Stk[0], (TASK_LED_STK_SIZE / 10u),
                  TASK_LED_STK_SIZE, 0u, 0u, 0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    my_assert(OS_ERR_NONE == os_err);

    // *****************************************************************
    // Create the switch debouncer task
    // *****************************************************************
    OSTaskCreate(&TaskDebounce_TCB, "Debounce Task", (OS_TASK_PTR ) debounce_task,
                 (void *)0, TASK_DEBOUNCE_PRIO,
                 &TaskDebounce_Stk[0], (TASK_DEBOUNCE_STK_SIZE / 10u),
                  TASK_DEBOUNCE_STK_SIZE, 0u, 0u, 0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    my_assert(OS_ERR_NONE == os_err);

    // *****************************************************************
    // Create the "switch 1" (single press) task
    // *****************************************************************
    OSTaskCreate(&TaskSW1_TCB, "SW1 Task", (OS_TASK_PTR ) sw1_task,
                 (void *)0, TASK_SW1_PRIO,
                 &TaskSW1_Stk[0], (TASK_SW1_STK_SIZE / 10u),
                  TASK_SW1_STK_SIZE, 0u, 0u, 0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    my_assert(OS_ERR_NONE == os_err);

    // *****************************************************************
    // Create the "switch 2" (auto-repeat press) task
    // *****************************************************************
    OSTaskCreate(&TaskSW2_TCB, "SW2 Task", (OS_TASK_PTR ) sw2_task,
                 (void *)0, TASK_SW2_PRIO,
                 &TaskSW2_Stk[0], (TASK_SW2_STK_SIZE / 10u),
                  TASK_SW2_STK_SIZE, 0u, 0u, 0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    my_assert(OS_ERR_NONE == os_err);

    // *****************************************************************
    // Create the ADC (potentiometer) task
    // *****************************************************************
    OSTaskCreate(&TaskADC_TCB, "ADC Task", (OS_TASK_PTR ) adc_task,
                 (void *)0, TASK_ADC_PRIO,
                 &TaskADC_Stk[0], (TASK_ADC_STK_SIZE / 10u),
                  TASK_ADC_STK_SIZE, 0u, 0u, 0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    my_assert(OS_ERR_NONE == os_err);

    // *****************************************************************
    // Create the Alarm task                  
    // *****************************************************************
    OSTaskCreate(&TaskAlarm_TCB, "Alarm Task", (OS_TASK_PTR ) alarm_task,
                 (void *)0, TASK_ALARM_PRIO,
                 &TaskAlarm_Stk[0], (TASK_ALARM_STK_SIZE / 10u),
                  TASK_ALARM_STK_SIZE, 0u, 0u, 0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    my_assert(OS_ERR_NONE == os_err);
    
    // *****************************************************************
    // Create the Scuba display task                  
    // *****************************************************************
    OSTaskCreate(&TaskDisplay_TCB, "Display Task", (OS_TASK_PTR ) display_task,
                 (void *)0, TASK_DISPLAY_PRIO,
                 &TaskDisplay_Stk[0], (TASK_DISPLAY_STK_SIZE / 10u),
                  TASK_DISPLAY_STK_SIZE, 0u, 0u, 0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    my_assert(OS_ERR_NONE == os_err);   
    
    // *****************************************************************
    // Create the Scuba display task                  
    // *****************************************************************
    OSTaskCreate(&TaskEdt_TCB, "Main Timer Task", (OS_TASK_PTR ) edt_task,
                 (void *)0, TASK_EDT_PRIO,
                 &TaskEdt_Stk[0], (TASK_EDT_STK_SIZE / 10u),
                  TASK_EDT_STK_SIZE, 0u, 0u, 0,
                  (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    my_assert(OS_ERR_NONE == os_err);

}

// *****************************************************************
static void AppObjCreate(void)
{

    // Create the mutex that protects the LED hardware from race conditions.
    protectedLED_Init();

    // Create the OS objects used by the debounce task to signal button events
    debounce_task_init();

    // Create the Alarm Flags Group
    OS_ERR  err;
    OSFlagCreate(&g_alarm_flags, "Alarm Flags", 0, &err);
    my_assert(OS_ERR_NONE == err);
    OSFlagCreate(&g_data_dirty, "Dirty Flag", 0, &err);
    my_assert(OS_ERR_NONE == err);
    OSMutexCreate (&g_mutex_scuba_data, "Scuba Data", &err);
    my_assert(OS_ERR_NONE == err);
    OSTmrCreate (&g_edt_timer, 
                 "EDT Timer", 
                 0, 
                 50, 
                 OS_OPT_TMR_PERIODIC, 
                 (OS_TMR_CALLBACK_PTR)TimerCallback, 
                 0, 
                 &err);
    my_assert(OS_ERR_NONE == err);
    OSTmrStart(&g_edt_timer, &err);
    my_assert(OS_ERR_NONE == err);
}
 
