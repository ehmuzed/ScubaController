
#ifndef  __BSP_TEST_H_
#define  __BSP_TEST_H_


#ifdef __cplusplus
extern  "C" {
#endif

// Enumeration for Test Outputs
typedef enum {
  TEST_OUTPUT_1 = 6UL,
  TEST_OUTPUT_2 = 7UL,
  TEST_OUTPUT_3 = 8UL,
  TEST_OUTPUT_4 = 9UL,
} TEST_OUTPUT_ID;

// PB API
extern void BSP_Test_Outputs_Init(void);
extern void BSP_Test_Output_On(TEST_OUTPUT_ID id);
extern void BSP_Test_Output_Off(TEST_OUTPUT_ID id);
extern void BSP_Test_Output_Toggle(TEST_OUTPUT_ID id);

#ifdef __cplusplus
}
#endif

#endif

