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

#include "FreeRTOS.h"
#include "xtensa_config.h"

#include "xtensa_api.h"
#include "xtensa_rtos.h"

#include "task.h"

/* Heap area (see heap_4.c). When MPU in use, align it to the MPU
   region boundary to avoid overlapping with non-heap data. */
#if portUSING_MPU_WRAPPERS && configAPPLICATION_ALLOCATED_HEAP
#define HEAP_SIZE    ((configTOTAL_HEAP_SIZE + XCHAL_MPU_ALIGN - 1) & -XCHAL_MPU_ALIGN)
PRIVILEGED_DATA uint8_t ucHeap[ HEAP_SIZE ] __attribute__((aligned(XCHAL_MPU_ALIGN)));
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

#if ( configNUMBER_OF_CORES > 1 )
extern uint32_t _bss_table_start;
extern uint32_t _bss_table_end;
extern void __bss_init(uint32_t * table_start, uint32_t * table_end);
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

#if (defined __DYNAMIC_REENT__)
  #if ( configNUMBER_OF_CORES > 1 )
    #if XSHAL_CLIB == XTHAL_CLIB_XCLIB
    #define _XT_INTDATA_REENT_INIT(x)   NULL, NULL, { 0 },
    #elif XSHAL_CLIB == XTHAL_CLIB_NEWLIB
      #if ( XT_USE_DATARAM )
      #define _XT_INTDATA_REENT_INIT(x)   NULL, NULL, _REENT_INIT(_xt_intdata.xt_reent),
      #else
      #define _XT_INTDATA_REENT_INIT(x)   NULL, NULL, _REENT_INIT(_xt_intdata[(x)].xt_reent),
      #endif
    #else
    #error Specified CLIB not reentrant
    #endif
  #else
    #define _XT_INTDATA_REENT_INIT      NULL,
  #endif    // configNUMBER_OF_CORES > 1
#else
  #if ( configNUMBER_OF_CORES > 1 )
    #define _XT_INTDATA_REENT_INIT(x)
  #else
    #define _XT_INTDATA_REENT_INIT
  #endif
#endif      // __DYNAMIC_REENT__

#if ( configNUMBER_OF_CORES == 1 )

// Interrupt nesting level and task switch flag maintained together.
xt_internal_data_t _xt_intdata = {
    0, 0, 0, 0xffffffff, _XT_INTDATA_REENT_INIT
};

#else

#if ( XT_USE_DATARAM )

// Per-core struct contains interrupt variables and uxCriticalNestings
// When in dataram, structure is in per-core memory and not padded.
xt_internal_data_t XT_DATARAM_ATTR
_xt_intdata = { 0, 0, 0, 0, 0xffffffff, 0, _XT_INTDATA_REENT_INIT(0) };

#else       // XT_USE_DATARAM

// Per-core struct contains interrupt variables and uxCriticalNestings
// When in shared sysram, structure is padded to cache line and indexed per-core
xt_internal_data_t __attribute__((aligned (XCHAL_DCACHE_LINESIZE)))
_xt_intdata[ configNUMBER_OF_CORES ] = {
    { 0, 0, 0, 0, 0xffffffff, 0, _XT_INTDATA_REENT_INIT(0) { 0 } },
#if ( configNUMBER_OF_CORES >= 2 )
    { 0, 0, 0, 0, 0xffffffff, 0, _XT_INTDATA_REENT_INIT(1) { 0 } },
#endif
#if ( configNUMBER_OF_CORES >= 3 )
    { 0, 0, 0, 0, 0xffffffff, 0, _XT_INTDATA_REENT_INIT(2) { 0 } },
#endif
#if ( configNUMBER_OF_CORES >= 4 )
    { 0, 0, 0, 0, 0xffffffff, 0, _XT_INTDATA_REENT_INIT(3) { 0 } },
#endif
#if ( configNUMBER_OF_CORES >= 5 )
    { 0, 0, 0, 0, 0xffffffff, 0, _XT_INTDATA_REENT_INIT(4) { 0 } },
#endif
#if ( configNUMBER_OF_CORES >= 6 )
    { 0, 0, 0, 0, 0xffffffff, 0, _XT_INTDATA_REENT_INIT(5) { 0 } },
#endif
#if ( configNUMBER_OF_CORES >= 7 )
    { 0, 0, 0, 0, 0xffffffff, 0, _XT_INTDATA_REENT_INIT(6) { 0 } },
#endif
#if ( configNUMBER_OF_CORES == 8 )
    { 0, 0, 0, 0, 0xffffffff, 0, _XT_INTDATA_REENT_INIT(7) { 0 } },
#endif
};

#endif      // XT_USE_DATARAM

PRIVILEGED_DATA xt_mutex __attribute__((aligned (XCHAL_DCACHE_LINESIZE))) _xt_mutex_ISR;
PRIVILEGED_DATA xt_mutex __attribute__((aligned (XCHAL_DCACHE_LINESIZE))) _xt_mutex_task;

#if (XT_USE_L2RAM)
XTHAL_L2_SETUP(XCHAL_L2RAM_RESET_PADDR, XT_L2RAM_SIXTEENTHS, XT_L2CACHE_SIXTEENTHS);
#endif

/*
 * Initialize the mutex.
 */
void
xt_mutex_init(xt_mutex_p pmtx)
{
    if (pmtx != NULL) {
        pmtx->owner = 0U;
        pmtx->count = 0U;
    }
}

/*
 * Lock the mutex, busy wait until lock acquired. Can be called repeatedly
 * to lock an already owned mutex. Note the locking is not protected against
 * interrupts. This is a potentially blocking function and should not be
 * called from an interrupt handler anyway.
 */
int32_t
xt_mutex_lock(xt_mutex_p pmtx)
{
    uint32_t id = (portGET_CORE_ID()) + 1U;

    if (pmtx != NULL) {
        if (pmtx->owner == id) {
            pmtx->count++;
        }
        else {
            int32_t ret;

            do {
#if XCHAL_HAVE_EXCLUSIVE
                /* Streamline implementation for SMP case.
                 * %0 : ret
                 * %1 : address &(pmtx->owner)
                 *    : test value (0) optimized w/ bnez
                 * %2 : set value (id) preserved for loop
                 * %3 : temp reg 1
                 * %4 : temp reg 2
                 */
                uint32_t t1 = 0, t2 = 1;    /* Different values trick optimizer */
                __asm__ volatile ("mov     %3, %2   /* %3 = copy of set_value */        \n\t"
                                  "1:                                                   \n\t"
                                  "l32ex   %0, %1   /* %0 = *address, set monitor */    \n\t"
                                  "bnez    %0, 2f   /* skip write if *address != 0 */   \n\t"
                                  "mov     %4, %3   /* %4 = set_value */                \n\t"
                                  "s32ex   %4, %1   /* *address = set_value */          \n\t"
                                  "getex   %4       /* get result of store */           \n\t"
                                  "beqz    %4, 1b                                       \n\t"
                                  "2:                                                   \n\t"
                                  "clrex            /* in case we skipped write */      \n\t"
                                  : "=&r"(ret)
                                  : "r"(&(pmtx->owner)), "r"(id), "r"(t1), "r"(t2));
#else
                ret = xthal_compare_and_set((int32_t *) &(pmtx->owner), 0, (int32_t) id);
#endif
            } while (ret != 0);
            pmtx->count = 1U;
        }
        return 0;
    }

    return -1;
}

/*
 * Unlock the mutex. Can be called repeatedly to unlock the same mutex.
 * The lock is only released when the lock count goes to zero.
 */
int32_t
xt_mutex_unlock(xt_mutex_p pmtx)
{
    uint32_t id = (portGET_CORE_ID()) + 1U;

    if ((pmtx != NULL) && (pmtx->owner == id)) {
        pmtx->count--;
        if (pmtx->count == 0U) {
            pmtx->owner = 0U;
        }
        return 0;
    }

    return -1;
}


// Ensure SMP initialization flag values are non-zero so it gets linked
// into .data and not .bss.
typedef enum {
    XT_SMP_SYNC_INITVAL = 1,
    XT_SMP_SYNC_DONE = 2,
} xt_smp_sync_t;

volatile xt_smp_sync_t xt_smp_sync = XT_SMP_SYNC_INITVAL;

#endif // ( configNUMBER_OF_CORES == 1 )

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
        interruptMask = taskENTER_CRITICAL_FROM_ISR();
        {
            ret = xTaskIncrementTick();
            ++xt_tick_count;
        }
        taskEXIT_CRITICAL_FROM_ISR( interruptMask );

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

#if ( configNUMBER_OF_CORES > 1 )
//-----------------------------------------------------------------------------
// IPI interrupts used for multicore scheduler
//-----------------------------------------------------------------------------
const uint32_t xt_ipi_intnum[configNUMBER_OF_CORES] = XCHAL_SUBSYS_IPI_S0_INTLIST;

//-----------------------------------------------------------------------------
// portYIELD_CORE IPI handler wrapper
//-----------------------------------------------------------------------------
static void xt_ipi_yield_wrapper( void * arg )
{
    // Flag a context switch and exit; _Interrupt() will do the rest.
    // Do NOT call vPortYieldFromInt() directly, which would result in twice
    // saving and clearing CPENABLE, corrupting the coprocessor state.
    UNUSED(arg);
    portYIELD_FROM_ISR(1);  // Flag a context switch and exit
}
#endif

//-----------------------------------------------------------------------------
// Start the scheduler.
//-----------------------------------------------------------------------------
BaseType_t xPortStartScheduler( void )
{
    #if XCHAL_HAVE_XEA3
    extern void xt_sched_handler(void * arg);
    extern void xt_unhandled_interrupt(void * arg);
    int32_t i;
    #endif
    #if (configNUMBER_OF_CORES > 1 )
    uint32_t c;
    uint32_t my_core = portGET_CORE_ID();
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
            if (xt_get_interrupt_handler(i) == &xt_unhandled_interrupt) {
                // Finalize the interrupt if not already in use
                xt_sw_intnum = i;
                break;
            }
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
    #endif  // XCHAL_HAVE_XEA3

    #if ( configNUMBER_OF_CORES > 1 )
    // Initialize SMP mutexes
    if (my_core == 0) {
        xt_mutex_init(&_xt_mutex_ISR);
        xt_mutex_init(&_xt_mutex_task);
    } else {
        // Ensure core 0 started first.
        // NOTE: if this assert triggers, ensure all nonzero cores start in RunStall.
        // On XTSC, that may entail setting the RunOnReset InitValue in subsys.yml.
        configASSERT( port_xSchedulerRunning );
    }

    // Limit the number of cacheops to prevent hangs in case the test uses large 
    // number of CacheOPs at the same time on multiple cores
    xthal_L2_prefetch_set_limit(XCHAL_L2CC_MAX_REQ/2);

    // Configure inter-processor interrupts that can be triggered by other cores;
    // used for portYIELD_CORE().
    for (c = 0; c < configNUMBER_OF_CORES; c++) {
        if (c != my_core) {
            if (!xt_set_interrupt_handler(xt_ipi_intnum[c], xt_ipi_yield_wrapper, NULL)) {
                return pdFALSE;
            }
            xt_interrupt_enable(xt_ipi_intnum[c]);
        }
    }

    if (my_core == configTICK_CORE) {
        // Set up and enable timer tick.
        xt_tick_timer_init();
    }
    #else   // configNUMBER_OF_CORES
    // Set up and enable timer tick.
    xt_tick_timer_init();
    #endif  // configNUMBER_OF_CORES

    #if XT_USE_THREAD_SAFE_CLIB
    // Init C library
    #if ( configNUMBER_OF_CORES > 1 )
    if (my_core == 0)
    #endif  // ( configNUMBER_OF_CORES > 1 )
    {
        // Init C library
        vPortClibInit();
    }
    #endif  // XT_USE_THREAD_SAFE_CLIB

    #if portUSING_MPU_WRAPPERS
    // Setup MPU
    if (prvSetupMPU() == pdFALSE)
       return pdFALSE;
    #endif

    port_xSchedulerRunning = 1U;

    #if ( configNUMBER_OF_CORES > 1 )
    if (my_core == 0) {
        // Cache-coherence means writeback operations are unnecessary.
        xt_smp_sync = XT_SMP_SYNC_DONE;

        // Release other cores last
        if (xthal_run_cores(XTSUB_RUN_ALL_CORES)) {
            return pdFALSE;
        }
    } else {
        // Used by xt-gdb thread-aware debug support
        _XT_INTDATA(my_core).xt_core_init_done = 1;
    }
    #endif

    // Spill and invalidate prior register windows so that solicited
    // restores do not inadvertently pick up starting register window
    xthal_window_spill();

    // Cannot be directly called from C; never returns
    __asm__ volatile ("call0    _frxt_dispatch\n");

    // Should never get here.
    return pdFALSE;
}

BaseType_t xPortIsInsideInterrupt( void )
{
    return (_XT_INTDATA(portGET_CORE_ID()).port_interruptNesting) > 0 ? pdTRUE : pdFALSE;
}

//-----------------------------------------------------------------------------
// Stop the scheduler.
//-----------------------------------------------------------------------------
void vPortEndScheduler( void )
{
    xt_tick_timer_stop();
    port_xSchedulerRunning = 0U;
}


#if ( configNUMBER_OF_CORES > 1 )
//-----------------------------------------------------------------------------
// Starting with port v3.11, the SMP init process is slightly different: only
// core 0 calls main(); other cores enter the scheduler directly via _start().
//
// Since SMP requires coherent shared memory, core 0 must do the following:
// 1) initialize BSS in shared memories, and 
// 2) call __clibrary_init()
//
// Nonzero cores will skip these steps and wait here until core 0 calls
// xPortStartScheduler(), ensuring all cores are synchronized, regardless of
// the order reset was released.
//
// This process is implemented by overriding the __memmap_init() hook in the
// XTOS CRT init sequence.  Note that BSS is not initialized at this point so
// we must only reference initialized global data.  Any systems that require
// additional MMU/TLB setup will need to hook in here as well.
//-----------------------------------------------------------------------------
void __memmap_init(void)
{
    if (portGET_CORE_ID() > 0) {
        portDISABLE_INTERRUPTS();
        while (xt_smp_sync != XT_SMP_SYNC_DONE) {
            // Busy-wait
        }

        // By this point core 0 will have initialized BSS in shared memory
        // but we still need to initialize per-core BSS segments
        __bss_init(&_bss_table_start, &_bss_table_end);

        (void) xPortStartScheduler();

        // Does not return here
        configASSERT( 0 );
    }
}
#endif // ( configNUMBER_OF_CORES > 1 )


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
