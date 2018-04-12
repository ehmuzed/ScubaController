/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*
*                                         STM32F746G-DISCO
*                                         Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu_core.h>
#include  <app_cfg.h>

#include  "bsp.h"
#include  "bsp_clock.h"
#include  "bsp_led.h"
#include  "bsp_pb.h"
#include  "bsp_test.h"
#include  "bsp_os.h"

#include  "stm32f7xx_hal.h"

#if (APP_CFG_GUI_EN > 0u)
#include  "bsp_touch.h"
#include  "stm32746g_discovery_sdram.h"
#endif


/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    // Initialize OS periodic time source
    BSP_OSTickInit();

#if (APP_CFG_GUI_EN > 0u)
    // ------------------ GUI REQUIREMENT ----------------- 
    // Must initialize SDRAM and CRC Module
    BSP_SDRAM_Init();
    __HAL_RCC_CRC_CLK_ENABLE();

    // ----------------- INIT TOUCH SCREEN ----------------
    // Initialize the LCD's Touch Capabilities.
    BSP_Touch_Init();
#endif

  BSP_LED_Init();            // LEDs
  BSP_Test_Outputs_Init();   // Test point outputs
  BSP_PB_Init();             // Pushbutton

}




// **********************************************************
/*!
* @brief Initialize a GPIO port's pin as a test output.
* @param[in] PortBaseAddr  GPIO port base address
* @param[in] Pin  The pin number (0-15) to configure
*/
void BSP_GPIO_Init_Output(uint32_t PortBaseAddr, uint32_t Pin)
{
  volatile uint32_t temp;

  // Param checking
  my_assert(PortBaseAddr >= 0x40020000UL);
  my_assert(PortBaseAddr < 0x40023000UL);
  my_assert(Pin <= 15);

  const uint32_t ThePin = Pin;

  // Configure GPIO as Output Pin
  temp  = REG32(GPIO_MODER(PortBaseAddr));
  temp &= ~(0x03UL << (ThePin * 2UL));
  temp |= (0x01UL << (ThePin * 2UL));  // output
  REG32(GPIO_MODER(PortBaseAddr)) = temp;

  // Output speed
  temp  = REG32(GPIO_SPEEDR(PortBaseAddr));
  temp &= ~(0x03UL << (ThePin * 2UL));
  temp |= (0x02UL << (ThePin * 2UL));  // High Speed
  REG32(GPIO_SPEEDR(PortBaseAddr)) = temp;

  // IO Output type - Push Pull (not open drain)
  temp  = REG32(GPIO_OTYPER(PortBaseAddr));
  temp &= ~(0x01UL << ThePin);
  temp |=  (0x00UL << ThePin);  // Push Pull
  REG32(GPIO_OTYPER(PortBaseAddr)) = temp;

  // Pull Up / Pull Down 
  temp  = REG32(GPIO_PUPDR(PortBaseAddr));
  temp &= ~(0x03UL << (ThePin * 2UL));
  temp |= (0x00UL << (ThePin * 2UL));  // No Pull Up or Pull Down
  REG32(GPIO_PUPDR(PortBaseAddr)) = temp;

}


// **********************************************************
/*!
* @brief Initialize a GPIO port's pin as an input.
* @param[in] PortBaseAddr  GPIO port base address
* @param[in] Pin  The pin number (0-15) to configure
*/
void BSP_GPIO_Init_Input(uint32_t PortBaseAddr, uint32_t Pin)
{
  volatile uint32_t temp;

  // Param checking
  my_assert(PortBaseAddr >= 0x40020000UL);
  my_assert(PortBaseAddr < 0x40023000UL);
  my_assert(Pin <= 15);

  const uint32_t ThePin = Pin;

  // Configure GPIO as Input Pin
  temp  = REG32(GPIO_MODER(PortBaseAddr));
  temp &= ~(0x03UL << (ThePin * 2UL));
  temp |= (0x00UL << (ThePin * 2UL));  // input
  REG32(GPIO_MODER(PortBaseAddr)) = temp;

  // Pin speed
  temp  = REG32(GPIO_SPEEDR(PortBaseAddr));
  temp &= ~(0x03UL << (ThePin * 2UL));
  temp |= (0x02UL << (ThePin * 2UL));  // High Speed
  REG32(GPIO_SPEEDR(PortBaseAddr)) = temp;

  // Pull Up / Pull Down 
  temp  = REG32(GPIO_PUPDR(PortBaseAddr));
  temp &= ~(0x03UL << (ThePin * 2UL));
  temp |= (0x00UL << (ThePin * 2UL));  // No Pull Up or Pull Down
  REG32(GPIO_PUPDR(PortBaseAddr)) = temp;

}


// **********************************************************
/*!
* @brief Set a GPIO port's pin to 0 or 1
* @param[in] PortBaseAddr  GPIO port base address
* @param[in] Pin  The pin number (0-15) to change
*/
void BSP_GPIO_WritePin(uint32_t PortBaseAddr, uint32_t Pin, uint32_t Val)
{
  my_assert(PortBaseAddr >= 0x40020000UL);
  my_assert(PortBaseAddr < 0x40023000UL);
  my_assert(Pin <= 15);
  Pin += (Val ? 0 : 16UL);  // Reset bits are upper 16 bits of register
  REG32(GPIO_BSRR(PortBaseAddr)) = (1UL << Pin); // Set or reset pin!
}

// **********************************************************
/*!
* @brief Toggle a GPIO output pin
* @param[in] PortBaseAddr  GPIO port base address
* @param[in] Pin  The pin number (0-15) to toggle
*/
void BSP_GPIO_TogglePin(uint32_t PortBaseAddr, uint32_t Pin)
{
  my_assert(PortBaseAddr >= 0x40020000UL);
  my_assert(PortBaseAddr < 0x40023000UL);
  my_assert(Pin <= 15);
  REG32(GPIO_ODR(PortBaseAddr)) ^= (1UL << Pin); // Toggle Pin x
}


/*!
 * @brief Called when a home-made assertion (my_assert()) fails.
 */
// *****************************************************************
void assert_failed(void)
{ 
  // DEBUG: Set breakpoint here; call stack shows history/trace
  // RELEASE: Log, then reset
  while (1)
  {
  }
}

