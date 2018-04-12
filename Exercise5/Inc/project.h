
/** \file project.h
*
* @brief Project-wide definitions
*
* @par
* COPYRIGHT NOTICE: (c) Barr Group, LLC.
* All rights reserved.
*/

#ifndef __PROJECT_H
#define __PROJECT_H

#include <stdint.h>     // uint32_t, etc...
#include <intrinsics.h> // __DSB(), __DMB(), __ISB(), interrupt control...

// Variable which holds the system (core CPU) clock speed (Hz)
extern void     assert_failed(void);

// Home-made assertion - avoid linking in std library's own implementation
#define my_assert(expr) \
  do { \
    if (!(expr)) { \
      assert_failed(); \
    } \
  } while (0)

#endif /* __PROJECT_H */


