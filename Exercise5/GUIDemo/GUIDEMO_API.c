
/** \file GUIDEMO_API.c
*
* @brief Public API to manipulate GUI
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Barr Group, LLC.
* All rights reserved.
*/


#include "project.h"
#include "os.h"

#include "GUIDEMO.h"
#include "GUIDEMO_API.h"
#include "GUI.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>   // rand()

#define GUI_TEXT_LINE_COUNT     (8UL)  // 8 text lines
#define GUI_TEXT_LINE_LEN_MAX  (31UL)  // max chars per line, incl. NULL


// Private mutex. Ensures all accesses to display data
// are mutually exclusive (protected).
// By making the mutex private, we ensure application code
// must use the public API.
static OS_MUTEX  s_gui_mutex;    	

// Single linear buffer to hold all lines of text
static char gui_text_buffer[(GUI_TEXT_LINE_COUNT) * (GUI_TEXT_LINE_LEN_MAX)];

// Holds 2 colors for background (BG) vertical gradient.
static uint32_t  s_BG_Color0 = 0x00FFFFFF;
static uint32_t  s_BG_Color1 = 0x00000000;

// Flag necessary since task initialization order depends on
// other tasks and their priorities
static bool gui_ready = false;

// **********************************************************
/*!
* @brief Initialize the GUI.
*/
void GUIDEMO_API_init(void)
{
   OS_ERR err;

  if (!gui_ready)
  {
     // Create the mutex that protects the display from race conditions.
     OSMutexCreate(&s_gui_mutex, "GUI Text Mutex", &err);
     my_assert(OS_ERR_NONE == err);
  
     // Zero out display buffer
     memset(gui_text_buffer, 0, sizeof(gui_text_buffer));

     gui_ready = true;
  }
}

// **********************************************************
/*!
* @brief Return data member
*/
uint32_t GUIDEMO_GetColorBG0(void)
{
  return s_BG_Color0;
}

// **********************************************************
/*!
* @brief Return data member
*/
uint32_t GUIDEMO_GetColorBG1(void)
{
  return s_BG_Color1;
}

// **********************************************************
/*!
* @brief Set background gradient color
* @param Enum indicating which color
*/
void GUIDEMO_SetColorBG(BG_COLOR newColorBG)
{
  OS_ERR err;

  // Init GUI if not already; safe to call even if already initialized.
  GUIDEMO_API_init();

  // Acquire the GUI mutex.
  OSMutexPend(&s_gui_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
  my_assert(OS_ERR_NONE == err);

  switch(newColorBG)
  {
    case BG_COLOR_RED:
      s_BG_Color0 = BK_RED_LIGHT;
      s_BG_Color1 = BK_RED_DARK;
      break;
    case BG_COLOR_ORANGE:
      s_BG_Color0 = BK_ORANGE_LIGHT;
      s_BG_Color1 = BK_ORANGE_DARK;
      break;
    case BG_COLOR_YELLOW:
      s_BG_Color0 = BK_YELLOW_LIGHT;
      s_BG_Color1 = BK_YELLOW_DARK;
      break;
    case BG_COLOR_GREEN:
      s_BG_Color0 = BK_GREEN_LIGHT;
      s_BG_Color1 = BK_GREEN_DARK;
      break;
    case BG_COLOR_BLUE:
      s_BG_Color0 = BK_BLUE_LIGHT;
      s_BG_Color1 = BK_BLUE_DARK;
      break;
    default:
      my_assert(0);  // Should not happen!
      break;
  }

  // Release the GUI mutex.
  OSMutexPost(&s_gui_mutex, OS_OPT_POST_NONE, &err);
  my_assert(OS_ERR_NONE == err);

}


/**
  * @brief  Set a line of text.  Should be NULL terminated,
  *          otherwise it will be truncated.
  * @param  Line - 0-based line number (0 = topmost line)
  * @param  Text - pointer to string to write / copy to GUI.
  * @retval None
  */
void GUIDEMO_API_writeLine(uint32_t Line, char const * Text)
{
  OS_ERR   err;

  my_assert(Line < GUI_TEXT_LINE_COUNT);

  // Point to first and last chars of this line's region in buffer
  char * first = gui_text_buffer + (Line * GUI_TEXT_LINE_LEN_MAX);
  char * last  = gui_text_buffer + (((Line + 1) * GUI_TEXT_LINE_LEN_MAX) - 1);

  // Init GUI if not already; safe to call even if already initialized.
  GUIDEMO_API_init();

  // Acquire the GUI mutex.
  OSMutexPend(&s_gui_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
  my_assert(OS_ERR_NONE == err);

  // Copy text into display buffer.  Ensure we don't copy too much.
  strncpy(first, Text, (GUI_TEXT_LINE_LEN_MAX - 1));
  *last = 0;  // Force / ensure NULL termination

  // Release the GUI mutex.
  OSMutexPost(&s_gui_mutex, OS_OPT_POST_NONE, &err);
  my_assert(OS_ERR_NONE == err);

}

/**
  * @brief  Refresh GUI with text buffer
  * @retval None
  */
void GUIDEMO_API_refresh(void)
{
  OS_ERR   err;

  const unsigned y_delta = 28;
  const unsigned x_pos = 16;
  uint32_t  y_pos = y_delta;
  char temp[GUI_TEXT_LINE_LEN_MAX + 2];

  // Init GUI if not already; safe to call even if already initialized.
  GUIDEMO_API_init();

  // Draw the screen background
  GUIDEMO_DrawBk();

  // Copy text to display.
  for (uint32_t i = 0; i < GUI_TEXT_LINE_COUNT; ++i)
  {
    char * first = gui_text_buffer + (i * GUI_TEXT_LINE_LEN_MAX);

    // Acquire the GUI mutex.
    OSMutexPend(&s_gui_mutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    my_assert(OS_ERR_NONE == err);

    strncpy(temp, first, (GUI_TEXT_LINE_LEN_MAX - 1));

    // Release the GUI mutex.
    OSMutexPost(&s_gui_mutex, OS_OPT_POST_NONE, &err);
    my_assert(OS_ERR_NONE == err);

    temp[GUI_TEXT_LINE_LEN_MAX - 1] = 0;

    GUI_DispStringAt(temp, x_pos, y_pos);

    y_pos += y_delta;
  }


}

#if 0
/**
  * @brief  Private test function...
  * @retval None
  */
static void GUIDEMO_API_simpleTest(void)
{
  const char * const str1 = "This is Line 1";
  const char * const str2 = "THIS is Line 2";
  const char * const str3 = "this IS Line 3";
  const char * const str4 = "Line 4 is here.";
  const char * const str5 = "Over here! Line 5...";
  const char * const str6 = "Line 6";
  const char * const str7 = "Getting near the bottom - Line 7";
  const char * const str8 = "Last but not least... Line 8";

  // "texty" is array of 8 character pointers.
  // Shuffle the array each time, so that the order of lines is different each time.
  const char * texty[GUI_TEXT_LINE_COUNT] = { str1, str2, str3, str4, str5, str6, str7, str8 }; 
  const char ** the_first = texty;
  const char ** the_last  = texty + GUI_TEXT_LINE_COUNT;  // iterator - ONE PAST END
  const char * tempy;
  unsigned i, n;
  n = the_last - the_first;
  for (i = (n - 1); i ; --i)
  {
    int rand_number = (rand() % (i + 1));
    tempy = the_first[i];
    the_first[i] = the_first[rand_number];
    the_first[rand_number] = tempy;
  }

  // Send strings to GUI
  for (i = 0; i < GUI_TEXT_LINE_COUNT; ++i)
  {
    GUIDEMO_API_writeLine(i, texty[i]);
  }

  // Change GUI screen color
  static BG_COLOR theColor = BG_COLOR_RED;
  GUIDEMO_SetColorBG(theColor);
  theColor = (BG_COLOR)((theColor == BG_COLOR_BLUE) ? BG_COLOR_RED : (theColor + 1));

}

#endif

