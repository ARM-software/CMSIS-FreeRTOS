 /*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2015-2025 Cadence Design Systems, Inc.
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * Xtensa interrupt handling data and assembly routines.
 * Also see xtensa_intr.c and xtensa_vectors.S.
 */

#include <xtensa/hal.h>
#include <xtensa/config/core.h>

#include <FreeRTOSConfig.h>

#include "xtensa_context.h"
#include "xtensa_config.h"

#if XCHAL_HAVE_INTERRUPTS

/*
-------------------------------------------------------------------------------
  INTENABLE virtualization information.

  Moved to _xt_intdata structure for better cache performance
-------------------------------------------------------------------------------
*/

/*
-------------------------------------------------------------------------------
  System interrupt stack.

  For SMP configurations, (configNUMBER_OF_CORES * configISR_STACK_SIZE) bytes
  are allocated.  Avoid placing this in per-core memory (usually dataram) as 
  that is a valuable resource.
-------------------------------------------------------------------------------
*/

#if (XCHAL_HAVE_XEA2 || XCHAL_HAVE_ISB)
    .data
#else
    .section    .intr.top, "aw"
#endif

    .global xt_interrupt_stack
    .global xt_interrupt_stack_top
    .align  16

xt_interrupt_stack:
    .space  configISR_STACK_SIZE
xt_interrupt_stack_top:
#if ( configNUMBER_OF_CORES > 1 )
    .rept   configNUMBER_OF_CORES - 1
    .space  configISR_STACK_SIZE
    .endr
#endif


/*
-------------------------------------------------------------------------------
  Table of C-callable interrupt handlers for each interrupt. For XEA2 configs,
  not all slots can be filled, because interrupts at level > EXCM_LEVEL will
  not be dispatched to a C handler by default.

  NOTE: This is not replicated for SMP systems, so registering an interrupt 
  handler on one core will register the same handler for that interrupt on all
  cores.  However, Interrupts are enabled and disabled on a per-core basis.
-------------------------------------------------------------------------------
*/

#if (XCHAL_HAVE_XEA2 || XCHAL_HAVE_ISB)
    .data
#else
    .section    .intr.data, "aw"
#endif

    .global _xt_interrupt_table
    .align  16

_xt_interrupt_table:

/*
-------------------------------------------------------------------------------
  If using the interrupt wrapper, make the first entry in the interrupt table
  point to the wrapper (XEA3) or leave it empty (XEA2).
-------------------------------------------------------------------------------
*/
#if XCHAL_HAVE_XEA3
    .word   xt_interrupt_wrapper
    .word   0
#elif XT_USE_INT_WRAPPER
    .word   0
    .word   0
#endif

    .set    i, 0
    .rept   XCHAL_NUM_INTERRUPTS
    .word   xt_unhandled_interrupt      /* handler address               */
    .word   i                           /* handler arg (default: intnum) */
    .set    i, i+1
    .endr

#endif /* XCHAL_HAVE_INTERRUPTS */


#if XCHAL_HAVE_EXCEPTIONS

/*
-------------------------------------------------------------------------------
  Table of C-callable exception handlers for each exception. Note that not all
  slots will be active, because some exceptions (e.g. coprocessor exceptions)
  are always handled by the OS and cannot be hooked by user handlers.
-------------------------------------------------------------------------------
*/

    .data
    .global xt_exception_table
    .align  4

xt_exception_table:
    .rept   XCHAL_EXCCAUSE_NUM
    .word   xt_unhandled_exception    /* handler address */
    .endr

#endif
