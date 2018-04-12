
/** \file bsp_led.c
*
* @brief STM32-F7 LED Driver
* @brief Embedded Software Boot Camp
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Barr Group, LLC.
* All rights reserved.
*/


#include  "bsp.h"
#include  "bsp_led.h"


// LED 1 - GPIO PI.1
#define LED1_GPIO_PORT_CLK_ENABLE()      GPIOI_CLK_ENABLE()
#define LED_1_PIN   (1UL)   // PI.1

// LED 2 - GPIO PA.0
#define LED2_GPIO_PORT_CLK_ENABLE()      GPIOA_CLK_ENABLE()
#define LED_2_PIN   (0UL)   // PA.0




// **********************************************************
/*!
* @brief Initialize the specified LED
* @param[in] Led  The LED number to be manipulated.
*/
static void BSP_LEDx_Init(BOARD_LED_ID Led)
{
  if (Led == LED1)
  {
    // Enable the LED's GPIO clock
    LED1_GPIO_PORT_CLK_ENABLE();

    // Configure the LED's GPIO pin for output
    BSP_GPIO_Init_Output(GPIO_PORT_I_BASE_ADDR, LED_1_PIN);
    
    // Init LED1 state to OFF
    BSP_GPIO_WritePin(GPIO_PORT_I_BASE_ADDR, LED_1_PIN, 0);
  }
  else if (Led == LED2)
  {
    // Enable the LED's GPIO clock
    LED2_GPIO_PORT_CLK_ENABLE();

    // Configure the LED's GPIO pin for output
    BSP_GPIO_Init_Output(GPIO_PORT_A_BASE_ADDR, LED_2_PIN);

    // Init LED2 state to OFF
    BSP_GPIO_WritePin(GPIO_PORT_A_BASE_ADDR, LED_2_PIN, 0);
  }
}

// **********************************************************
/*!
* @brief Initialize the board's LEDs
*/
void BSP_LED_Init()
{
  BSP_LEDx_Init(LED1);
  BSP_LEDx_Init(LED2);
}


// **********************************************************
/*!
* @brief Turn on specified LED
* @param[in] Led  The LED number to be manipulated.
*/
void BSP_LED_On(BOARD_LED_ID Led)
{
  if (Led == LED1)
  {
    BSP_GPIO_WritePin(GPIO_PORT_I_BASE_ADDR, LED_1_PIN, 1);  // PI1
  }
  else if (Led == LED2)
  {
    BSP_GPIO_WritePin(GPIO_PORT_A_BASE_ADDR, LED_2_PIN, 1);  // PA0
  }
}

// **********************************************************
/*!
* @brief Turn off specified LED
* @param[in] Led  The LED number to be manipulated.
*/
void BSP_LED_Off(BOARD_LED_ID Led)
{
  if (Led == LED1)
  {
    BSP_GPIO_WritePin(GPIO_PORT_I_BASE_ADDR, LED_1_PIN, 0);  // PI1
  }
  else if (Led == LED2)
  {
    BSP_GPIO_WritePin(GPIO_PORT_A_BASE_ADDR, LED_2_PIN, 0);  // PA0
  }
}

// **********************************************************
/*!
* @brief Toggle specified LED
* @param[in] Led  The LED number to be manipulated.
*/
void BSP_LED_Toggle(BOARD_LED_ID Led)
{
  if (Led == LED1)
  {
    BSP_GPIO_TogglePin(GPIO_PORT_I_BASE_ADDR, LED_1_PIN);    // Toggle PI1
  }
  else if (Led == LED2)
  {
    BSP_GPIO_TogglePin(GPIO_PORT_A_BASE_ADDR, LED_2_PIN);    // Toggle PA0
  }
}

