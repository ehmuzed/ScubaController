
/** \file bsp_test.c
*
* @brief Board Support Test Output
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Barr Group, LLC.
* All rights reserved.
*/


#include "project.h"
#include "bsp.h"
#include "bsp_test.h"

// Test Outputs - PF.6 through PF.9
#define TESTOUT_GPIO_PORT_CLK_ENABLE()   GPIOF_CLK_ENABLE()
#define TEST_OUT_PIN6         (6UL)   // PF.6
#define TEST_OUT_PIN7         (7UL)   // PF.7
#define TEST_OUT_PIN8         (8UL)   // PF.8
#define TEST_OUT_PIN9         (9UL)   // PF.9
#define TEST_OUT_PIN_FIRST    (TEST_OUT_PIN6)
#define TEST_OUT_PIN_LAST     (TEST_OUT_PIN9)
#define TEST_OUT_PORT         (GPIO_PORT_F_BASE_ADDR)   // Port F


// **********************************************************
/**
  * @brief  Init a few GPIOs as outputs for probing / testing / debugging
  * @note Assumes PF6 - PF9 unused
  * @retval None
  */
void BSP_Test_Outputs_Init(void)
{
  // Enable the Test Output Port's GPIO clock
  TESTOUT_GPIO_PORT_CLK_ENABLE();

  for (uint32_t pin = TEST_OUT_PIN_FIRST; pin <= TEST_OUT_PIN_LAST; ++pin)
  {
    // Configure the GPIO pin for output
    BSP_GPIO_Init_Output(TEST_OUT_PORT, pin);

    // Init pin to OFF / LOW / RESET
    BSP_GPIO_WritePin(TEST_OUT_PORT, pin, 0);
  }
}


// **********************************************************
/**
  * @brief  Drive a test output high
  * @param  id: output to be driven (6-9)
  * @note Assumes PF6 - PF9 used
  * @retval None
  */
void BSP_Test_Output_On(TEST_OUTPUT_ID id)
{
  my_assert(id >= TEST_OUTPUT_1);
  my_assert(id <= TEST_OUTPUT_4);
  BSP_GPIO_WritePin(TEST_OUT_PORT, id, 1);
}

// **********************************************************
/**
  * @brief  Drive a test output low
  * @param  id: output to be driven (6-9)
  * @note Assumes PF6 - PF9 used
  * @retval None
  */
void BSP_Test_Output_Off(TEST_OUTPUT_ID id)
{
  my_assert(id >= TEST_OUTPUT_1);
  my_assert(id <= TEST_OUTPUT_4);
  BSP_GPIO_WritePin(TEST_OUT_PORT, id, 0);
}

// **********************************************************
/**
  * @brief  Toggle a test output
  * @param  id: output to be driven (6-9)
  * @note Assumes PF6 - PF9 used
  * @retval None
  */
void BSP_Test_Output_Toggle(TEST_OUTPUT_ID id)
{
  my_assert(id >= TEST_OUTPUT_1);
  my_assert(id <= TEST_OUTPUT_4);
  BSP_GPIO_TogglePin(TEST_OUT_PORT, id);
}

