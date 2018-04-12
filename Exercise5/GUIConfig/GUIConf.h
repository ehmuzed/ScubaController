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
File        : GUIConf.h
Purpose     : Configuration of available features and default values
*********************************************************************************************************
*/

#ifndef GUICONF_H
#define GUICONF_H

/*********************************************************************
*
*       Multi layer/display support
*/
#define GUI_NUM_LAYERS             2   // Maximum number of available layers

/*********************************************************************
*
*       Multi tasking support
*/
#define GUI_OS                    (1)  // Compile with multitasking support

/*********************************************************************
*
*         Configuration of available packages
*/
#define GUI_SUPPORT_TOUCH         (0)  // Support touchscreen
#define GUI_SUPPORT_MOUSE         (0)  // Support a mouse
#define GUI_SUPPORT_UNICODE       (0)  // Support mixed ASCII/UNICODE strings
#define GUI_WINSUPPORT            (1)  // Use window manager
#define GUI_SUPPORT_MEMDEV        (0)  // Memory device package available
#define GUI_SUPPORT_DEVICES       (1)  // Enable use of device pointers
#define GUI_SUPPORT_AA            (1)  // Anti aliasing available

#endif  /* Avoid multiple inclusion */

/*************************** End of file ****************************/
