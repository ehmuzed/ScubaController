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
* Filename      : bsp_touch.h
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

#ifndef  BSP_TOUCH_H_
#define  BSP_TOUCH_H_


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

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
                                                                /* ------------ TF5336 TOUCH LCD DATASHEET ------------ */
#define  BSP_TOUCH_FT5336_REG_DEV_MODE                  0x00u
#define  BSP_TOUCH_FT5336_REG_GEST_ID                   0x01u
#define  BSP_TOUCH_FT5336_REG_TD_STATUS                 0x02u

#define  BSP_TOUCH_FT5336_REG_TOUCH1_XH                 0x03u
#define  BSP_TOUCH_FT5336_REG_TOUCH1_XL                 0x04u
#define  BSP_TOUCH_FT5336_REG_TOUCH1_YH                 0x05u
#define  BSP_TOUCH_FT5336_REG_TOUCH1_YL                 0x06u

#define  BSP_TOUCH_FT5336_REG_TOUCH2_XH                 0x09u
#define  BSP_TOUCH_FT5336_REG_TOUCH2_XL                 0x0Au
#define  BSP_TOUCH_FT5336_REG_TOUCH2_YH                 0x0Bu
#define  BSP_TOUCH_FT5336_REG_TOUCH2_YL                 0x0Cu

#define  BSP_TOUCH_FT5336_REG_TOUCH3_XH                 0x0Fu
#define  BSP_TOUCH_FT5336_REG_TOUCH3_XL                 0x10u
#define  BSP_TOUCH_FT5336_REG_TOUCH3_YH                 0x11u
#define  BSP_TOUCH_FT5336_REG_TOUCH3_YL                 0x12u

#define  BSP_TOUCH_FT5336_REG_TOUCH4_XH                 0x15u
#define  BSP_TOUCH_FT5336_REG_TOUCH4_XL                 0x16u
#define  BSP_TOUCH_FT5336_REG_TOUCH4_YH                 0x17u
#define  BSP_TOUCH_FT5336_REG_TOUCH4_YL                 0x18u

#define  BSP_TOUCH_FT5336_REG_TOUCH5_XH                 0x1Bu
#define  BSP_TOUCH_FT5336_REG_TOUCH5_XL                 0x1Cu
#define  BSP_TOUCH_FT5336_REG_TOUCH5_YH                 0x1Du
#define  BSP_TOUCH_FT5336_REG_TOUCH5_YL                 0x1Eu

#define  BSP_TOUCH_FT5336_REG_ID_THGROUP                0x80u
#define  BSP_TOUCH_FT5336_REG_ID_THPEAK                 0x81u
#define  BSP_TOUCH_FT5336_REG_ID_THCAL                  0x82u
#define  BSP_TOUCH_FT5336_REG_ID_THWATER                0x83u
#define  BSP_TOUCH_FT5336_REG_ID_TEMP                   0x84u
#define  BSP_TOUCH_FT5336_REG_ID_THDIFF                 0x85u
#define  BSP_TOUCH_FT5336_REG_ID_CTRL                   0x86u
#define  BSP_TOUCH_FT5336_REG_ID_TIME_MONITOR           0x87u
#define  BSP_TOUCH_FT5336_REG_ID_PERIODACTIVE           0x88u
#define  BSP_TOUCH_FT5336_REG_ID_PERIODMONITOR          0x89u
#define  BSP_TOUCH_FT5336_REG_ID_AUTO_CLB_MODE          0xA0u
#define  BSP_TOUCH_FT5336_REG_ID_LIB_VERSION_H          0xA1u
#define  BSP_TOUCH_FT5336_REG_ID_LIB_VERSION_L          0xA2u
#define  BSP_TOUCH_FT5336_REG_ID_CIPHER                 0xA3u
#define  BSP_TOUCH_FT5336_REG_ID_MODE                   0xA4u
#define  BSP_TOUCH_FT5336_REG_ID_PMODE                  0xA5u
#define  BSP_TOUCH_FT5336_REG_ID_FIRMID                 0xA6u
#define  BSP_TOUCH_FT5336_REG_ID_STATE                  0xA7u
#define  BSP_TOUCH_FT5336_REG_ID_FT5201ID               0xA8u
#define  BSP_TOUCH_FT5336_REG_ID_ERR                    0xA9u
#define  BSP_TOUCH_FT5336_REG_ID_CLB                    0xAAu

#define  BSP_TOUCH_FT5336_REG_LOG_MSG_CNT               0xFEu
#define  BSP_TOUCH_FT5336_REG_LOG_CUR_CHA               0xFFu

                                                                /* -------------- DEVICE MODE REG DEFINES ------------- */
#define  BSP_TOUCH_FT5336_REG_DEV_NORMAL_OP             0x00u
#define  BSP_TOUCH_FT5336_REG_DEV_TEST                  0x04u
#define  BSP_TOUCH_FT5336_REG_DEV_SYS_INFO              0x01u
                                                                /* -------------- GESTURE ID REG DEFINES -------------- */
#define  BSP_TOUCH_FT5336_GEST_ID_ZOOM_IN               0x40u
#define  BSP_TOUCH_FT5336_GEST_ID_ZOOM_OUT              0x49u
#define  BSP_TOUCH_FT5336_GEST_ID_NO_GEST               0x00u
                                                                /* ------------ TOUCH POINT ID REG DEFINES ------------ */
#define  BSP_TOUCH_FT5336_TOUCH_POINT_NONE              0x00u
#define  BSP_TOUCH_FT5336_TOUCH_POINT_1                 0x01u
#define  BSP_TOUCH_FT5336_TOUCH_POINT_2                 0x02u
#define  BSP_TOUCH_FT5336_TOUCH_POINT_3                 0x03u
#define  BSP_TOUCH_FT5336_TOUCH_POINT_4                 0x04u
#define  BSP_TOUCH_FT5336_TOUCH_POINT_5                 0x05u

#define  BSP_TOUCH_FT5336_TOUCH_POINT_MAX               BSP_TOUCH_FT5336_TOUCH_POINT_5

                                                                /* ----------- TOUCH REG EVENT FLAG DEFINES ----------- */
#define  BSP_TOUCH_FT5336_TOUCHx_EVENT_PUT_DWN          0x00u
#define  BSP_TOUCH_FT5336_TOUCHx_EVENT_PUT_UP           0x01u
#define  BSP_TOUCH_FT5336_TOUCHx_EVENT_CONTACT          0x02u
                                                                /* -------------- ID CONTROL REG DEFINES -------------- */
#define  BSP_TOUCH_FT5336_ID_CTRL_PWR_NOT_AUTO_JMP      0x00u
#define  BSP_TOUCH_FT5336_ID_CTRL_PWR_AUTO_JMP          0x01u
                                                                /* ----------- AUTO CALIBRATION REG DEFINES ----------- */
#define  BSP_TOUCH_FT5336_ID_AUTO_CLB_MODE_ENABLE       0x00u
#define  BSP_TOUCH_FT5336_ID_AUTO_CLB_MODE_DISABLE      0xFFu
                                                                /* ------------ INTERRUPT MODE REG DEFINES ------------ */
#define  BSP_TOUCH_FT5336_ID_MODE_INT_ENABLE            0x00u
#define  BSP_TOUCH_FT5336_ID_MODE_INT_DISABLE           0x01u
                                                                /* ----------------- MODE REG DEFINES ----------------- */
#define  BSP_TOUCH_FT5336_ID_PMODE_ACTIVE               0x00u
#define  BSP_TOUCH_FT5336_ID_PMODE_MONITOR              0x01u
#define  BSP_TOUCH_FT5336_ID_PMODE_HIBERNATE            0x03u
                                                                /* ----------------- STATE REG DEFINES ---------------- */
#define  BSP_TOUCH_FT5336_ID_STATE_RUN_CONFIG           0x00u
#define  BSP_TOUCH_FT5336_ID_STATE_RUN_WORK             0x01u
#define  BSP_TOUCH_FT5336_ID_STATE_RUN_CALIB            0x02u
#define  BSP_TOUCH_FT5336_ID_STATE_RUN_FACTORY          0x03u
#define  BSP_TOUCH_FT5336_ID_STATE_RUN_AUTO_CALIB       0x04u
                                                                /* ----------------- ERROR REG DEFINES ---------------- */
#define  BSP_TOUCH_FT5336_ID_ERR_OK                     0x00u
#define  BSP_TOUCH_FT5336_ID_ERR_CHIP_REG               0x03u
#define  BSP_TOUCH_FT5336_ID_ERR_CHIP_START             0x05u
#define  BSP_TOUCH_FT5336_ID_ERR_CALIB_FAIL             0x1Au


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/
                                                                /* ---------------- CTPM VENDOR CHIP ID --------------- */
#define  BSP_TOUCH_FT5336_VENDOR_ID                     0x51u

                                                                /* --------------------- MISC VARs -------------------- */
#define  BSP_TOUCH_FT5336_REGs_PER_TOUCH                4u      /* 4 Register for Every Touch Point: XH, XL, YH, YL.    */


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

typedef  struct  bsp_touch_struct {                             /* ---------------- I2C TOUCH STRUCTURE --------------- */
    CPU_INT16U   Xpos;                                          /* X-Axis Coordinate Position of 'Touched' Value.       */
    CPU_INT16U   Ypos;                                          /* Y-Axis Coordinate Position of 'Touched' Value.       */
} BSP_TOUCH_STRUCT;

typedef  enum  bsp_touch_status {                               /* ------------- I2C TOUCH STATUS DEFINES ------------- */
    BSP_TOUCH_STATUS_INACTIVE = 0u,                             /* I2C Handshake Successful, No Touch Points Recorded.  */
    BSP_TOUCH_STATUS_ACTIVE   = 1u,                             /* I2C Handshake Successful, Active Touch Points Found. */
    BSP_TOUCH_STATUS_ERR      = 2u,                             /* Error in I2C Handshake Communication.                */
} BSP_TOUCH_STATUS;



/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

CPU_BOOLEAN       BSP_Touch_Init  (void);

BSP_TOUCH_STATUS  BSP_Touch_Read  (BSP_TOUCH_STRUCT  *touch_rd);


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
