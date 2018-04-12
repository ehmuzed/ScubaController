/*
*********************************************************************************************************
*                                             uC/GUI V5.34
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2016, Micrium Inc., Weston, FL
*                       (c) Copyright 2016, SEGGER Microcontroller GmbH & Co. KG
*
*              uC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
*********************************************************************************************************
File        : GUIDEMO_Intro.c
Purpose     : Introduction for uC/GUI generic demo
              (This is also a good file to demo and explain basic
              uC/GUI features by setting breakpoints)
*********************************************************************************************************
*/

#include <string.h>

#include "GUIDEMO.h"
#include "GUIDEMO_API.h"

/*********************************************************************
*
*       Defines
*/
#define SCREEN_DIV          6  // 2^6 = 64

#define FACTOR_GUI          4
#define FACTOR_DESC        11
#define FACTOR_ANY_COMP    22
#define FACTOR_VERSION     31
#define FACTOR_LOGO        38
#define FACTOR_WWW         56

#define DIST_ANY_COMP      18

/*********************************************************************
*
*       GUIDEMO_Intro
*
**********************************************************************
*/
void GUIDEMO_Intro(void)
{
  // Draw background (blue gradient)
  GUIDEMO_DrawBk();

  // Set up text properties (font, color, size, etc.)
  GUI_SetTextMode(GUI_TM_TRANS);
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_FontRounded22);
}

/*************************** End of file ****************************/
