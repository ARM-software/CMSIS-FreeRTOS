 /*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2015-2024 Cadence Design Systems, Inc.
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
 * Xtensa-specific API for RTOS ports.
 */

#ifndef __XTENSA_API_H__
#define __XTENSA_API_H__

#include <stdint.h>
#include <xtensa/hal.h>

#include "mpu_wrappers.h"
#include "xtensa_context.h"


/* Typedef for C-callable interrupt handler function */
typedef void (*xt_handler)(void *);

/* Typedef for C-callable exception handler function */
typedef void (*xt_exc_handler)(XtExcFrame *);


/*
-------------------------------------------------------------------------------
  Call this function to set a handler for the specified exception.

    n        - Exception number (type)
    f        - Handler function address, NULL to uninstall handler.

  The handler will be passed a pointer to the exception frame, which is created
  on the stack of the thread that caused the exception.

  If the handler returns, the thread context will be restored and the faulting
  instruction will be retried. Any values in the exception frame that are
  modified by the handler will be restored as part of the context. For details
  of the exception frame structure see xtensa_context.h.
-------------------------------------------------------------------------------
*/
extern xt_exc_handler xt_set_exception_handler( uint32_t n, xt_exc_handler f ) PRIVILEGED_FUNCTION;


/*
-------------------------------------------------------------------------------
  Call this function to set a handler for the specified interrupt.
 
    n        - Interrupt number.
    f        - Handler function address, NULL to uninstall handler.
    arg      - Argument to be passed to handler.
-------------------------------------------------------------------------------
*/
extern xt_handler xt_set_interrupt_handler( uint32_t n, xt_handler f, void * arg ) PRIVILEGED_FUNCTION;


/*
-------------------------------------------------------------------------------
  Call this function to enable the specified interrupt.

    intnum    - Interrupt number to be enabled.

  Returns: Nothing.
-------------------------------------------------------------------------------
*/
extern void xt_interrupt_enable( uint32_t intnum ) PRIVILEGED_FUNCTION;


/*
-------------------------------------------------------------------------------
  Call this function to disable the specified interrupt.

    intnum    - Interrupt number to be disabled.

  Returns: Nothing.
-------------------------------------------------------------------------------
*/
extern void xt_interrupt_disable( uint32_t intnum ) PRIVILEGED_FUNCTION;


/*
-------------------------------------------------------------------------------
  Call this function to check the enable state of the specified interrupt.

    intnum    - Interrupt number to be checked.

  Returns: 1 if the specified interrupt is enabled, zero if the interrupt is
  disabled, zero if the interrupt number is invalid.
-------------------------------------------------------------------------------
*/
uint32_t
xt_interrupt_enabled( uint32_t intnum ) PRIVILEGED_FUNCTION;


/*
-------------------------------------------------------------------------------
  Call this function to trigger the specified (s/w) interrupt.

    intnum    - Interrupt number to be triggered.

  Returns: Nothing.
-------------------------------------------------------------------------------
*/
extern void xt_interrupt_trigger( uint32_t intnum ) PRIVILEGED_FUNCTION;


/*
-------------------------------------------------------------------------------
  Call this function to clear the specified (s/w or edge-triggered)
  interrupt.

    intnum    - Interrupt number to be cleared.

  Returns: Nothing.
-------------------------------------------------------------------------------
*/
extern void xt_interrupt_clear( uint32_t intnum ) PRIVILEGED_FUNCTION;


/*
-------------------------------------------------------------------------------
  Update CPU core/timer frequency.

  This is implemented when configUSE_TICKLESS_IDLE is set to 1.
  XT_CLOCK_FREQ or xtbsp_clock_freq_hz must return new clock frequency
  when this function is called.
 
  This function should be called as soon as the core frequency has changed.
  It is safe to call it from an interrupt service routine.
  It will update deadline of the internal tick timer and cache new core
  frequency.

  Returns: Nothing
-------------------------------------------------------------------------------
*/
extern void xt_update_clock_frequency( void ) PRIVILEGED_FUNCTION;

/*
 * These map directly to HAL functions.
 */
#define xt_get_ccount           xthal_get_ccount
#define xt_get_ccompare(t)      xthal_get_ccompare(t)
#define xt_set_ccompare(t,v)    xthal_set_ccompare(t,v)


#endif /* __XTENSA_API_H__ */
