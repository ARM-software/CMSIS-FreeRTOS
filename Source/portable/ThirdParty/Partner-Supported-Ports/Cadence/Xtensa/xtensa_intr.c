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
 * Xtensa-specific interrupt and exception functions for RTOS ports.
 * Also see xtensa_intr_asm.S.
 */

#include <stdlib.h>
#include <unistd.h>

#include <xtensa/config/core.h>
#include <xtensa/core-macros.h>
#if XCHAL_HAVE_INTERRUPTS
#include <xtensa/tie/xt_interrupt.h>
#endif

#include "xtensa_api.h"

#include "FreeRTOS.h"


#if XCHAL_HAVE_EXCEPTIONS

/* Handler table is in xtensa_intr_asm.S */
extern xt_exc_handler xt_exception_table[XCHAL_EXCCAUSE_NUM];


/*
  Default handler for unhandled exceptions.
*/
void
xt_unhandled_exception( XtExcFrame * frame )
{
    (void) frame;
#if ( configNUMBER_OF_CORES > 1 )
    static int uexc_recursion_depth = 0;
    if (++uexc_recursion_depth >= configNUMBER_OF_CORES) {
        /* In the event that calling _exit() results in an unhandled 
         * exception, stop the subsequent infinite loop.
         */
        while ( 1 );
    }
    if (portGET_CORE_ID() > 0) {
        _exit( -1 );
    }
#endif
    exit( -1 );
}


/*
  This function registers a handler for the specified exception.
  The function returns the address of the previous handler.
  On error, it returns NULL.
*/
xt_exc_handler
xt_set_exception_handler( uint32_t n, xt_exc_handler f )
{
    xt_exc_handler old;

    if ( n >= (uint32_t) XCHAL_EXCCAUSE_NUM )
    {
        // Invalid exception number.
        return NULL;
    }

    old = xt_exception_table[n];

    if ( f != NULL )
    {
        xt_exception_table[n] = f;
    }
    else
    {
        xt_exception_table[n] = &xt_unhandled_exception;
    }

    return old;
}

#endif

#if XCHAL_HAVE_INTERRUPTS

/* Handler table is in xtensa_intr_asm.S */
typedef struct xt_handler_table_entry {
    xt_handler handler;
    void *     arg;
} xt_handler_table_entry;

#if (XT_USE_INT_WRAPPER || XCHAL_HAVE_XEA3)
extern xt_handler_table_entry _xt_interrupt_table[XCHAL_NUM_INTERRUPTS + 1];
#else
extern xt_handler_table_entry _xt_interrupt_table[XCHAL_NUM_INTERRUPTS];
#endif


/*
  Default handler for unhandled interrupts.
*/
void
xt_unhandled_interrupt( void * arg )
{
    (void) arg;
#if ( configNUMBER_OF_CORES > 1 )
    if (portGET_CORE_ID() > 0) {
        _exit( -1 );
    }
#endif
    exit( -1 );
}


/*
  This function registers a handler for the specified interrupt. The "arg"
  parameter specifies the argument to be passed to the handler when it is
  invoked. The function returns the address of the previous handler.
  Note that the previous handler's argument will be lost.
  On error, it returns NULL.
*/
xt_handler
xt_set_interrupt_handler( uint32_t n, xt_handler f, void * arg )
{
    xt_handler_table_entry * entry;
    xt_handler               old;

    if ( n >= (uint32_t) XCHAL_NUM_INTERRUPTS )
    {
        // Invalid interrupt number.
        return NULL;
    }

#if XCHAL_HAVE_XEA2
    if ( Xthal_intlevel[n] > XCHAL_EXCM_LEVEL )
    {
        // Priority level too high to safely handle in C.
        return NULL;
    }
#endif

#if (XT_USE_INT_WRAPPER || XCHAL_HAVE_XEA3)
    entry = _xt_interrupt_table + n + 1;
#else
    entry = _xt_interrupt_table + n;
#endif
    old   = entry->handler;

    if ( f != NULL )
    {
        entry->handler = f;
        entry->arg     = arg;
    }
    else
    {
        entry->handler = &xt_unhandled_interrupt;
        entry->arg     = (void*)n;
    }

    return old;
}


/*
  This function returns a handler for the specified interrupt.
  If no handler has been registered, it returns a pointer to 
  xt_unhandled_interrupt().  
  If the interrupt number or level are invalid, it returns NULL.
*/
xt_handler
xt_get_interrupt_handler( uint32_t n )
{
    xt_handler_table_entry * entry;

    if ( n >= (uint32_t) XCHAL_NUM_INTERRUPTS )
    {
        // Invalid interrupt number.
        return NULL;
    }

#if XCHAL_HAVE_XEA2
    if ( Xthal_intlevel[n] > XCHAL_EXCM_LEVEL )
    {
        // Priority level too high to safely handle in C.
        return NULL;
    }
#endif

#if (XT_USE_INT_WRAPPER || XCHAL_HAVE_XEA3)
    entry = _xt_interrupt_table + n + 1;
#else
    entry = _xt_interrupt_table + n;
#endif
    return entry->handler;
}


/*
  This function enables the interrupt whose number is specified as
  the argument.
*/
void
xt_interrupt_enable( uint32_t intnum )
{
#if XCHAL_HAVE_XEA2 && (XCHAL_NUM_INTERRUPTS <= 32)
    if ( intnum < (uint32_t) XCHAL_NUM_INTERRUPTS )
    {
        #if ( configNUMBER_OF_CORES > 1 )
        xt_internal_data_t *intdatap = &(_XT_INTDATA(portGET_CORE_ID()));
        #else
        xt_internal_data_t *intdatap = &_xt_intdata;
        #endif
        int ps = XT_RSIL( 15 );

        // New INTENABLE = (xt_intenable | mask) & xt_vpri_mask.
        intdatap->xt_intenable |= ( 1U << intnum );
        XT_WSR_INTENABLE( intdatap->xt_intenable & intdatap->xt_vpri_mask );
        XT_WSR_PS( ps );
        XT_RSYNC();
    }
#else
    xthal_interrupt_enable( intnum );
#endif
}


/*
  This function disables the interrupt whose number is specified as
  the argument.
*/
void
xt_interrupt_disable( uint32_t intnum )
{
#if XCHAL_HAVE_XEA2 && (XCHAL_NUM_INTERRUPTS <= 32)
    if ( intnum < (uint32_t) XCHAL_NUM_INTERRUPTS )
    {
        #if ( configNUMBER_OF_CORES > 1 )
        xt_internal_data_t *intdatap = &(_XT_INTDATA(portGET_CORE_ID()));
        #else
        xt_internal_data_t *intdatap = &_xt_intdata;
        #endif
        int ps = XT_RSIL( 15 );

        // New INTENABLE = (xt_intenable & ~mask) & xt_vpri_mask.
        intdatap->xt_intenable &= ~( 1U << intnum );
        XT_WSR_INTENABLE( intdatap->xt_intenable & intdatap->xt_vpri_mask );
        XT_WSR_PS( ps );
        XT_RSYNC();
    }
#else
    xthal_interrupt_disable( intnum );
#endif
}


/*
  This function returns : 1 if the specified interrupt is enabled, zero
  if the interrupt is disabled, zero if the interrupt number is invalid.
*/
uint32_t
xt_interrupt_enabled( uint32_t intnum )
{
#if XCHAL_HAVE_XEA2 && (XCHAL_NUM_INTERRUPTS <= 32)
    if ( intnum < (uint32_t) XCHAL_NUM_INTERRUPTS )
    {
        #if ( configNUMBER_OF_CORES > 1 )
        xt_internal_data_t *intdatap = &(_XT_INTDATA(portGET_CORE_ID()));
        #else
        xt_internal_data_t *intdatap = &_xt_intdata;
        #endif

        return ( (intdatap->xt_intenable & (1U << intnum)) != 0 ) ? 1U : 0;
    }
    return 0;
#else
    return xthal_interrupt_enabled( intnum );
#endif
}


/*
  This function triggers the specified interrupt.
*/
void
xt_interrupt_trigger( uint32_t intnum )
{
    xthal_interrupt_trigger( intnum );
}


/*
  This function clears the specified interrupt.
*/
void
xt_interrupt_clear( uint32_t intnum )
{
    xthal_interrupt_clear( intnum );
}


#endif /* XCHAL_HAVE_INTERRUPTS */
