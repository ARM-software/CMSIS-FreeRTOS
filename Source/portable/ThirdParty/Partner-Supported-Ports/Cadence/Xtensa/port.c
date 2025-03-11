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

#if portUSING_MPU_WRAPPERS
# define MPU_WRAPPERS_INCLUDED_FROM_API_FILE
#endif

#include <stdlib.h>

#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#if XCHAL_HAVE_INTERRUPTS
#include <xtensa/tie/xt_interrupt.h>
#endif
#if XCHAL_HAVE_ISL || XCHAL_HAVE_KSL || XCHAL_HAVE_PSL
#include <xtensa/tie/xt_exception_dispatch.h>
#endif

#include "xtensa_api.h"
#include "xtensa_rtos.h"

#include "FreeRTOS.h"
#include "task.h"

/* Heap area (see heap_4.c). When MPU in use, align it to the MPU
   region boundary to avoid overlapping with non-heap data. */
#if portUSING_MPU_WRAPPERS
#define HEAP_SIZE    ((configTOTAL_HEAP_SIZE + XCHAL_MPU_ALIGN - 1) & -XCHAL_MPU_ALIGN)
PRIVILEGED_DATA uint8_t ucHeap[ HEAP_SIZE ] __attribute__((aligned(XCHAL_MPU_ALIGN)));
#else
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif

#if portUSING_MPU_WRAPPERS
/* Configure a number of standard MPU regions that are used by all tasks. */
extern BaseType_t prvSetupMPU( void ) PRIVILEGED_FUNCTION;

/*
 * Checks to see if being called from the context of an unprivileged task, and
 * if so raises the privilege level and returns false - otherwise does nothing
 * other than return true.
 */
BaseType_t xPortRaisePrivilege( void );

#endif

#if XCHAL_CP_NUM > 0
extern void _xt_coproc_init( void );
extern void _xt_coproc_exc(XtExcFrame * fp);
#endif

// Defined in xtensa_vectors.S.
extern void _xt_task_start( void );
#if XCHAL_HAVE_XEA3 && portUSING_MPU_WRAPPERS
extern void _xt_task_start_user( void );
#endif

// Timer tick interval in cycles.
static uint32_t xt_tick_cycles;
TickType_t xMaxSuppressedTicks;

static uint32_t xt_tick_count;

#if ( configUSE_TICKLESS_IDLE != 0 )
// Flag to indicate tick handling should be skipped.
static volatile uint32_t xt_skip_tick;
#endif

#if XCHAL_HAVE_XEA3
int32_t xt_sw_intnum = -1;
#endif

// Duplicate of inaccessible xSchedulerRunning.
uint32_t port_xSchedulerRunning = 0U;

// Interrupt nesting level.
uint32_t port_interruptNesting  = 0U;

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

//-----------------------------------------------------------------------------
// Tick timer interrupt handler.
//-----------------------------------------------------------------------------
static void xt_tick_handler( void )
{
    int32_t diff;

#if ( configUSE_TICKLESS_IDLE != 0 )
    if ( xt_skip_tick )
    {
        vTaskStepTick( xt_skip_tick );
        xt_tick_count += xt_skip_tick;
        xt_skip_tick = 0;
    }
#endif

    do
    {
        BaseType_t ret;
        uint32_t   interruptMask;
        uint32_t   ulOldCCompare = xt_get_ccompare( XT_TIMER_INDEX );

        // Set CCOMPARE for next tick.
        xt_set_ccompare( XT_TIMER_INDEX, ulOldCCompare + xt_tick_cycles );

        portbenchmarkIntLatency();

        // Interrupts upto configMAX_SYSCALL_INTERRUPT_PRIORITY must be
        // disabled before calling xTaskIncrementTick as it accesses the
        // kernel lists.
        interruptMask = portSET_INTERRUPT_MASK_FROM_ISR();
        {
            ret = xTaskIncrementTick();
            ++xt_tick_count;
        }
        portCLEAR_INTERRUPT_MASK_FROM_ISR( interruptMask );

        portYIELD_FROM_ISR( ret );

        // Signed comparison gracefully handles cases where another source
        // has called xt_update_clock_frequency(), e.g. for tickless idle
        // or variable frequency support, and ccompare was advanced 
        // farther than expected.
        diff = (int32_t)(xt_get_ccount() - ulOldCCompare);
    }
    while ( diff > (int32_t)xt_tick_cycles );
}

static void update_xt_tick_cycles( void )
{
    // Compute the number of cycles per tick.
    #ifdef XT_CLOCK_FREQ
    xt_tick_cycles = ( XT_CLOCK_FREQ / XT_TICK_PER_SEC );
    #elif defined(XT_BOARD)
    xt_tick_cycles = xtbsp_clock_freq_hz() / XT_TICK_PER_SEC;
    #else
    #error "No way to obtain processor clock frequency"
    #endif
}

//-----------------------------------------------------------------------------
// Tick timer init. Install interrupt handler, set up first tick, and
// enable timer interrupt.
//-----------------------------------------------------------------------------
static void xt_tick_timer_init( void )
{
    update_xt_tick_cycles();
    xMaxSuppressedTicks = 0xFFFFFFFFU / xt_tick_cycles;
    xt_set_interrupt_handler( XT_TIMER_INTNUM, (xt_handler) xt_tick_handler, 0 );
    xt_set_ccompare( XT_TIMER_INDEX, xthal_get_ccount() + xt_tick_cycles );
    xt_tick_count = xTaskGetTickCount();
    xt_interrupt_enable( XT_TIMER_INTNUM );
}

//-----------------------------------------------------------------------------
// Tick timer stop. Disable timer interrupt and clear ccompare register.
//-----------------------------------------------------------------------------
static void xt_tick_timer_stop( void )
{
    xt_interrupt_disable( XT_TIMER_INTNUM );
    xt_set_ccompare( XT_TIMER_INDEX, 0 );
}

//-----------------------------------------------------------------------------
// Start the scheduler.
//-----------------------------------------------------------------------------
BaseType_t xPortStartScheduler( void )
{
    #if XCHAL_HAVE_XEA3
    extern void xt_sched_handler(void * arg);
    int32_t i;
    #endif

    // Interrupts are disabled at this point and stack contains PS with
    // enabled interrupts when task context is restored.

    #if XCHAL_CP_NUM > 0
    // Initialize co-processor management for tasks. Leave CPENABLE alone.
    _xt_coproc_init();

    #if XCHAL_HAVE_XEA3
    // Install the coprocessor exception handler.
    xt_set_exception_handler(EXCCAUSE_CP_DISABLED, _xt_coproc_exc);
    #endif
    #endif

    #if XCHAL_HAVE_XEA3
    // Select a software interrupt to use for scheduling.
    for (i = 0; i < XCHAL_NUM_INTERRUPTS; i++) {
        if ((Xthal_inttype[i] == XTHAL_INTTYPE_SOFTWARE) && (Xthal_intlevel[i] == 1)) {
            xt_sw_intnum = i;
            break;
        }
    }

    if (xt_sw_intnum == -1) {
        return pdFALSE;
    }

    /* Set the interrupt handler and enable the interrupt. */
    xt_set_interrupt_handler(xt_sw_intnum, xt_sched_handler, 0);
    xt_interrupt_enable(xt_sw_intnum);

    #if XCHAL_HAVE_KSL
    XT_WSR_KSL(0);
    #endif
    #if XCHAL_HAVE_ISL
    XT_WSR_ISL(0);
    #endif
    #endif

    // Set up and enable timer tick.
    xt_tick_timer_init();

    #if XT_USE_THREAD_SAFE_CLIB
    // Init C library
    vPortClibInit();
    #endif

    #if portUSING_MPU_WRAPPERS
    // Setup MPU
    if (prvSetupMPU() == pdFALSE)
       return pdFALSE;
    #endif

    port_xSchedulerRunning = 1U;

    // Cannot be directly called from C; never returns
    __asm__ volatile ("call0    _frxt_dispatch\n");

    // Should never get here.
    return pdFALSE;
}

//-----------------------------------------------------------------------------
// Stop the scheduler.
//-----------------------------------------------------------------------------
void vPortEndScheduler( void )
{
    xt_tick_timer_stop();
    port_xSchedulerRunning = 0U;
}

//-----------------------------------------------------------------------------
// Stack initialization.
// Reserve coprocessor save area if needed, construct a dummy stack frame and
// populate it for task startup. Return the pointer to the dummy stack frame.
// (NOTE: the value returned from this function is expected to be stored in
// pxTCB->pxTopOfStack. In the task wrapper code, we will set the value of
// pxTCB->pxEndOfStack, which will then be treated as the coprocessor state
// area pointer.
//-----------------------------------------------------------------------------
#if portUSING_MPU_WRAPPERS
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack,
                                    TaskFunction_t pxCode,
                                    void *pvParameters,
                                    BaseType_t xRunPrivileged,
                                    xMPU_SETTINGS * xMPUSettings )
#else
StackType_t *pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                    TaskFunction_t pxCode,
                                    void * pvParameters )
#endif
{
    StackType_t *sp;
    StackType_t *tp;
    StackType_t *ret;
    XtExcFrame  *frame;
    #if XCHAL_CP_NUM > 0
    uint32_t *p;
    #endif

#if portUSING_MPU_WRAPPERS
    // TODO: Handle this...
    (void)xMPUSettings;
#endif

    // Allocate enough space for coprocessor state, align base address. This is the
    // adjusted top-of-stack and also the start of the coprocessor save area.
    sp = (StackType_t *) ((((uint32_t) pxTopOfStack) - (uint32_t) XT_CP_SIZE) & (uint32_t)~0xF);

    // Allocate interrupt stack frame. XT_STK_FRMSZ is always a multiple of 16 bytes
    // so 16-byte alignment is ensured.
    tp = sp - (XT_STK_FRMSZ/sizeof(StackType_t));

    // This is the address we will return.
    ret = tp;

    #if XCHAL_HAVE_XEA3
    frame = (XtExcFrame *) (tp + (XT_STK_XTRA_SZ/sizeof(StackType_t)));
    #else
    frame = (XtExcFrame *) tp;
    #endif

    // Clear the frame (do not use memset() because we don't depend on C library).
    for (; tp < sp; ++tp)
    {
        *tp = 0;
    }

    // Explicitly initialize certain saved registers. Note that the entry point
    // is set to be the task wrapper, and the address of the coprocessor save
    // area (sp) is saved in the dummy frame for the wrapper to use.

    #if XCHAL_HAVE_XEA2
    frame->pc   = (long) pxCode;            // task entrypoint
    frame->a0   = 0;                        // to terminate GDB backtrace
    frame->a1   = (long) sp;                // top of stack - CP save area
    frame->exit = (long) _xt_task_start;    // task start wrapper
    // Set initial PS to int level 0, EXCM disabled ('rfe' will enable), user mode.
    // Also set entry point argument parameter.
    #ifdef __XTENSA_CALL0_ABI__
    frame->a2 = (long) pvParameters;
    frame->ps = PS_UM | PS_EXCM;
    #else
    // + for windowed ABI also set WOE and CALLINC (pretend task was 'call4'd).
    frame->a6 = (long) pvParameters;
    frame->ps = PS_UM | PS_EXCM | PS_WOE | PS_CALLINC(1);
    #endif
    #if portUSING_MPU_WRAPPERS
    if(!xRunPrivileged) {
       frame->ps |= (1 << PS_RING_SHIFT);
    }
    #endif
    #endif

    #if XCHAL_HAVE_XEA3
    frame->a8 = (long) pxCode;              // task entrypoint
    frame->a9 = (long) sp;                  // top of stack - CP save area
    #if portUSING_MPU_WRAPPERS
    frame->pc = (!xRunPrivileged) ?
       (long) _xt_task_start_user :         // PS_RING will be set later
       (long) _xt_task_start;               // standard task start wrapper
    #else
    frame->pc = (long) _xt_task_start;      // task start wrapper
    #endif
    frame->ps = PS_STACK_FIRSTKER;          // initial PS
    frame->atomctl = 0;                     // initial value
    // Set entry point arg.
    #ifdef __XTENSA_CALL0_ABI__
    frame->a2  = (long) pvParameters;
    #else
    frame->a10 = (long) pvParameters;
    #endif
    #endif

    #ifdef XT_USE_SWPRI
    // Set the initial virtual priority mask value to all 1's.
    frame->vpri = 0xFFFFFFFF;
    #endif

    #if XCHAL_CP_NUM > 0
    // Init the coprocessor save area (see xtensa_context.h).
    p = (uint32_t *) sp;
    p[0] = 0;
    p[1] = 0;
    p[2] = (((uint32_t) p) + 12 + XCHAL_TOTAL_SA_ALIGN - 1) & -XCHAL_TOTAL_SA_ALIGN;
    #endif

    return ret;
}

//-----------------------------------------------------------------------------
// Tickless idle support. Suppress N ticks and sleep when directed by kernel.
//-----------------------------------------------------------------------------
#if ( configUSE_TICKLESS_IDLE != 0 )
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    eSleepModeStatus eSleepStatus;

    // Lock out all interrupts. Otherwise reading and using ccount can
    // get messy. Shouldn't be a problem here since we are about to go
    // to sleep, and the waiti will re-enable interrupts shortly.
    
    // Must call vTaskEnterCritical() in order to increment FreeRTOS 
    // nesting count, otheriwse the call to vTaskStepTick() inside this
    // critical section will inadvertently reenable interrupts.
    portENTER_CRITICAL();

    eSleepStatus = eTaskConfirmSleepModeStatus();
    if ( eSleepStatus == eAbortSleep )
    {
        // Abort, fall through.
    }
    else
    {
        uint32_t num_cycles;
        uint32_t first_blocked_tick;
        uint32_t ccompare;
        uint32_t skip_tick;
        uint32_t now;

        // Compute number of cycles to sleep for, capped by max limit.
        // we use one less than the number of ticks because we are already
        // partway through the current tick. This is adjusted later below.
        if ( xExpectedIdleTime > xMaxSuppressedTicks )
        {
            skip_tick = xMaxSuppressedTicks - 1U;
        }
        else
        {
            skip_tick = xExpectedIdleTime - 1U;
        }

        num_cycles = xt_tick_cycles * skip_tick;
        first_blocked_tick = xt_get_ccompare( XT_TIMER_INDEX );
        xt_skip_tick = skip_tick;

        // Set up for timer interrupt and sleep.
        ccompare = first_blocked_tick + num_cycles;
        xt_set_ccompare( XT_TIMER_INDEX, ccompare );
#if XCHAL_HAVE_XEA3
        // Ccompare write will not clear pending interrupt.
        xt_interrupt_clear( XT_TIMER_INTNUM );
#endif
        // Ensure any clearing of pending interrupt takes effect.
        // WAITI instruction will reduce interrupt level and sleep so
        // it must be followed by raising the interrupt level to ensure
        // the critical section is maintained.  The NESTED version is 
        // used here since it does not increment the nesting count that 
        // is managed upon entry and exit of this function.
        XT_ISYNC();
        XT_WAITI( 0 );
        portENTER_CRITICAL_NESTED();

        skip_tick = xt_skip_tick;
        now = xt_get_ccount();

        // Awakened by non-timer interrupt, update tick counter here.

        if ( skip_tick )
        {
            // If there's more than a tick period from now to the timer
            // deadline try to move deadline to the next possible tick.
            // Otherwise update tick count for the passed ticks, but don't
            // change the deadline.
            // If we get here, we have not handled a timer interrupt, so
            // ccompare is still in the future, or in the immediate past
            // (crossed after entering the critical section above). In the
            // latter case the pending interrupt will be cleared below.

            if ( ccompare - now > xt_tick_cycles )
            {
                uint32_t prev_tick = first_blocked_tick - xt_tick_cycles;
                uint32_t actual_cycles = now - prev_tick;
                uint32_t ticks = actual_cycles / xt_tick_cycles;
                uint32_t diff;

                ccompare = first_blocked_tick + ticks * xt_tick_cycles;

                do
                {
                    vTaskStepTick( ticks );
                    xt_tick_count += ticks;
#if XCHAL_HAVE_XEA3
                    // ccompare may be very close in the xt_set_ccompare call
                    // below, so we cannot reliably call xt_interrupt_clear
                    // after it. Make sure that timer IRQ is clear when
                    // xt_set_ccompare( XT_TIMER_INDEX, ccompare ) is called.
                    // Set ccompare to current ccount to avoid timer IRQ
                    // arriving in the gap between xt_interrupt_clear
                    // and the following xt_set_ccompare.
                    xt_set_ccompare( XT_TIMER_INDEX, xt_get_ccount() );
                    // Ccompare write will not clear pending interrupt.
                    xt_interrupt_clear( XT_TIMER_INTNUM );
#endif
                    xt_set_ccompare( XT_TIMER_INDEX, ccompare );
                    diff = xt_get_ccount() - ccompare;
                    ccompare += xt_tick_cycles;
                    ticks = 1;

                } while ( diff <= INT32_MAX );
            }
            else
            {
                vTaskStepTick( skip_tick );
                xt_tick_count += skip_tick;
            }

            xt_skip_tick = 0;
        }
    }

    portEXIT_CRITICAL();
}
#endif

#if portUSING_MPU_WRAPPERS
extern void vPortResetPrivilege(BaseType_t previous);
void vPortEnterCritical( void )
{
    // TODO: handle configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS
    // For reference, see commit 79704b from 9/16/2022
    if( portIS_PRIVILEGED() == pdFALSE )
    {
        portRAISE_PRIVILEGE();
        vTaskEnterCritical();
        portRESET_PRIVILEGE();
    }
    else
    {
        // TODO: handle port_interruptNesting
        vTaskEnterCritical();
    }
}

void vPortExitCritical( void )
{
    // TODO: handle configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS
    // For reference, see commit 79704b from 9/16/2022
    if( portIS_PRIVILEGED() == pdFALSE )
    {
        portRAISE_PRIVILEGE();
        vTaskExitCritical();
        portRESET_PRIVILEGE();
    }
    else
    {
        // TODO: handle port_interruptNesting
        vTaskExitCritical();
    }
}
#endif

#if ( configUSE_VARIABLE_FREQUENCY != 0 )
static void update_tick_remainder( uint32_t now )
{
    uint32_t old_ccompare;
    uint32_t old_tick_cycles;

    old_ccompare = xt_get_ccompare( XT_TIMER_INDEX );
    old_tick_cycles = xt_tick_cycles;
    update_xt_tick_cycles();

    // If tick deadline has not been reached yet correct number of remaining
    // cycles, otherwise timer interrupt must be pending, just service it.

    if ( old_ccompare - now < old_tick_cycles )
    {
        uint32_t new_ccompare = now +
            (uint32_t)((uint64_t)(old_ccompare - now) * xt_tick_cycles / old_tick_cycles);

        xt_set_ccompare( XT_TIMER_INDEX, new_ccompare );
        now = xt_get_ccount();
        if ( new_ccompare - now > xt_tick_cycles )
            xt_tick_handler();
    }
}

#if ( configUSE_TICKLESS_IDLE != 0 )
void xt_update_clock_frequency( void )
{
    uint32_t skip_tick;
    uint32_t now;

    portENTER_CRITICAL();

    now = xt_get_ccount();
    skip_tick = xt_skip_tick;

    if ( skip_tick )
    {
        uint32_t ccompare = xt_get_ccompare( XT_TIMER_INDEX );

        // If there's more than a tick period from now to the timer
        // deadline try to move deadline to the next possible tick.
        // Otherwise update tick count for the passed ticks, but don't
        // change the deadline.

        if ( ccompare - now > xt_tick_cycles &&
             ccompare - now <= INT32_MAX )
        {
            uint32_t first_blocked_tick = ccompare - xt_tick_cycles * skip_tick;
            uint32_t prev_tick = first_blocked_tick - xt_tick_cycles;
            uint32_t actual_cycles = now - prev_tick;
            uint32_t ticks = actual_cycles / xt_tick_cycles;
            uint32_t diff;

            ccompare = first_blocked_tick + ticks * xt_tick_cycles;

            do
            {
                vTaskStepTick( ticks );
                xt_tick_count += ticks;
                xt_set_ccompare( XT_TIMER_INDEX, ccompare );
                diff = xt_get_ccount() - ccompare;
                ccompare += xt_tick_cycles;
                ticks = 1;

            } while ( diff <= INT32_MAX );
        }
        else
        {
            vTaskStepTick( skip_tick );
            xt_tick_count += skip_tick;
        }
        xt_skip_tick = 0;
    }
    update_tick_remainder( now );

    portEXIT_CRITICAL();
}
#else
void xt_update_clock_frequency( void )
{
    portENTER_CRITICAL();
    update_tick_remainder( xt_get_ccount() );
    portEXIT_CRITICAL();
}
#endif
#endif
