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
File        : GUI_SPRITE_Private.h
Purpose     : Private header file for sprites
*********************************************************************************************************
*/

#ifndef GUI_SPRITE_PRIVATE_H
#define GUI_SPRITE_PRIVATE_H

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define SPRITE_LOCK_H(h) (GUI_SPRITE_OBJ *)GUI_LOCK_H(h)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
//
// The sprite object
//
typedef struct {
  GUI_DEVICE       * pDevice;
  GUI_RECT           Rect;
  GUI_HMEM           hColors;
  U16                Flags;
  const GUI_BITMAP * pBM;
  void            (* pCB)(GUI_HSPRITE hSprite, int Cmd); // Callback routine for animated sprites
  GUI_HMEM           hContext;
} GUI_SPRITE_OBJ;


#endif // GUI_SPRITE_PRIVATE_H

/*************************** End of file ****************************/
