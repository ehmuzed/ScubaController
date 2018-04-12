/*
*********************************************************************************************************
*                                             uC/GUI V5.28
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2015, Micrium Inc., Weston, FL
*                       (c) Copyright 2015, SEGGER Microcontroller GmbH & Co. KG
*
*              uC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
*********************************************************************************************************
File        : LCDConf.h
Purpose     : Display driver configuration file
*********************************************************************************************************
*/

#include "stm32f7xx_hal.h"
#include "GUI.h"
#include "GUIDRV_Lin.h"

#ifndef LCDCONF_H
#define LCDCONF_H

//
// Physical display size
//
#define XSIZE_PHYS 480
#define YSIZE_PHYS 272

typedef struct
{
  int32_t               address;
  __IO int32_t          pending_buffer;
  int32_t               buffer_index;
  int32_t               xSize;
  int32_t               ySize;
  int32_t               BytesPerPixel;
  LCD_API_COLOR_CONV   *pColorConvAPI;
}
LCD_LayerPropTypedef;

#endif /* LCDCONF_H */

/*************************** End of file ****************************/
