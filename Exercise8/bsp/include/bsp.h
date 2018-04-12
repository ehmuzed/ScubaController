
#ifndef  BSP_H_
#define  BSP_H_


#ifdef __cplusplus
extern  "C" {
#endif


#include "project.h"

#define SET_BIT(REG, BIT)   ((REG) |= (BIT))
#define SET_BITn(REG, n)    ((REG) |= (1UL << (n)))
#define CLEAR_BIT(REG, BIT) ((REG) &= ~(BIT))
#define CLEAR_BITn(REG, n)  ((REG) &= ~(1UL << (n)))
#define READ_BIT(REG, BIT)  ((REG) & (BIT))
#define READ_BITn(REG, n)   ((REG) & (1UL << (n))

// Macro for quick-and-dirty manipulation of registers
#define REG32(reg_addr)  ( * ( ( volatile uint32_t * ) (reg_addr) ) )
#define REG16(reg_addr)  ( * ( ( volatile uint16_t * ) (reg_addr) ) )
#define REG8(reg_addr)   ( * ( ( volatile  uint8_t * ) (reg_addr) ) )


// ENABLE CLOCK TO GPIO PORT
// Dummy read after register write is required because we need a
// delay after enabling RCC peripheral clock
#define GPIO_CLK_ENABLE(VAL) \
                                       do { \
                                        volatile uint32_t tmpreg; \
                                        SET_BIT((REG32(0x40023830)), (VAL));\
                                        tmpreg = READ_BIT((REG32(0x40023830)), (VAL));\
                                        (void)(tmpreg); \
                                      } while(0)


// Macros to enable clock for specific GPIO ports
#define GPIOA_CLK_ENABLE()   GPIO_CLK_ENABLE(1UL << 0UL)
#define GPIOF_CLK_ENABLE()   GPIO_CLK_ENABLE(1UL << 5UL)
#define GPIOI_CLK_ENABLE()   GPIO_CLK_ENABLE(1UL << 8UL)


// Base addresses of memory-mapped GPIO port peripherals
#define  GPIO_PORT_A_BASE_ADDR    (0x40020000UL)
#define  GPIO_PORT_F_BASE_ADDR    (0x40021400UL)
#define  GPIO_PORT_I_BASE_ADDR    (0x40022000UL)

// GPIO port register offsets
#define GPIO_MODER_OFFSET  (0x00UL)
#define GPIO_OTYPER_OFFSET (0x04UL)
#define GPIO_SPEEDR_OFFSET (0x08UL)
#define GPIO_PUPDR_OFFSET  (0x0CUL)
#define GPIO_IDR_OFFSET    (0x10UL)
#define GPIO_ODR_OFFSET    (0x14UL)
#define GPIO_BSRR_OFFSET   (0x18UL)


// GPIO port register addresses
#define GPIO_MODER(Port)   ((Port) + GPIO_MODER_OFFSET)
#define GPIO_OTYPER(Port)  ((Port) + GPIO_OTYPER_OFFSET)
#define GPIO_SPEEDR(Port)  ((Port) + GPIO_SPEEDR_OFFSET)
#define GPIO_PUPDR(Port)   ((Port) + GPIO_PUPDR_OFFSET)
#define GPIO_IDR(Port)     ((Port) + GPIO_IDR_OFFSET)
#define GPIO_ODR(Port)     ((Port) + GPIO_ODR_OFFSET)
#define GPIO_BSRR(Port)    ((Port) + GPIO_BSRR_OFFSET)


// *****************************************************************
// *****   External API / Function Prototypes                *******
// *****************************************************************
extern void  BSP_Init(void);
extern void  BSP_GPIO_Init_Input(uint32_t PortBaseAddr, uint32_t Pin);
extern void  BSP_GPIO_Init_Output(uint32_t PortBaseAddr, uint32_t Pin);
extern void  BSP_GPIO_WritePin(uint32_t PortBaseAddr, uint32_t Pin, uint32_t Val);
extern void  BSP_GPIO_TogglePin(uint32_t PortBaseAddr, uint32_t Pin);


#ifdef __cplusplus
}
#endif


#endif

