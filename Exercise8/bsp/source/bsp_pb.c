
/** \file bsp_pb.c
*
* @brief PushButton BSP
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Barr Group, LLC.
* All rights reserved.
*/


#include "project.h"
#include "bsp.h"
#include "bsp_pb.h"

// Push Buttons (PB)
// PB1 - GPIO PI.11
#define PB1_GPIO_PORT_CLK_ENABLE()   GPIOI_CLK_ENABLE()
#define PB1_PIN               (11UL)  // PI.11
#define PB1_GPIO_PORT         (GPIO_PORT_I_BASE_ADDR)


// **********************************************************
/**
  * @brief  Initialize the board's push button
  * @retval None
  */
void BSP_PB_Init(void)
{
  // PB is GPIOI.11
  // Enable clock for GPIO port I
  PB1_GPIO_PORT_CLK_ENABLE();

  // Configure the Pushbutton GPIO
  BSP_GPIO_Init_Input(PB1_GPIO_PORT, PB1_PIN);
}

// **********************************************************
/**
  * @brief  Read the board's push button
  * @retval 1 = pressed, 0 = not pressed
  */
uint32_t BSP_PB_Read(void)
{
  // PB is GPIOI.11
  const uint32_t ThePin = 11UL;
  // Button input goes LOW when pressed
  return ((REG32(GPIO_IDR(PB1_GPIO_PORT)) >> ThePin) & 0x01UL);
}

