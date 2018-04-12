#ifndef  __BSP_LED_H_
#define  __BSP_LED_H_



#ifdef __cplusplus
extern  "C" {
#endif

// Enumeration for LEDs
// LED1 = on-board LED
// LED2 = external LED
typedef enum {
  LED1, LED2
} BOARD_LED_ID;

// LED API
extern void  BSP_LED_Init  (void);
extern void  BSP_LED_On    (BOARD_LED_ID  led);
extern void  BSP_LED_Off   (BOARD_LED_ID  led);
extern void  BSP_LED_Toggle(BOARD_LED_ID  led);


#ifdef __cplusplus
}
#endif

#endif

