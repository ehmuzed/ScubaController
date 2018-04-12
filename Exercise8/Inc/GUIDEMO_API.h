
/** \file GUIDEMO_API.h
*
* @brief GUI usage API
*
* @par
* COPYRIGHT NOTICE: (c) Barr Group, LLC.
* All rights reserved.
*/

#ifndef __GUIDEMO_API_H
#define __GUIDEMO_API_H

#include <stdint.h>

// Enumeration for valid background (BG) colors
typedef enum {
  BG_COLOR_RED = 1,
  BG_COLOR_ORANGE = 2,
  BG_COLOR_YELLOW = 3,
  BG_COLOR_GREEN = 4,
  BG_COLOR_BLUE = 5,
} BG_COLOR;

// Public (exported) API
extern void GUIDEMO_API_init(void);
extern void GUIDEMO_API_writeLine(uint32_t Line, char const * Text);
extern void GUIDEMO_API_refresh(void);
extern void GUIDEMO_SetColorBG(BG_COLOR newColorBG);
extern uint32_t GUIDEMO_GetColorBG0(void);
extern uint32_t GUIDEMO_GetColorBG1(void);


#endif /* __GUIDEMO_API_H */


