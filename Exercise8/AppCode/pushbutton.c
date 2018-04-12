/** \file pushbutton.c
*
* @brief Button Debouncer
*
* @par
* COPYRIGHT NOTICE: (C) 2017 Barr Group, LLC.
* All rights reserved.
*/

#include <stdint.h>
#include <stdbool.h>

#include "project.h"
#include "os.h"
#include "bsp_pb.h"                                
#include "pushbutton.h"                           
// State machine functions
struct tag_sm;   // Forward declaration
typedef void (*ee_func)(void);
typedef struct tag_sm const * (*state_func)(uint32_t ev);

// Struct for /Simple/ State Machine "state".
typedef struct tag_sm {
  ee_func    entry_action;
  state_func event_handler;
  ee_func    exit_action;
} SIMPLE_STATE;

#define CHANGE_STATE(x) (&(x))
#define NO_STATE_CHANGE()  ((void *)0)

static uint32_t ticksInState = 0UL;
#define  RESET_STATE_TICKS() do { ticksInState = 0UL; } while (0)

// Allocate Shared OS Objects
OS_SEM      g_sw1_sem;
OS_SEM      g_sw2_sem;


// ******  PROTOTYPES           ****************
static void eeDummy(void);
static SIMPLE_STATE const * eventHandlerIdle(uint32_t ev);
static SIMPLE_STATE const * eventHandlerPending(uint32_t ev);
static SIMPLE_STATE const * eventHandlerSingle(uint32_t ev);
static SIMPLE_STATE const * eventHandlerRepeating(uint32_t ev);
static void entrySingle(void);
static void entryRepeating(void);

// *********************************************
static const SIMPLE_STATE stateIdle = {
  eeDummy, 
  eventHandlerIdle, 
  eeDummy 
};

// *********************************************
static const SIMPLE_STATE statePending = {
  eeDummy, 
  eventHandlerPending, 
  eeDummy 
};

// *********************************************
static const SIMPLE_STATE stateSingle = {
  entrySingle, 
  eventHandlerSingle, 
  eeDummy 
};

// *********************************************
static const SIMPLE_STATE stateRepeating = {
  entryRepeating, 
  eventHandlerRepeating, 
  eeDummy 
};

// *********************************************
static SIMPLE_STATE const * eventHandlerIdle(uint32_t ev)
{
  if (ev)
  {
    // Button pressed
    return CHANGE_STATE(statePending);
  }
  return NO_STATE_CHANGE();
} 

// *********************************************
static SIMPLE_STATE const * eventHandlerPending(uint32_t ev)
{
  if (ev)
  {
    // Button still pressed
    return CHANGE_STATE(stateSingle);
  }
  return CHANGE_STATE(stateIdle);
} 

// *********************************************
static SIMPLE_STATE const * eventHandlerSingle(uint32_t ev)
{
  if (ev)
  {
    // Button pressed long enough for repeat?
    if (++ticksInState >= 10UL)
    {
      return CHANGE_STATE(stateRepeating);
    }
    return NO_STATE_CHANGE();  // Not yet...
  }
  // Pushbutton was pressed long enough to register,
  // but too short to repeat.  Count it as a single press.
  OS_ERR err;
  OSSemPost(&g_sw1_sem, OS_OPT_POST_1, &err);
  //toggle metric
  g_scuba_data.b_is_metric = g_scuba_data.b_is_metric == 0 ? 1 : 0;
  my_assert(OS_ERR_NONE == err);
  return CHANGE_STATE(stateIdle);
} 

// *********************************************
static SIMPLE_STATE const * eventHandlerRepeating(uint32_t ev)
{
  if (ev)
  {
    // Every 4th time through here, signal a "repeat"
    ++ticksInState;
    if ((ticksInState & 0x03UL) == 0UL) 
    {
      OS_ERR err;
      // Signal that the pushbutton is still pressed.
      OSSemPost(&g_sw2_sem, OS_OPT_POST_1, &err);
      my_assert(OS_ERR_NONE == err);
    }
    return NO_STATE_CHANGE();
  }
  return CHANGE_STATE(stateIdle);
}

// *********************************************
static void eeDummy(void)
{
  // Empty entry / exit action
} 

// *********************************************
static void entrySingle(void)
{
  ticksInState = 0;
} 

// *********************************************
static void entryRepeating(void)
{
  ticksInState = 0;
  // Signal that the pushbutton is still pressed.
  OS_ERR err;
  OSSemPost(&g_sw2_sem, OS_OPT_POST_1, &err);
  my_assert(OS_ERR_NONE == err);
} 

// The button state machine's current state - 
// represented by a pointer to a struct, which holds
// function pointers to entry and exit actions for the state,
// as well as the state's event handler "callback" function.
SIMPLE_STATE const * buttonState = &stateIdle;

/*!
*
* @brief Create debounce-related OS objects
*/
void debounce_task_init(void)
{
  OS_ERR err;
  // Create the semaphores signaled by the button debouncer.
  OSSemCreate(&g_sw1_sem, "Switch 1", 0, &err);
  my_assert(OS_ERR_NONE == err);

  OSSemCreate(&g_sw2_sem, "Switch 2", 0, &err);
  my_assert(OS_ERR_NONE == err);
}

/*!
*
* @brief Button Debounce Task
*/
void debounce_task(void * p_arg)
{
  OS_ERR err;
  (void)p_arg;    // NOTE: Silence compiler warning about unused param.

  for (;;)
  {
    // Delay for 50 ms.
    OSTimeDlyHMSM(0, 0, 0, 50UL, OS_OPT_TIME_HMSM_STRICT, &err);
    my_assert(OS_ERR_NONE == err);
    
    // Read the current state of the pushbutton.
    bool button1_pressed = (bool)BSP_PB_Read();

    // Execute switch debouncing state machine.
    SIMPLE_STATE const * next_state = buttonState->event_handler(button1_pressed);

    // See if there is a state change
    if (next_state)
    {
      // Exit the current state
      buttonState->exit_action();   // run exit action of current state

      // Enter the new state
      buttonState = next_state;    // change state
      buttonState->entry_action();  // run entry action of next state
    }
  }
}

