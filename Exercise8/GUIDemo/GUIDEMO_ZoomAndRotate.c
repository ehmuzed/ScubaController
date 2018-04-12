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
File        : GUIDEMO_ZoomAndRotate.c
Purpose     : Demo of zooming and rotating images
*********************************************************************************************************
*/

#include "GUIDEMO.h"

#if (SHOW_GUIDEMO_ZOOMANDROTATE && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV)

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MAG                        300
#define TITLE_SIZE                 40
#define MIN_TIME_PER_FRAME_SHIFT   20
#define MIN_TIME_PER_FRAME_ENLARGE 30
#define BORDER_SIZE                4
#define FINAL_STEP                 100

//
// Memory device format in dependence of color format
//
#if (GUI_USE_ARGB)
  #define GUI_COLOR_CONV GUICC_M8888I
#else
  #define GUI_COLOR_CONV GUICC_8888
#endif


/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  const GUI_BITMAP GUI_UNI_PTR * pBitmap;
  GUI_MEMDEV_Handle              hMem;
  int                            xSize;
  int                            ySize;
} IMAGE;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static IMAGE _aImage[] = {
  { &bmMicrium_Logo_200x56 },
  { &bmOS2_Logo            },
  { &bmOS3_Logo            },
  { &bmGUI_Logo            }
};

static int _NumImages = sizeof(_aImage) / sizeof(_aImage[0]);

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetImage
*/
static int _GetImage(IMAGE * pImage) {
  pImage->xSize = pImage->pBitmap->XSize;
  pImage->ySize = pImage->pBitmap->YSize;
  pImage->hMem  = GUI_MEMDEV_CreateFixed(0, 0, pImage->xSize, pImage->ySize, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV);
  if (pImage->hMem == 0) {
    return 1;
  }
  GUI_MEMDEV_Select(pImage->hMem);
  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_Clear();
  GUI_DrawBitmap(pImage->pBitmap, 0, 0);
  GUI_MEMDEV_Select(0);
  return 0;
}

/*********************************************************************
*
*       _CopyFromLCD
*/
static void _CopyFromLCD(GUI_MEMDEV_Handle hMem) {
#if GUIDEMO_SUPPORT_CURSOR
  GUIDEMO_HideCursor();
#endif
  GUI_MEMDEV_CopyFromLCD(hMem);
#if GUIDEMO_SUPPORT_CURSOR
  GUIDEMO_ShowCursor();
#endif
}

/*********************************************************************
*
*       _RotateAndZoomImage
*/
static int _RotateAndZoomImage(void) {
  static int           Index;
  GUI_MEMDEV_Handle    hLCD;
  GUI_MEMDEV_Handle    hDst;
  IMAGE              * pImage;
  void              (* pfRotate)(GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
  int                  TimeContinue;
  int                  TimeNow;
  int                  xSize;
  int                  ySize;
  int                  Mag;
  int                  a;
  int                  i;

  pfRotate = GUI_MEMDEV_Rotate;
  pImage   = &_aImage[Index];
  xSize    = LCD_GetXSize();
  ySize    = LCD_GetYSize();
  //
  // Create memory devices if no exist
  //
  hLCD = GUI_MEMDEV_CreateFixed(0, 0, xSize, ySize, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV);
  if (hLCD == 0) {
    return 1;
  }
  _CopyFromLCD(hLCD);
  hDst = GUI_MEMDEV_CreateFixed(0, 0, xSize, ySize, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV);
  if (hDst == 0) {
    GUI_MEMDEV_Delete(hLCD);
    return 1;
  }
  ySize -= TITLE_SIZE;
  //
  // Rotate and enlarge while moving in.
  //
  for (i = 2; (i <= 100); i += 2) {
    TimeContinue = GUIDEMO_GetTime() + MIN_TIME_PER_FRAME_ENLARGE;
    GUI_MEMDEV_Select(hDst);
    GUI_MEMDEV_Write(hLCD);
    pfRotate(pImage->hMem, hDst, (xSize - pImage->xSize) / 2 - (xSize * (100 - i)) / 250, TITLE_SIZE + (ySize - pImage->ySize) / 2, (i - 100) * 450, i * 10);
    GUI_MEMDEV_Select(0);
    GUI_MULTIBUF_Begin();
    GUI_MEMDEV_CopyToLCD(hDst);
    GUI_MULTIBUF_End();
    //
    // Delay
    //
    TimeNow = GUIDEMO_GetTime();
    if (TimeContinue > TimeNow) {
      GUI_Delay(TimeContinue - TimeNow);
    } else {
      GUI_Exec();
    }
    if (GUIDEMO_CheckCancel()) {
      GUI_MEMDEV_Delete(hLCD);
      GUI_MEMDEV_Delete(hDst);
      return 1;
    }
  }
  //
  // Rotate, enlarge and shrink on spot.
  //
  Mag = 1000;
  for (a = 5; a <= 360; a += 5) {
    TimeContinue = GUIDEMO_GetTime() + MIN_TIME_PER_FRAME_ENLARGE;
    //
    // Calculate magnification
    //
    if (a <= 180) {
      Mag = (Mag * 1050) / 1000;
    } else {
      Mag = (Mag * 1000) / 1050;
    }
    //
    // Do animation
    //
    GUI_MEMDEV_Select(hDst);
    GUI_MEMDEV_Write(hLCD);
    pfRotate(pImage->hMem, hDst, (xSize - pImage->xSize) / 2, TITLE_SIZE + (ySize - pImage->ySize) / 2, a * 1000, Mag);
    GUI_MEMDEV_Select(0);
    GUI_MULTIBUF_Begin();
    GUI_MEMDEV_CopyToLCD(hDst);
    GUI_MULTIBUF_End();
    //
    // Delay
    //
    TimeNow = GUIDEMO_GetTime();
    if (TimeContinue > TimeNow) {
      GUI_Delay(TimeContinue - TimeNow);
    } else {
      GUI_Exec();
    }
    if (GUIDEMO_CheckCancel()) {
      GUI_MEMDEV_Delete(hLCD);
      GUI_MEMDEV_Delete(hDst);
      return 1;
    }
  }
  //
  // Rotate complete screen
  //
  GUI_MEMDEV_CopyFromLCD(hLCD);
  GUIDEMO_ConfigureDemo(NULL, NULL, GUIDEMO_SHOW_CURSOR);
  WM_ValidateWindow(WM_HBKWIN);
  for (a = 5; a <= 360; a += 5) {
    TimeContinue = GUI_GetTime() + 30;
    //
    // Calculate magnification
    //
    Mag = (a < 180)
        ? ((180 - a) * 800) / 180 + 200
        : ((a - 180) * 800) / 180 + 200;
    //
    // Do animation
    //
    GUI_MEMDEV_Select(hDst);
    GUI_SetBkColor(GUI_MAKE_COLOR(0xa56e3a));
    GUI_Clear();
    GUI_MEMDEV_Select(0);
    pfRotate(hLCD, hDst, 0, 0, a * 1000, Mag);
    GUI_MULTIBUF_Begin();
    GUI_MEMDEV_CopyToLCD(hDst);
    GUI_MULTIBUF_End();
    //
    // Delay
    //
    TimeNow = GUI_GetTime();
    if (TimeContinue > TimeNow) {
      GUI_Delay(TimeContinue - TimeNow);
    }
  }
  GUIDEMO_ConfigureDemo(NULL, NULL, GUIDEMO_SHOW_CURSOR | GUIDEMO_SHOW_CONTROL);
  //
  // Clean up LCD context
  //
  GUI_MEMDEV_Select(hLCD);
  GUI_DrawGradientV(0, 40, xSize - 1, ySize + TITLE_SIZE - 1, GUI_WHITE, GUI_LIGHTBLUE);
  //
  // Rotate and enlarge while moving out.
  //
  for (i = 2; (i <= 100); i += 2) {
    TimeContinue = GUIDEMO_GetTime() + MIN_TIME_PER_FRAME_ENLARGE;
    GUI_MEMDEV_Select(hDst);
    GUI_MEMDEV_Write(hLCD);
    pfRotate(pImage->hMem, hDst, (xSize - pImage->xSize) / 2 + (xSize * i) / 250, TITLE_SIZE + (ySize - pImage->ySize) / 2, i * 450, (100 - i) * 10);
    GUI_MEMDEV_Select(0);
    GUI_MULTIBUF_Begin();
    GUI_MEMDEV_CopyToLCD(hDst);
    GUI_MULTIBUF_End();
    //
    // Delay
    //
    TimeNow = GUIDEMO_GetTime();
    if (TimeContinue > TimeNow) {
      GUI_Delay(TimeContinue - TimeNow);
    } else {
      GUI_Exec();
    }
    if (GUIDEMO_CheckCancel()) {
      GUI_MEMDEV_Delete(hLCD);
      GUI_MEMDEV_Delete(hDst);
      return 1;
    }
  }
  GUI_MULTIBUF_Begin();
  GUI_MEMDEV_CopyToLCD(hLCD);
  GUI_MULTIBUF_End();
  Index = (Index + 1) % _NumImages;
  //
  // Remove memory devices
  //
  GUI_MEMDEV_Delete(hLCD);
  GUI_MEMDEV_Delete(hDst);
  return 0;
}

/*********************************************************************
*
*       _ShiftImage
*/
static int _ShiftImage(int CleanUp) {
  static GUI_MEMDEV_Handle    hMemHQ[GUI_COUNTOF(_aImage)];
  static GUI_MEMDEV_Handle    hDstEnlarge;
  static GUI_MEMDEV_Handle    hLCDEnlarge;
  static GUI_MEMDEV_Handle    hDstShift;
  static GUI_MEMDEV_Handle    hLCD;
  static int                  Index;
  IMAGE                     * pImage;
  void                     (* pfRotate)(GUI_MEMDEV_Handle hSrc, GUI_MEMDEV_Handle hDst, int dx, int dy, int a, int Mag);
  int                         xSizeImageMag;
  int                         ySizeImageMag;
  int                         TimeContinue;
  int                         DispSpaceX;
  int                         DispSpaceY;
  int                         TimeNow;
  int                         Index0;
  int                         Index1;
  int                         Index2;
  int                         Index3;
  int                         xSize;
  int                         ySize;
  int                         Step;
  int                         Add;
  int                         Mag;
  int                         a;
  int                         i;

  xSize      = LCD_GetXSize();
  ySize      = LCD_GetYSize() - TITLE_SIZE;
  DispSpaceX = xSize / 3;
  DispSpaceY = (ySize * 3) / 5;
  //
  // Create memory devices if not existing
  //
  if (hLCD == 0) {
    hLCD        = GUI_MEMDEV_CreateFixed(0,         TITLE_SIZE + ySize / 5, xSize,           DispSpaceY, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV);
    if (hLCD == 0) {
      return 1;
    }
    _CopyFromLCD(hLCD);
  }
  if (hDstShift == 0) {
    hDstShift   = GUI_MEMDEV_CreateFixed(0,         TITLE_SIZE + ySize / 5, xSize,           DispSpaceY, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV);
    if (hDstShift == 0) {
      GUI_MEMDEV_Delete(hLCD);
      return 1;
    }
  }
  if (hDstEnlarge == 0) {
    hDstEnlarge = GUI_MEMDEV_CreateFixed(xSize / 6, TITLE_SIZE,             (xSize * 2) / 3, ySize,      GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV);
    if (hDstEnlarge == 0) {
      GUI_MEMDEV_Delete(hLCD);
      GUI_MEMDEV_Delete(hDstShift);
      return 1;
    }
  }
  if (hLCDEnlarge == 0) {
    hLCDEnlarge = GUI_MEMDEV_CreateFixed(xSize / 6, TITLE_SIZE,             (xSize * 2) / 3, ySize,      GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV);
    if (hLCDEnlarge == 0) {
      GUI_MEMDEV_Delete(hLCD);
      GUI_MEMDEV_Delete(hDstShift);
      GUI_MEMDEV_Delete(hDstEnlarge);
      return 1;
    }
  }
  GUI_MEMDEV_Select(0);
  for (i = 0; i < _NumImages; i++) {
    if (hMemHQ[i] == 0) {
      pImage        = &_aImage[i];
      xSizeImageMag = pImage->xSize * MAG / 1000;
      ySizeImageMag = pImage->ySize * MAG / 1000;
      hMemHQ[i]     = GUI_MEMDEV_CreateFixed((DispSpaceX - xSizeImageMag) / 2 - BORDER_SIZE, TITLE_SIZE + ySize / 5 + (DispSpaceY - ySizeImageMag) / 2 - BORDER_SIZE, xSizeImageMag + BORDER_SIZE * 2, ySizeImageMag + BORDER_SIZE * 2, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV);
      _CopyFromLCD(hMemHQ[i]);
      GUI_MEMDEV_RotateHQ(pImage->hMem, hMemHQ[i], (xSizeImageMag - pImage->xSize) / 2 + BORDER_SIZE, (ySizeImageMag - pImage->ySize) / 2 + BORDER_SIZE, 0, MAG);
    }
  }
  //
  // Get image indices
  //
  Index0 = (Index + 0) % _NumImages;
  Index1 = (Index + 1) % _NumImages;
  Index2 = (Index + 2) % _NumImages;
  Index3 = (Index + 3) % _NumImages;
  //
  // Perform shifting
  //
  pfRotate = GUI_MEMDEV_Rotate;
  for (i = 1; i <= FINAL_STEP; i++) {
    TimeContinue = GUIDEMO_GetTime() + MIN_TIME_PER_FRAME_SHIFT;
    Step         = (DispSpaceX * i) / FINAL_STEP;
    //
    // Calculate rotation
    //
    a = (-Step * 360 * 1000) / DispSpaceX;
    //
    // Do animation
    //
    GUI_MEMDEV_Select(hDstShift);
    GUI_MEMDEV_Write(hLCD);
    if (Step == DispSpaceX) {
      pImage        = &_aImage[Index0];
      ySizeImageMag = pImage->ySize * MAG / 1000;
      xSizeImageMag = pImage->xSize * MAG / 1000;
      GUI_MEMDEV_WriteAt(hMemHQ[Index0], (DispSpaceX - xSizeImageMag) / 2            - BORDER_SIZE, TITLE_SIZE + ySize / 5 + (DispSpaceY - ySizeImageMag) / 2 - BORDER_SIZE);
      pImage        = &_aImage[Index1];
      ySizeImageMag = pImage->ySize * MAG / 1000;
      xSizeImageMag = pImage->xSize * MAG / 1000;
      GUI_MEMDEV_WriteAt(hMemHQ[Index1], (DispSpaceX - xSizeImageMag) / 2 + Step     - BORDER_SIZE, TITLE_SIZE + ySize / 5 + (DispSpaceY - ySizeImageMag) / 2 - BORDER_SIZE);
      pImage        = &_aImage[Index2];
      ySizeImageMag = pImage->ySize * MAG / 1000;
      xSizeImageMag = pImage->xSize * MAG / 1000;
      GUI_MEMDEV_WriteAt(hMemHQ[Index2], (DispSpaceX - xSizeImageMag) / 2 + Step * 2 - BORDER_SIZE, TITLE_SIZE + ySize / 5 + (DispSpaceY - ySizeImageMag) / 2 - BORDER_SIZE);
    } else {
      pImage        = &_aImage[Index0];
      pfRotate(pImage->hMem, hDstShift, ((xSize - pImage->xSize) / 2) - (DispSpaceX * 2) + Step, (DispSpaceY - pImage->ySize) / 2, a, MAG);
      pImage        = &_aImage[Index1];
      pfRotate(pImage->hMem, hDstShift, ((xSize - pImage->xSize) / 2) -  DispSpaceX      + Step, (DispSpaceY - pImage->ySize) / 2, a, MAG);
      pImage        = &_aImage[Index2];
      pfRotate(pImage->hMem, hDstShift, ((xSize - pImage->xSize) / 2)                    + Step, (DispSpaceY - pImage->ySize) / 2, a, MAG);
      pImage        = &_aImage[Index3];
      pfRotate(pImage->hMem, hDstShift, ((xSize - pImage->xSize) / 2) +  DispSpaceX      + Step, (DispSpaceY - pImage->ySize) / 2, a, MAG);
    }
    GUI_MULTIBUF_Begin();
    GUI_MEMDEV_CopyToLCD(hDstShift);
    GUI_MULTIBUF_End();
    //
    // Delay
    //
    TimeNow = GUIDEMO_GetTime();
    if (TimeContinue > TimeNow) {
      GUI_Delay(TimeContinue - TimeNow);
    } else {
      GUI_Exec();
    }
    if (GUIDEMO_CheckCancel()) {
      GUI_MEMDEV_Delete(hLCD);
      GUI_MEMDEV_Delete(hDstShift);
      GUI_MEMDEV_Delete(hDstEnlarge);
      GUI_MEMDEV_Delete(hLCDEnlarge);
      hLCD        = 0;
      hDstShift   = 0;
      hDstEnlarge = 0;
      hLCDEnlarge = 0;
      return 1;
    }
  }
  //
  // Enlarge & shrink
  //
  pImage = &_aImage[Index1];
  _CopyFromLCD(hLCDEnlarge);
  GUI_MEMDEV_Select(hLCDEnlarge);
  GUI_DrawGradientV(DispSpaceX, TITLE_SIZE, DispSpaceX * 2 - 1, ySize + TITLE_SIZE - 1, GUI_WHITE, GUI_LIGHTBLUE);
  GUI_MEMDEV_Select(hDstEnlarge);
  Add = 1;
  Mag = MAG;
  i   = 1;
  do {
    TimeContinue = GUIDEMO_GetTime() + MIN_TIME_PER_FRAME_ENLARGE;
    //
    // Use HQ version only for last step
    //
    if (i == 0) {
      pfRotate = GUI_MEMDEV_RotateHQ;
    } else {
      pfRotate = GUI_MEMDEV_Rotate;
    }
    //
    // Calculate magnification
    //
    if (Add > 0) {
      Mag = (Mag * 1050) / 1000;
    } else {
      Mag = (Mag * 1000) / 1050;
    }
    if (Mag < MAG) {
      Mag = MAG;
    }
    //
    // Do animation
    //
    GUI_MEMDEV_Write(hLCDEnlarge);
    pfRotate(pImage->hMem, hDstEnlarge, (((xSize * 2) / 3) - pImage->xSize) / 2, (ySize - pImage->ySize) / 2, 0, Mag);
    GUI_MULTIBUF_Begin();
    GUI_MEMDEV_CopyToLCD(hDstEnlarge);
    GUI_MULTIBUF_End();
    i += Add;
    if (i == 20) {
      Add = -Add;
    }
    //
    // Delay
    //
    TimeNow = GUIDEMO_GetTime();
    if (TimeContinue > TimeNow) {
      GUI_Delay(TimeContinue - TimeNow);
    }
    if (GUIDEMO_CheckCancel()) {
      GUI_MEMDEV_Delete(hLCD);
      GUI_MEMDEV_Delete(hDstShift);
      GUI_MEMDEV_Delete(hDstEnlarge);
      GUI_MEMDEV_Delete(hLCDEnlarge);
      GUI_MEMDEV_Select(0);
      hLCD        = 0;
      hDstShift   = 0;
      hDstEnlarge = 0;
      hLCDEnlarge = 0;
      return 1;
    }
  } while (i > 0);
  if (Index == 0) {
    Index = _NumImages - 1;
  } else {
    Index--;
  }
  //
  // Remove memory devices (if required)
  //
  if (CleanUp) {
    GUI_MEMDEV_Delete(hLCD);
    GUI_MEMDEV_Delete(hDstShift);
    GUI_MEMDEV_Delete(hDstEnlarge);
    GUI_MEMDEV_Delete(hLCDEnlarge);
    GUI_MEMDEV_Select(0);
    hLCD        = 0;
    hDstShift   = 0;
    hDstEnlarge = 0;
    hLCDEnlarge = 0;
  }
  return 0;
}

/*********************************************************************
*
*       _DemoZoomAndRotate
*/
static void _DemoZoomAndRotate(void) {
  int FontDistY;
  int xSize;
  int ySize;
  int i;

  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  //
  // Get images
  //
  for (i = 0; i < _NumImages; i++) {
    if (_GetImage(&_aImage[i])) {
      return;
    }
  }
  //
  // Set background and headline
  //
  GUI_DrawGradientV(0, 0,          xSize - 1, TITLE_SIZE - 1, GUI_LIGHTBLUE, GUI_WHITE);
  GUI_DrawGradientV(0, TITLE_SIZE, xSize - 1, ySize      - 1, GUI_WHITE,     GUI_LIGHTBLUE);
  GUI_DrawBitmap(&bmMicriumLogoFrame69x32, 6, (TITLE_SIZE - bmMicriumLogoFrame69x32.YSize) / 2);
  GUI_SetTextMode(GUI_TM_TRANS);
  GUI_SetFont(&GUI_FontRounded22);
  FontDistY = GUI_GetFontDistY();
  GUI_SetColor(GUI_BLACK);
  GUI_DispStringHCenterAt("Zoom and rotate", 62 + (xSize - 62) / 2,  (TITLE_SIZE - FontDistY) / 2);
  if (_RotateAndZoomImage() == 0) {
    if (_ShiftImage(0) == 0) {
      if (_ShiftImage(0) == 0) {
        _ShiftImage(1);
      }
    }
  }
  //
  // Remove memory devices
  //
  for (i = 0; i < _NumImages; i++) {
    GUI_MEMDEV_Delete(_aImage[i].hMem);
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIDEMO_ZoomAndRotate
*/
void GUIDEMO_ZoomAndRotate(void) {
  GUIDEMO_ConfigureDemo("Zoom and rotate", "Zooming and rotating any\nkind of images with uC/GUI.", GUIDEMO_SHOW_CURSOR | GUIDEMO_SHOW_CONTROL);
  _DemoZoomAndRotate();
}

#else

void GUIDEMO_ZoomAndRotate_C(void);
void GUIDEMO_ZoomAndRotate_C(void) {}

#endif  // SHOW_GUIDEMO_ZOOMANDROTATE && GUI_WINSUPPORT && GUI_SUPPORT_MEMDEV

/*************************** End of file ****************************/
