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

#include <xtensa/config/core.h>
#include <xtensa/core-macros.h>

#include "xtensa_rtos.h"
#include "xtensa_api.h"
#include "portmacro.h"


#if (XCHAL_HAVE_XEA3 && XCHAL_HAVE_INTERRUPTS)

/* Handler table is in xtensa_intr_asm.S */

typedef struct xt_handler_table_entry {
    void * handler;
    void * arg;
} xt_handler_table_entry;

extern xt_handler_table_entry _xt_interrupt_table[XCHAL_NUM_INTERRUPTS + 1];

extern int32_t  xt_sw_intnum;
extern int32_t  port_switch_flag;
extern uint32_t port_interruptNesting;

static int32_t  xt_wflag;


/**************************************************************************/
/*    Wrapper for interrupt handlers. Argument is (intnum << 2).          */
/*    Execution comes here from the dispatch code if the wrapper is       */
/*    enabled.                                                            */
/**************************************************************************/
void
xt_interrupt_wrapper(void * arg)
{
    uint32_t                 intnum = (uint32_t)(arg) >> 2;
    uint32_t                 state;
    xt_handler_table_entry * entry;
    xt_handler               handler;

    state = portENTER_CRITICAL_NESTED();
    port_interruptNesting++;
    portEXIT_CRITICAL_NESTED(state);

    /* Load handler address and argument from table. Note that the
       first entry in the table points to this wrapper, so we have
       to skip ahead one.
     */
    entry = _xt_interrupt_table + intnum + 1;
    handler = (xt_handler) entry->handler;

    (*handler)(entry->arg);

    /* If a context switch is pending, trigger the SW interrupt
       to process the switch. Set an internal flag so we don't
       trigger the sw interrupt again when handling it.
     */
    if (xt_wflag != 0) {
        xt_wflag = 0;
    }
    else if (port_switch_flag) {
        xt_wflag = 1;
        xt_interrupt_trigger(xt_sw_intnum);
    }

    state = portENTER_CRITICAL_NESTED();
    port_interruptNesting--;
    portEXIT_CRITICAL_NESTED(state);
}

#endif /* XCHAL_HAVE_XEA3 && XCHAL_HAVE_INTERRUPTS */
