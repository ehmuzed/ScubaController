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
*                                         STM32F746G-DISCO
*                                         Evaluation Board
*
* Filename      : bsp_touch.c
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  "app_cfg.h"

#include  "os.h"
#include  "gui.h"

#include  "bsp_osal.h"
#include  "bsp_touch.h"
#include  "stm32f7xx_hal.h"


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
                                                                /* ----------------- I2C TOUCH DEFINES ---------------- */
#define  BSP_TOUCH_I2C3_TOUCH_DEV_ADDR              0x70u

#define  BSP_TOUCH_LCD_ISR_EXTI_PIN                 GPIO_PIN_13

#define  BSP_TOUCH_VAL_TIMEOUT                      0xFFFFu


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
                                                                /* ------------- TOUCH LCD I2C CHANNEL VAR ------------ */
I2C_HandleTypeDef  TouchI2C;                                    /* Touchscreen's I2C Handle.                            */

                                                                /* ----------------- TOUCH SEMAPHORES ----------------- */
BSP_OSAL_SEM       TouchTsk_SignalSem;                          /* Touch Task Signal Semaphore.                         */

BSP_OSAL_SEM       I2C_TxCompl_Sem;                             /* Transmit & Receive Completion I2C Semaphores.        */
BSP_OSAL_SEM       I2C_RxCompl_Sem;

                                                                /* ------------------ TOUCH TASK VARS ----------------- */
BSP_OSAL_TCB       TouchTask_TCB;
BSP_OSAL_STK       TouchTask_Stk[APP_CFG_TOUCH_TASK_STK_SIZE];

void               Touch_Task (void  *p_arg);


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Touch_I2C_Rd (CPU_INT08U   reg_addr,           /* I2C Read Function from FT5336 Touchscreen.           */
                               CPU_INT08U  *data,
                               CPU_INT16U   count);

CPU_BOOLEAN  BSP_Touch_I2C_Wr (CPU_INT08U   reg_addr,           /* I2C Write Function to FT5336 Touchscreen.            */
                               CPU_INT08U   data);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                   INITIALIZE LCD TOUCH CAPABILITY
*
* Description : Initializes the I2C channel and additional modules required to use the capacitive touch
*               screen found on the LCD.
*
* Argument(s) : none.
*
* Return(s)   : DEF_FAIL : I2C3 Channel wasn't ready or Slave didn't Match FT5336 Touch Screen Ctrl.
*               DEF_OK   : Initialization was Successful.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : (1) The Touch Screen on the STM32F746-Discovery board is based on an FT5336 Touch
*                   Screen Controller.
*                   (A) This Touch Screen Controller Supports Multi-Touch. However, it has not been
*                       implemented in this BSP.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Touch_Init (void)
{
    GPIO_InitTypeDef   touchGPIO;
    HAL_StatusTypeDef  Hal_err;
    CPU_BOOLEAN        osal_err;
    CPU_INT08U         devID;
    CPU_INT08U         isr_status;
    CPU_INT32U         timeout;

                                                                /* ------------- CREATE LOCKING SEMAPHORES ------------ */
    osal_err = BSP_OSAL_SemCreate(&I2C_TxCompl_Sem,             /* I2C Tx Complete Switch Semaphore.                    */
                                  0u,
                                 "I2C Tx Compl Sem");
    if (osal_err != DEF_OK) {                                   /* Check Proper Creation of Semaphore.                  */
        return (DEF_FAIL);
    }

    osal_err = BSP_OSAL_SemCreate(&I2C_RxCompl_Sem,             /* I2C Rx Complete Switch Semaphore.                    */
                                  0u,
                                 "I2C Rx Compl Sem");
    if (osal_err != DEF_OK) {                                   /* Check Proper Creation of Semaphore.                  */
        return (DEF_FAIL);
    }

    osal_err = BSP_OSAL_SemCreate(&TouchTsk_SignalSem,          /* Touch Task Signal Switch Semaphore.                  */
                                  0u,
                                 "Touch Task Signal Sem");
    if (osal_err != DEF_OK) {                                   /* Check Proper Creation of Semaphore.                  */
        return (DEF_FAIL);
    }

                                                                /* ------------- ENABLE PERIPHERAL CLOCKS ------------- */
    __HAL_RCC_I2C3_CLK_ENABLE();                                /* Enable the I2C Channel 3 Clock.                      */
    __HAL_RCC_GPIOH_CLK_ENABLE();                               /* Enable the GPIO H Pin Clock for GPIO Configuration.  */
    __HAL_RCC_GPIOI_CLK_ENABLE();                               /* Enable the GPIO H Pin Clock for GPIO Configuration.  */

                                                                /* ----------------- TOUCH I2C CONFIG ----------------- */
    __HAL_RCC_I2C3_FORCE_RESET();                               /* Force   the I2C3 Module  to  Reset.                  */
    __HAL_RCC_I2C3_RELEASE_RESET();                             /* Release the I2C3 Module from Reset.                  */

    TouchI2C.Instance = I2C3;                                   /* Set Touch Screen I2C Instance: I2C Channel 3         */
    TouchI2C.Init.Timing          = 0x40912732u;                /* I2C Channel Timing Settings.                         */
                                                                /* Timing               : Prescaler       - 4           */
                                                                /*                      : Data Setup Time - 9           */
                                                                /*                      : Data Hold  Time - 1           */
                                                                /*                      : SCL High Period - 39          */
                                                                /*                      : SCL Low  Period - 20          */
    TouchI2C.Init.OwnAddress1     = 0u;                         /* Own Address 1 & 2    : Disabled.                     */
    TouchI2C.Init.OwnAddress2     = 0u;                         /*                        The Slave Addr is NACKed.     */
    TouchI2C.Init.AddressingMode  = DEF_ON;                     /* Addressing Mode      : 7-Bit Accessing Mode.         */
    TouchI2C.Init.DualAddressMode = DEF_OFF;                    /* Dual Addressing Mode : Disabled.                     */
    TouchI2C.Init.GeneralCallMode = DEF_OFF;                    /* General Call Mode    : Disabled.                     */
    TouchI2C.Init.NoStretchMode   = DEF_OFF;                    /* No Stretch Mode      : Disabled.                     */

                                                                /* ------------------ EXTI ISR CONFIG ----------------- */
    touchGPIO.Pin       = BSP_TOUCH_LCD_ISR_EXTI_PIN;           /* Touchscreen Ext LCD Int : GPIO Port I, Pin 13.       */
    touchGPIO.Mode      = GPIO_MODE_IT_RISING_FALLING;          /* GPIO Mode               : EXTI Pin, Dual-Edge Trig.  */
    touchGPIO.Pull      = GPIO_NOPULL;                          /* GPIO Pull               : No Pull.                   */
    HAL_GPIO_Init(GPIOI, &touchGPIO);                           /* Initialize Ext LCD Int Pin.                          */

                                                                /* --------------- TOUCH I2C GPIO CONFIG -------------- */
    touchGPIO.Pin       = GPIO_PIN_7;                           /* I2C3 SCL (Tx) Pin  : GPIO Port H, Pin 7.             */
    touchGPIO.Mode      = GPIO_MODE_AF_OD;                      /* GPIO Mode          : Alternate Function, Open Drain. */
    touchGPIO.Pull      = GPIO_NOPULL;                          /* GPIO Pull          : No Pull.                        */
    touchGPIO.Speed     = GPIO_SPEED_FAST;                      /* GPIO Speed         : Set Fast Speed.                 */
    touchGPIO.Alternate = GPIO_AF4_I2C3;                        /* Alternate Function : Alternate Function 4 for I2C3.  */
    HAL_GPIO_Init(GPIOH, &touchGPIO);                           /* Initialize I2C Tx Pin according to Config.           */

    touchGPIO.Pin       = GPIO_PIN_8;                           /* I2C3 SCA (Rx) Pin  : GPIO Port H, Pin 8.             */
    HAL_GPIO_Init(GPIOH, &touchGPIO);                           /* Initialize I2C Rx Pin according to Prev Config.      */
                                                                /* -------------------- ISR CONFIG -------------------- */
    HAL_NVIC_SetPriority(I2C3_EV_IRQn, 0x05u, 0u);              /* Enable and Set the I2C3 Event Int IRQ to Low Prio.   */
    HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);

    HAL_NVIC_SetPriority(I2C3_ER_IRQn, 0x05u, 0u);              /* Enable and Set the I2C3 Error Int IRQ to Low Prio.   */
    HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0x05u, 0u);            /* Enable and Set the EXTI Ch[10-15] Int IRQ to Low Prio*/
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

                                                                /* ------------------ INIT I2C TOUCH ------------------ */
    HAL_I2C_Init(&TouchI2C);                                    /* Initialize the I2C Touch Screen Channel.             */

                                                                /* ---------------- CHECK DEVICE STATUS --------------- */
    HAL_Delay(50u);                                             /* A delay is needed before checking if I2C ch is ready */
    Hal_err = HAL_I2C_IsDeviceReady(&TouchI2C,                  /* Test if I2C Channel 3 is Ready and Comm w/ Touch.    */
                                     BSP_TOUCH_I2C3_TOUCH_DEV_ADDR,
                                     2u,
                                     5u);
    if (Hal_err != HAL_OK) {                                    /* Check if Device - I2C3 Channel is Ready.             */
        return (DEF_FAIL);
    }

    timeout = BSP_TOUCH_VAL_TIMEOUT;
    do {
        BSP_Touch_I2C_Rd( BSP_TOUCH_FT5336_REG_ID_FT5201ID,     /* Check Touch Screen's Vendor ID for Match.            */
                         &devID,
                          1u);
        timeout--;
    } while ((devID   != BSP_TOUCH_FT5336_VENDOR_ID) &&
             (timeout != 0u                        ));

    if (devID != BSP_TOUCH_FT5336_VENDOR_ID) {                  /* Match Reply with Known Vendor ID.                    */
        return (DEF_FAIL);
    }
                                                                /* ------------- CHECK FT5336 ISR SETTINGS ------------ */
    BSP_Touch_I2C_Rd( BSP_TOUCH_FT5336_REG_ID_MODE,             /* Check the FT5336's current 'ISR to Host' Status.     */
                     &isr_status,
                      1u);

    if (isr_status != BSP_TOUCH_FT5336_ID_MODE_INT_ENABLE) {    /* If ISR Status is Disabled, Enable it & check Status. */
        BSP_Touch_I2C_Wr( BSP_TOUCH_FT5336_REG_ID_MODE,
                          BSP_TOUCH_FT5336_ID_MODE_INT_ENABLE);
    }

    BSP_Touch_I2C_Rd( BSP_TOUCH_FT5336_REG_ID_MODE,             /* Confirm ISR Status is Enabled, if not Return Error.  */
                     &isr_status,
                      1u);

    if (isr_status != BSP_TOUCH_FT5336_ID_MODE_INT_ENABLE) {
        return (DEF_FAIL);
    }

                                                                /* ---------------- TOUCH TASK CREATION --------------- */
    osal_err = BSP_OSAL_TaskCreate(&TouchTask_TCB,              /* Create Touch Task to Manage Touch Signals.           */
                                    &TouchTask_Stk[0u],
                                     Touch_Task,
                                    "GUI Touch Task",
                                     APP_CFG_TOUCH_TASK_PRIO,
                                     APP_CFG_TOUCH_TASK_STK_SIZE);
    if (osal_err != DEF_OK) {
        return (DEF_FAIL);
    }

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                       READ LCD 'TOUCHED' VALUE
*
* Description : Read the Touch Screen's I2C channel and obtain the Status, X & Y Coordinates of the
*               Touch Screen's state.
*
* Argument(s) : touch_rd    Read Touch Coordinates to return from the I2C's Touch Screen
*
* Return(s)   : Returns the Status of the Read Function:
*                   - BSP_TOUCH_STATUS_INACTIVE : If there is NO Active Touch on the Touchscreen.
*                   - BSP_TOUCH_STATUS_ACTIVE   : If there is A new Active Touch on the Touchscreen.
*                   - BSP_TOUCH_STATUS_ERR      : If there is an Error in I2C Communication.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

BSP_TOUCH_STATUS  BSP_Touch_Read (BSP_TOUCH_STRUCT  *touch_rd)
{
    CPU_BOOLEAN  I2C_err;
    CPU_INT08U   touch_status;
    CPU_INT08U   rd_touch[BSP_TOUCH_FT5336_REGs_PER_TOUCH];

                                                                /* ------------------- SEND ADDRESS ------------------- */
    I2C_err = BSP_Touch_I2C_Rd( BSP_TOUCH_FT5336_REG_TD_STATUS, /* Read Qty of Touches on the Touchscreen.              */
                               &touch_status,
                                1u);

    if (I2C_err != DEF_OK) {                                    /* Return with Error on I2C Handshake Request/Reply.    */
        return (BSP_TOUCH_STATUS_ERR);
    }

    touch_status &= 0x0Fu;                                      /* Mask to tell how many Touch Points have been Pressed.*/

    if (touch_status == BSP_TOUCH_FT5336_TOUCH_POINT_NONE) {    /* If no Touch Points, Return with INACTIVE, Touch ...  */
        return (BSP_TOUCH_STATUS_INACTIVE);                     /* ... Screen was Released, Signal GUI State Change.    */
    } else if (touch_status > BSP_TOUCH_FT5336_TOUCH_POINT_MAX) {
        return (BSP_TOUCH_STATUS_ERR);                          /* Junk or Out-of-Bound Values.                         */
    }

                                                                /* Read 1st Touch Regs Only. Can be Mod for MultiTouch. */
    I2C_err = BSP_Touch_I2C_Rd( BSP_TOUCH_FT5336_REG_TOUCH1_XH,
                                rd_touch,
                                sizeof(rd_touch));

    touch_rd->Xpos = (rd_touch[3u]) | ((rd_touch[2u] & 0x0Fu) << 8u);
    touch_rd->Ypos = (rd_touch[1u]) | ((rd_touch[0u] & 0x0Fu) << 8u);

    return (BSP_TOUCH_STATUS_ACTIVE);
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                             TOUCH TASK
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             Touch_Task()
*
* Description : Task that manages the I2C FT5336 Touch Screen Controller and updates uC/GUI's Touch
*               Status.
*
* Argument(s) : none.
*
* Return(s)   : This is a Task.
*
* Caller(s)   : BSP_Touch_Init()
*
* Note(s)     : (1) This Task has been specifically designed to manage the Touch Status of the GUI and
*                   the FT5336 Touch Screen Controller.
*
*               (2) Although not a typical location for a Task, this has been placed in a location that
*                   will Initialize and Control the Touch capabilities all in one Function Call
*                   (BSP_Touch_Init).
*********************************************************************************************************
*/

void Touch_Task (void  *p_arg)
{
    BSP_TOUCH_STATUS  status;
    BSP_TOUCH_STRUCT  rd_touch;
    GUI_PID_STATE     gui_state;


    while (DEF_ON) {
        BSP_OSAL_SemPend(&TouchTsk_SignalSem,                   /* Pend Task until Status Update from Touch Screen Ctrl */
                          0u);

        GUI_PID_GetState(&gui_state);                           /* Get the current state of the GUI 'Touch' State.      */

        status = BSP_Touch_Read(&rd_touch);                     /* Read Touchscreen to see if what's the new 'status'.  */

        if (status == BSP_TOUCH_STATUS_ACTIVE) {                /* ------------- ACTIVE STATUS - NEW TOUCH ------------ */
            if (gui_state.Pressed == DEF_OFF) {                 /* If GUI Pressed State is OFF, update GUI State.       */
                gui_state.x       = rd_touch.Xpos;              /* Give GUI State new X & Y Position Values.            */
                gui_state.y       = rd_touch.Ypos;
                gui_state.Pressed = DEF_ON;

                GUI_PID_StoreState(&gui_state);                 /* Store updated GUI State.                             */
            }
        } else if (status == BSP_TOUCH_STATUS_INACTIVE) {       /* --------- INACTIVE STATUS - TOUCH RELEASED --------- */
            if (gui_state.Pressed == DEF_ON) {                  /* If GUI Pressed State is ON, update GUI State.        */
                gui_state.Pressed = DEF_OFF;

                GUI_PID_StoreState(&gui_state);                 /* Store updated GUI State.                             */
            }
        }
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                          LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        I2C3 TOUCHSCREEN READ
*
* Description : Reads from the FT5336 Touchscreen using I2C Communication. This function will send the
*               Slave (Touchscreen) the Requested Address, then will read back the data reply.
*
* Argument(s) : reg_addr    Address requested by Master
*
*               data        Data to return to the Master
*
*               count       Qty of bytes to return
*
* Return(s)   : DEF_FAIL : If something occurred during the I2C Communication.
*               DEF_OK   : Everything went OK.
*
* Caller(s)   : BSP_Touch_Init().
*               BSP_Touch_Read().
*
* Note(s)     : (1) This function has been configured for the Touchscreen only, as such the I2C Handle
*                   as well as the Slave Address are constant.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Touch_I2C_Rd (CPU_INT08U   reg_addr,
                               CPU_INT08U  *data,
                               CPU_INT16U   count)
{
    HAL_StatusTypeDef  Hal_err;


    Hal_err = HAL_I2C_Master_Transmit_IT(&TouchI2C,             /* Request Data from the Touch Screen's Register.       */
                                          BSP_TOUCH_I2C3_TOUCH_DEV_ADDR,
                                         &reg_addr,
                                          1u);
    if (Hal_err != HAL_OK) {
        return (DEF_FAIL);
    }

    BSP_OSAL_SemPend(&I2C_TxCompl_Sem,                          /* Pend Forever until Tx Complete ISR.                  */
                      0u);

    Hal_err = HAL_I2C_Master_Receive_IT(&TouchI2C,              /* Read the Data from the Requested Register.           */
                                         BSP_TOUCH_I2C3_TOUCH_DEV_ADDR,
                                         data,
                                         count);

    if (Hal_err != HAL_OK) {
        return (DEF_FAIL);
    }

    BSP_OSAL_SemPend(&I2C_RxCompl_Sem,                          /* Pend Forever until Rx Complete ISR.                  */
                      0u);

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        I2C3 TOUCHSCREEN WRITE
*
* Description : Writes to the FT5336 Touchscreen using I2C Communication. This function will send the
*               Slave (Touchscreen) the Requested Address as well as the Byte of Data that will be
*               written.
*
* Argument(s) : reg_addr    Desired Register Address to Write
*
*               data        Data to Write
*
* Return(s)   : DEF_FAIL : If something occurred during the I2C Communication.
*               DEF_OK   : Everything went OK.
*
* Caller(s)   : BSP_Touch_Init().
*
* Note(s)     : (1) This function has been configured for the Touchscreen only, as such the I2C Handle
*                   as well as the Slave Address are constant.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Touch_I2C_Wr (CPU_INT08U  reg_addr,
                               CPU_INT08U  data)
{
    HAL_StatusTypeDef  Hal_err;
    CPU_INT08U         tx_data[2u];


    tx_data[0u] = reg_addr;                                     /* Format Data Array to Send to Touchscreen.            */
    tx_data[1u] = data;

    Hal_err = HAL_I2C_Master_Transmit_IT(&TouchI2C,             /* Send Data to the Touch Screen's Register.            */
                                          BSP_TOUCH_I2C3_TOUCH_DEV_ADDR,
                                         &tx_data[0u],
                                          sizeof(tx_data));
    if (Hal_err != HAL_OK) {
        return (DEF_FAIL);
    }

    BSP_OSAL_SemPend(&I2C_TxCompl_Sem,                          /* Pend Forever until Tx Complete ISR.                  */
                      0u);

    return (DEF_OK);
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                               I2C CHANNEL 3 ISR HANDLER & CALLBACKS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                I2C3 MASTER Tx & Rx COMPLETE CALLBACKS
*
* Description : Master Tx & Rx Complete Callback Functions. This will Trigger the continuation of the
*               HAL_I2C_Master_Transmit_IT() & HAL_I2C_Master_Receive_IT() functions to perform I2C
*               'handshake' from the Touchscreen without any issues.
*
* Argument(s) : hi2c        I2C Handle that triggered the Tx Complete Callback
*
* Return(s)   : none.
*
* Caller(s)   : This is a callback within an ISR.
*
* Note(s)     : (1) Sequence is as Follows (For Tx & Rx):
*                       HAL_I2C_Master_Transmit_IT() -> SemPend() -> MasterTxCpltCallback -> SemPost()
*
*               (2) Although currently not necessary, there will be a comparison between the argument
*                   and the Touchscreen's I2C Handle to make sure that we have the correct handle before
*                   continuing on. Thus preventing any future I2C code from corrupting the I2C3's
*                   communication handshake.
*********************************************************************************************************
*/

void  HAL_I2C_MasterTxCpltCallback (I2C_HandleTypeDef  *hi2c)
{
    if (hi2c == &TouchI2C) {                                    /* Touchscreen's I2C Handle triggered Tx Comp Callback. */
        BSP_OSAL_SemPost(&I2C_TxCompl_Sem);                     /* Post to Tx Complete Semaphore to Continue Handshake. */
    }
}


void  HAL_I2C_MasterRxCpltCallback (I2C_HandleTypeDef  *hi2c)
{
    if (hi2c == &TouchI2C) {                                    /* Touchscreen's I2C Handle triggered Rx Comp Callback. */
        BSP_OSAL_SemPost(&I2C_RxCompl_Sem);                     /* Post to Rx Complete Semaphore to Continue Handshake. */
    }
}


/*
*********************************************************************************************************
*                                        I2C3 EVENT ISR HANDLER
*
* Description : Strong Declaration of the I2C Channel 3 Event ISR Handler, to handle Touchscreen Events.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : (1) This ISR will be OS-Aware as the Master Tx Complete and Master Rx Complete Callbacks
*                   get called within the generic HAL I2C EV IRQ Handler. The OS-Aware sequence is
*                   described in the 'README_ISR_HANDLER.txt' file.
*********************************************************************************************************
*/

void I2C3_EV_IRQHandler (void)
{
     CPU_SR_ALLOC();                                            /* Allocate storage for CPU Status Register             */


     CPU_CRITICAL_ENTER();
     OSIntEnter();                                              /* Tell OS that we are starting an ISR                  */
     CPU_CRITICAL_EXIT();

     HAL_I2C_EV_IRQHandler(&TouchI2C);

     OSIntExit();                                               /* Tell OS that we are leaving the ISR                  */
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                   EXT_ISR ISR HANDLER & CALLBACK
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                   EXTI GPIO LINE DETECTED CALLBACK
*
* Description : Line Detection Callback for the EXTI GPIO passed in. This Callback will only occur if
*               there has been a Line Detection that is valid.
*
* Argument(s) : hi2c        I2C Handle that triggered the Tx Complete Callback
*
* Return(s)   : none.
*
* Caller(s)   : This is a callback within an ISR.
*
* Note(s)     : (1) Sequence is as Follows:
*                       HAL_GPIO_EXTI_IRQHandler() -> HAL_GPIO_EXTI_Callback -> Task SemPost()
*
*               (2) Although currently not necessary, there will be a comparison between the argument
*                   and the Touchscreen's EXTI Pin to make sure that we have the correct pin before
*                   continuing on.
*********************************************************************************************************
*/

void  HAL_GPIO_EXTI_Callback (uint16_t  GPIO_Pin)
{
    if (GPIO_Pin == BSP_TOUCH_LCD_ISR_EXTI_PIN) {
        BSP_OSAL_SemPost(&TouchTsk_SignalSem);                  /* Post to the Touch Task Semaphore to Read FT5336.     */
    }
}


/*
*********************************************************************************************************
*                                       EXT LCD INT ISR HANDLER
*
* Description : External Interrupt (EXTI) ISR Handler for Channels 10->15. Since the LCD_INT is found
*               in Channel 13, this ISR handler will (currently) only handle LCD_INT Events.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : (1) This ISR will be OS-Aware since the Post to the 'Touch Task' will occur from here
*                   informing that task that there was a new Event (Either Touch Released or Pressed).
*********************************************************************************************************
*/

void  EXTI15_10_IRQHandler (void)
{
     CPU_SR_ALLOC();                                            /* Allocate storage for CPU Status Register             */


     CPU_CRITICAL_ENTER();
     OSIntEnter();                                              /* Tell OS that we are starting an ISR                  */
     CPU_CRITICAL_EXIT();

     HAL_GPIO_EXTI_IRQHandler(BSP_TOUCH_LCD_ISR_EXTI_PIN);

     OSIntExit();                                               /* Tell OS that we are leaving the ISR                  */
}