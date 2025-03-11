/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: MIT AND BSD-3-Clause
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

/*-----------------------------------------------------------
* Implementation of functions defined in portable.h for the RISC-V port.
*----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "rp2040_config.h"
#include "hardware/clocks.h"
#include "hardware/exception.h"
#include "hardware/irq.h"
#include "portmacro.h"

/* Standard includes. */
#include "string.h"


/* Let the user override the pre-loading of the initial RA. */
#ifdef configTASK_RETURN_ADDRESS
    #define portTASK_RETURN_ADDRESS    configTASK_RETURN_ADDRESS
#else
    #define portTASK_RETURN_ADDRESS    0
#endif

/*
 * LIB_PICO_MULTICORE == 1, if we are linked with pico_multicore (note that
 * the non SMP FreeRTOS_Kernel is not linked with pico_multicore itself). We
 * use this flag to determine if we need multi-core functionality.
 */
#if ( LIB_PICO_MULTICORE == 1 )
#include "pico/multicore.h"
#endif /* LIB_PICO_MULTICORE */

#ifndef configUSE_ISR_STACK
#define configUSE_ISR_STACK 1
#endif

/* The stack used by interrupt service routines.  Set configISR_STACK_SIZE_WORDS
 * to use a statically allocated array as the interrupt stack.  Alternative leave
 * configISR_STACK_SIZE_WORDS undefined and update the linker script so that a
 * linker variable names __freertos_irq_stack_top has the same value as the top
 * of the stack used by main.  Using the linker script method will repurpose the
 * stack that was used by main before the scheduler was started for use as the
 * interrupt stack after the scheduler has started. */
#ifdef configISR_STACK_SIZE_WORDS
#error configISR_STACK_SIZE_WORDS not yet supported; we need per core ones
static __attribute__( ( aligned( 16 ) ) ) StackType_t xISRStack[ configISR_STACK_SIZE_WORDS ] = { 0 };
const StackType_t xISRStackTop = ( StackType_t ) &( xISRStack[ configISR_STACK_SIZE_WORDS & ~portBYTE_ALIGNMENT_MASK ] );

/* Don't use 0xa5 as the stack fill bytes as that is used by the kernel for
 * the task stacks, and so will legitimately appear in many positions within
 * the ISR stack. */
    #define portISR_STACK_FILL_BYTE    0xee
#else
    /*
     * If these are 0, then no stack switch is performed.
     *
     * Note: if configUSE_ISR_STACK == 0, then this is never initialized,
     * but we keep this array around, because portASM.S / portContext.h does not
     * have access to config variables
     */
    StackType_t xISRStackTops[NUM_CORES] = {0};
#endif

#if ( configSUPPORT_PICO_SYNC_INTEROP == 1 || configNUMBER_OF_CORES > 1 )
    #include "hardware/irq.h"
#endif /* ( configSUPPORT_PICO_SYNC_INTEROP == 1 || configNUMBER_OF_CORES > 1 ) */
#if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
    #include "pico/lock_core.h"
    #include "event_groups.h"
    #if configSUPPORT_STATIC_ALLOCATION
        static StaticEventGroup_t xStaticEventGroup;
        #define pEventGroup    ( &xStaticEventGroup )
    #endif /* configSUPPORT_STATIC_ALLOCATION */
    static EventGroupHandle_t xEventGroup;
    #if ( configNUMBER_OF_CORES == 1 )
        static EventBits_t uxCrossCoreEventBits;
        static spin_lock_t * pxCrossCoreSpinLock; /* protects uxCrossCoreEventBits */
    #endif
#endif /* configSUPPORT_PICO_SYNC_INTEROP */

/* Note this is an SDK IRQ priority which use the ARM convention */
#define portMIN_INTERRUPT_PRIORITY            ( 255UL )

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void ) __attribute__( ( weak ) );

/*-----------------------------------------------------------*/

/* Used to program the machine timer compare register. */
uint64_t ullNextTime = 0ULL;
const uint64_t * pullNextTime = &ullNextTime; /* used by ASM code */
size_t uxTimerIncrementsForOneTick;
volatile uint64_t * pullMachineTimerCompareRegister = NULL;

/* Holds the critical nesting value - deliberately non-zero at start up to
 * ensure interrupts are not accidentally enabled before the scheduler starts. */
typedef struct tskTaskControlBlock TCB_t;
#if configNUMBER_OF_CORES == 1
size_t xCriticalNesting = ( size_t ) 0xaaaaaaaa;
size_t *xCriticalNestingArray = &xCriticalNesting;
extern portDONT_DISCARD PRIVILEGED_DATA TCB_t * volatile pxCurrentTCB;
TCB_t * volatile * pxCurrentTCBArray = &pxCurrentTCB;
static uint32_t zero; /* todo we could use a pre-existing zero elsewhere! */
uint32_t * pxCurrentCoreID = &zero;
#else
size_t xCriticalNestings[ configNUMBER_OF_CORES ] = { 0 };
size_t *xCriticalNestingArray = &xCriticalNestings[0];
extern portDONT_DISCARD PRIVILEGED_DATA TCB_t * volatile pxCurrentTCBs[ configNUMBER_OF_CORES ];
TCB_t * volatile * pxCurrentTCBArray = pxCurrentTCBs;
const volatile uint32_t * pxCurrentCoreID = &sio_hw->cpuid;
#endif

/* Used to catch tasks that attempt to return from their implementing function. */
size_t xTaskReturnAddress = ( size_t ) portTASK_RETURN_ADDRESS;

/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
 * stack checking.  A problem in the ISR stack will trigger an assert, not call
 * the stack overflow hook function (because the stack overflow hook is specific
 * to a task stack, not the ISR stack). */
#if defined( configISR_STACK_SIZE_WORDS ) && ( configCHECK_FOR_STACK_OVERFLOW > 2 )
    #warning "This path not tested, or even compiled yet."

    static const uint8_t ucExpectedStackBytes[] =
    {
        portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, \
        portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, \
        portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, \
        portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, \
        portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE
    }; \

    #define portCHECK_ISR_STACK()    configASSERT( ( memcmp( ( void * ) xISRStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) == 0 ) )
#else /* if defined( configISR_STACK_SIZE_WORDS ) && ( configCHECK_FOR_STACK_OVERFLOW > 2 ) */
    /* Define the function away. */
    #define portCHECK_ISR_STACK()
#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */

#define INVALID_PRIMARY_CORE_NUM    0xffu
/* The primary core number (the own which has the SysTick handler) */
static uint8_t ucPrimaryCoreNum = INVALID_PRIMARY_CORE_NUM;
/* Initialize to -1 so that when using INTEROP with SDK and running on one core only, we can tell if this is initialized yet */
static int8_t cDoorbellNum = -1;

/* Note: portIS_FREE_RTOS_CORE() also returns false until the scheduler is started */
#if ( configNUMBER_OF_CORES != 1 )
#define portIS_FREE_RTOS_CORE()    ( ucPrimaryCoreNum != INVALID_PRIMARY_CORE_NUM )
#else
#define portIS_FREE_RTOS_CORE()    ( ucPrimaryCoreNum == get_core_num() )
#endif

/*-----------------------------------------------------------*/

#include "hardware/riscv_platform_timer.h"

void vPortSetupTimerInterrupt( void )
{
    riscv_timer_set_fullspeed(true);
    uint64_t ullCurrentTime = riscv_timer_get_mtime();
    uxTimerIncrementsForOneTick = ( size_t ) ( ( clock_get_hz( clk_sys ) ) / ( configTICK_RATE_HZ ) ); /* Assumes increment won't go over 32-bits. */

    pullMachineTimerCompareRegister = ( volatile uint64_t * ) &sio_hw->mtimecmp;

    ullNextTime = ullCurrentTime + uxTimerIncrementsForOneTick;
    *pullMachineTimerCompareRegister = ullNextTime;

    /* Prepare the time to use after the next tick interrupt. */
    ullNextTime += uxTimerIncrementsForOneTick;

    extern void freertos_risc_v_mtimer_interrupt_handler(void);
#if 1
    irq_set_exclusive_handler(SIO_IRQ_MTIMECMP, freertos_risc_v_mtimer_interrupt_handler);
    irq_set_enabled(SIO_IRQ_MTIMECMP, true);
#else
    riscv_set_csr(mie, RVCSR_MIE_MTIE_BITS);
#endif
}

void vPortInstallVectorTableHandlers(void) {
    extern void freertos_risc_v_trap_handler(void);
    extern void freertos_risc_v_interrupt_handler(void);
    irq_set_riscv_vector_handler(RISCV_VEC_MACHINE_EXCEPTION, freertos_risc_v_trap_handler);
    irq_set_riscv_vector_handler(RISCV_VEC_MACHINE_EXTERNAL_IRQ, freertos_risc_v_interrupt_handler);
}

StackType_t xPortInitISRStack(void) {
    StackType_t xISRStackTop;
    //( StackType_t ) /*__freertos_irq_stack_top*/__StackTop;
    // todo fixup for allocated stacks
    if (get_core_num())
    {
        extern const uint32_t __StackOneTop[];
        xISRStackTop = ( StackType_t ) __StackOneTop;
    }
    else
    {
        extern const uint32_t __StackTop[];
        xISRStackTop = ( StackType_t ) __StackTop;
    }
    xISRStackTops[get_core_num()] = xISRStackTop;
    return xISRStackTop;
}
/*-----------------------------------------------------------*/

#if ( LIB_PICO_MULTICORE == 1 ) && ( configSUPPORT_PICO_SYNC_INTEROP == 1 || configNUMBER_OF_CORES > 1)
    static void prvDoorbellInterruptHandler()
    {
        if (cDoorbellNum >= 0 && multicore_doorbell_is_set_current_core(cDoorbellNum))
        {
            multicore_doorbell_clear_current_core(cDoorbellNum);
            #if ( configNUMBER_OF_CORES != 1 )
                portYIELD_FROM_ISR( pdTRUE );
            #elif ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                uint32_t ulSave = spin_lock_blocking( pxCrossCoreSpinLock );
                EventBits_t ulBits = uxCrossCoreEventBits;
                uxCrossCoreEventBits &= ~ulBits;
                spin_unlock( pxCrossCoreSpinLock, ulSave );
                xEventGroupSetBitsFromISR( xEventGroup, ulBits, &xHigherPriorityTaskWoken );
                portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
            #endif /* configNUMBER_OF_CORES != 1 */
        }
    }
#endif /* ( LIB_PICO_MULTICORE == 1 ) && ( configSUPPORT_PICO_SYNC_INTEROP == 1 || configNUMBER_OF_CORES > 1) */
/*-----------------------------------------------------------*/

#if ( configNUMBER_OF_CORES != 1 )
    static BaseType_t xPortStartSchedulerOnCore( void )
    {
        extern void xPortStartFirstTask( void );
        #if ( configUSE_ISR_STACK == 1)
            StackType_t xISRStackTop = xPortInitISRStack();

            #if ( configASSERT_DEFINED == 1 )
                {
                    /* Check alignment of the interrupt stack - which is the same as the
                     * stack that was being used by main() prior to the scheduler being
                     * started. */
                    configASSERT( ( xISRStackTop & portBYTE_ALIGNMENT_MASK ) == 0 );

                    #ifdef configISR_STACK_SIZE_WORDS
                    {
                        memset( ( void * ) xISRStack, portISR_STACK_FILL_BYTE, sizeof( xISRStack ) );
                    }
                    #endif /* configISR_STACK_SIZE_WORDS */
                }
            #endif /* configASSERT_DEFINED */
        #endif /* configUSE_ISR_STACK */

        if( ucPrimaryCoreNum == get_core_num() ) {
            /* If there is a CLINT then it is ok to use the default implementation
             * in this file, otherwise vPortSetupTimerInterrupt() must be implemented to
             * configure whichever clock is to be used to generate the tick interrupt. */
            vPortSetupTimerInterrupt();

            //    extern void freertos_risc_v_trap_handler(void);
            //    exception_set_exclusive_handler(0, freertos_risc_v_trap_handler);
            #if ((configMTIME_BASE_ADDRESS != 0) && (configMTIMECMP_BASE_ADDRESS != 0))
                {
                    /* Enable mtime and external interrupts.  1<<7 for timer interrupt,
                     * 1<<11 for external interrupt.  _RB_ What happens here when mtime is
                     * not present as with pulpino? */
                    __asm volatile ( "csrs mie, %0" ::"r" ( 0x880 ) );
                }
            #endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) */
        }

        /* Install Doorbell handler to receive interrupt from other core */
        uint32_t irq_num = multicore_doorbell_irq_num(cDoorbellNum);
        irq_set_priority( irq_num, portMIN_INTERRUPT_PRIORITY );
        irq_set_exclusive_handler( irq_num, prvDoorbellInterruptHandler );
        irq_set_enabled( irq_num, 1 );

        xPortStartFirstTask();

        /* Should not get here as after calling xPortStartFirstTask() only tasks
         * should be executing. */
        return pdFAIL;
    }

    static void prvDisableInterruptsAndPortStartSchedulerOnCore( void )
    {
        portDISABLE_INTERRUPTS();
        xPortStartSchedulerOnCore();
    }

    BaseType_t xPortStartScheduler( void )
    {
        configASSERT( ucPrimaryCoreNum == INVALID_PRIMARY_CORE_NUM );

        vPortInstallVectorTableHandlers();

        /* No one else should use these! */
        spin_lock_claim( configSMP_SPINLOCK_0 );
        spin_lock_claim( configSMP_SPINLOCK_1 );

        // claim same number of both cores for simplicity
        cDoorbellNum = (int8_t)multicore_doorbell_claim_unused(0b11, true);
        multicore_doorbell_clear_current_core(cDoorbellNum);
        multicore_doorbell_clear_other_core(cDoorbellNum);

        ucPrimaryCoreNum = configTICK_CORE;
        configASSERT( get_core_num() == 0 ); /* we must be started on core 0 */
        multicore_reset_core1();
        multicore_launch_core1( prvDisableInterruptsAndPortStartSchedulerOnCore );
        xPortStartSchedulerOnCore();

        /* Should not get here! */
        return 0;
    }
#else /* if ( configNUMBER_OF_CORES != 1 ) */
    BaseType_t xPortStartScheduler( void )
    {
        extern void xPortStartFirstTask( void );

        vPortInstallVectorTableHandlers();

        #if ( configUSE_ISR_STACK == 1)
            StackType_t xISRStackTop = xPortInitISRStack();

            #if ( configASSERT_DEFINED == 1 )
            {
                /* Check alignment of the interrupt stack - which is the same as the
                 * stack that was being used by main() prior to the scheduler being
                 * started. */
                configASSERT( ( xISRStackTop & portBYTE_ALIGNMENT_MASK ) == 0 );

                #ifdef configISR_STACK_SIZE_WORDS
                {
                    memset( ( void * ) xISRStack, portISR_STACK_FILL_BYTE, sizeof( xISRStack ) );
                }
                #endif /* configISR_STACK_SIZE_WORDS */
            }
            #endif /* configASSERT_DEFINED */
        #endif /* configUSE_ISR_STACK */

        /* If there is a CLINT then it is ok to use the default implementation
         * in this file, otherwise vPortSetupTimerInterrupt() must be implemented to
         * configure whichever clock is to be used to generate the tick interrupt. */
        vPortSetupTimerInterrupt();

    //    extern void freertos_risc_v_trap_handler(void);
    //    exception_set_exclusive_handler(0, freertos_risc_v_trap_handler);
        #if ( ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) )
        {
            /* Enable mtime and external interrupts.  1<<7 for timer interrupt,
             * 1<<11 for external interrupt.  _RB_ What happens here when mtime is
             * not present as with pulpino? */
            __asm volatile ( "csrs mie, %0" ::"r" ( 0x880 ) );
        }
        #endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) */

        ucPrimaryCoreNum = get_core_num();
        #if ( LIB_PICO_MULTICORE == 1 )
            #if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
                // claim same number of both cores for simplicity
                cDoorbellNum = (int8_t) multicore_doorbell_claim_unused(0b11, true);
                multicore_doorbell_clear_current_core(cDoorbellNum);
                multicore_doorbell_clear_other_core(cDoorbellNum);
                uint32_t irq_num = multicore_doorbell_irq_num(cDoorbellNum);
                irq_set_priority( irq_num, portMIN_INTERRUPT_PRIORITY );
                irq_set_exclusive_handler( irq_num, prvDoorbellInterruptHandler );
                irq_set_enabled( irq_num, 1 );
            #endif
        #endif

        xPortStartFirstTask();

        /* Should not get here as after calling xPortStartFirstTask() only tasks
         * should be executing. */
        return pdFAIL;
    }
#endif /* ( configNUMBER_OF_CORES != 1 ) */
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented. */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

#if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
void vPortYield( void )
{
    __asm volatile ( "ecall" );
}
#endif /* configSUPPORT_PICO_SYNC_INTEROP */

/*-----------------------------------------------------------*/
#if ( configSUPPORT_PICO_SYNC_INTEROP == 1 ) || ( configSUPPORT_PICO_TIME_INTEROP == 1 )
    static TickType_t prvGetTicksToWaitBefore( absolute_time_t t )
    {
        int64_t xDelay = absolute_time_diff_us( get_absolute_time(), t );
        const uint32_t ulTickPeriod = 1000000 / configTICK_RATE_HZ;

        xDelay += ulTickPeriod - 1;

        if( xDelay >= ulTickPeriod )
        {
            return xDelay / ulTickPeriod;
        }

        return 0;
    }
#endif /* if ( configSUPPORT_PICO_SYNC_INTEROP == 1 ) || ( configSUPPORT_PICO_TIME_INTEROP == 1 ) */

#if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
    uint32_t ulPortLockGetCurrentOwnerId()
    {
        if( portIS_FREE_RTOS_CORE() )
        {
            uint32_t exception = __get_current_exception();

            if( !exception )
            {
                return ( uintptr_t ) xTaskGetCurrentTaskHandle();
            }

            /* Note: since ROM as at 0x00000000, these can't be confused with
             * valid task handles (pointers) in RAM */
            /* We make all exception handler/core combinations distinct owners */
            return get_core_num() + exception * 2;
        }

        /* Note: since ROM as at 0x00000000, this can't be confused with
         * valid task handles (pointers) in RAM */
        return get_core_num();
    }

    static inline EventBits_t prvGetEventGroupBit( spin_lock_t * spinLock )
    {
        uint32_t ulBit;

        #if ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS )
            ulBit = 1u << ( spin_lock_get_num( spinLock ) & 0x7u );
        #elif ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS )
            ulBit = 1u << ( spin_lock_get_num( spinLock ) % 24 );
        #endif /* configTICK_TYPE_WIDTH_IN_BITS */
        return ( EventBits_t ) ulBit;
    }

    static inline EventBits_t prvGetAllEventGroupBits()
    {
        #if ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS )
            return ( EventBits_t ) 0xffu;
        #elif ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS )
            return ( EventBits_t ) 0xffffffu;
        #endif /* configTICK_TYPE_WIDTH_IN_BITS */
    }

    void vPortLockInternalSpinUnlockWithWait( struct lock_core * pxLock,
                                              uint32_t ulSave )
    {
        configASSERT( !portCHECK_IF_IN_ISR() );
        configASSERT( pxLock->spin_lock );

        if( !portIS_FREE_RTOS_CORE() )
        {
            spin_unlock( pxLock->spin_lock, ulSave );
            __wfe();
        }
        else
        {
            /* The requirement (from the SDK) on this implementation is that this method
             * should always wake up from a corresponding call to vPortLockInternalSpinUnlockWithNotify
             * that happens after this method is called.
             *
             * The moment that we unlock the spin lock, we need to be sure that
             * there is no way that we end up blocking in xEventGroupWaitBits,
             * despite the fact that other tasks can now run, if the corresponding
             * unlock has occurred.
             *
             * Previously the RP2xxx ports used to attempt to disable IRQs until the
             * task actually (potentially) became blocked by hooking the IRQ re-enable
             * when xEventGroupWaitBits completes (or switches tasks), but this
             * was a broken hack, in that IRQs are re-enabled at other points during
             * that call.
             *
             * This deferred IRQ enable is not actually needed, because all we
             * care about is that:
             *
             * Even in the presence of other tasks acquiring then releasing
             * the lock, between the interrupt_enable and the xEventGroupWaitBits,
             * the corresponding bit will still be set.
             *
             * This is the case, even any intervening blocked lock (which
             * clears the event bit) will need to unlock it before we proceed,
             * which will set the event bit again.
             *
             * The multiplexing down of multiple spin lock numbers to fewer
             * event bits does not cause a possible race condition,
             * but it does mean that a task waiting for lock A can be
             * blocked by a task B which owns another lock.
             *
             * This could be fixed by using an array of event groups, however
             * since the SDK spin locks are generally intended for very short
             * term usage anyway, and rarely nested except in exotic cases
             * like video output, we'll leave it as one event group for now
             */
            spin_unlock( pxLock->spin_lock, ulSave);
            xEventGroupWaitBits( xEventGroup, prvGetEventGroupBit( pxLock->spin_lock ),
                                 pdTRUE, pdFALSE, portMAX_DELAY );
        }
    }

    void vPortLockInternalSpinUnlockWithNotify( struct lock_core * pxLock,
                                                uint32_t ulSave )
    {
        EventBits_t uxBits = prvGetEventGroupBit( pxLock->spin_lock );

        if( portIS_FREE_RTOS_CORE() )
        {
            #if LIB_PICO_MULTICORE
                /* signal an event in case a regular core is waiting */
                __sev();
            #endif
            spin_unlock( pxLock->spin_lock, ulSave );

            if( !portCHECK_IF_IN_ISR() )
            {
                xEventGroupSetBits( xEventGroup, uxBits );
            }
            else
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xEventGroupSetBitsFromISR( xEventGroup, uxBits, &xHigherPriorityTaskWoken );
                portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
            }
        }
        else
        {
            __sev();
            #if ( configNUMBER_OF_CORES == 1 )
                if( pxCrossCoreSpinLock != pxLock->spin_lock )
                {
                    spin_lock_unsafe_blocking( pxCrossCoreSpinLock );
                    uxCrossCoreEventBits |= uxBits;
                    spin_unlock_unsafe( pxCrossCoreSpinLock );
                }
                else
                {
                    uxCrossCoreEventBits |= uxBits;
                }

                /* This causes doorbell irq on the other (FreeRTOS) core which will do the set the event bits */
                if ( cDoorbellNum >= 0 )
                {
                    multicore_doorbell_set_other_core(cDoorbellNum);
                }
            #endif /* configNUMBER_OF_CORES == 1 */
            spin_unlock( pxLock->spin_lock, ulSave );
        }
    }

    bool xPortLockInternalSpinUnlockWithBestEffortWaitOrTimeout( struct lock_core * pxLock,
                                                                 uint32_t ulSave,
                                                                 absolute_time_t uxUntil )
    {
        configASSERT( !portCHECK_IF_IN_ISR() );
        configASSERT( pxLock->spin_lock );

        /* note no need to check LIB_PICO_MULTICORE, as this is always returns true if that is not defined */
        if( !portIS_FREE_RTOS_CORE() )
        {
            spin_unlock( pxLock->spin_lock, ulSave );
            return best_effort_wfe_or_timeout( uxUntil );
        }
        else
        {
            configASSERT( portIS_FREE_RTOS_CORE() );

            TickType_t uxTicksToWait = prvGetTicksToWaitBefore( uxUntil );

            if( uxTicksToWait )
            {
                /* See comment in vPortLockInternalSpinUnlockWithWait for detail
                 * about possible race conditions */
                spin_unlock( pxLock->spin_lock, ulSave );
                xEventGroupWaitBits( xEventGroup,
                                     prvGetEventGroupBit( pxLock->spin_lock ), pdTRUE,
                                     pdFALSE, uxTicksToWait );
            }
            else
            {
                spin_unlock( pxLock->spin_lock, ulSave );
            }

            if( time_reached( uxUntil ) )
            {
                return true;
            }
            else
            {
                /* We do not want to hog the core */
                portYIELD();
                /* We aren't sure if we've reached the timeout yet; the caller will check */
                return false;
            }
        }
    }

    #if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
        /* runs before main */
        static void __attribute__( ( constructor ) ) prvRuntimeInitializer( void )
        {
            /* This must be done even before the scheduler is started, as the spin lock
             * is used by the overrides of the SDK wait/notify primitives */
            #if ( configNUMBER_OF_CORES == 1 )
                pxCrossCoreSpinLock = spin_lock_instance( next_striped_spin_lock_num() );
            #endif /* configNUMBER_OF_CORES == 1 */

            /* The event group is not used prior to scheduler init, but is initialized
             * here to since it logically belongs with the spin lock */
            #if ( configSUPPORT_STATIC_ALLOCATION == 1 )
                xEventGroup = xEventGroupCreateStatic( &xStaticEventGroup );
            #else

                /* Note that it is slightly dubious calling this here before the scheduler is initialized,
                 * however the only thing it touches is the allocator which then calls vPortEnterCritical
                 * and vPortExitCritical, and allocating here saves us checking the one time initialized variable in
                 * some rather critical code paths */
                xEventGroup = xEventGroupCreate();
            #endif /* configSUPPORT_STATIC_ALLOCATION */
        }
    #endif /* if ( configSUPPORT_PICO_SYNC_INTEROP == 1 ) */
#endif /* configSUPPORT_PICO_SYNC_INTEROP */

#if ( configSUPPORT_PICO_TIME_INTEROP == 1 )
    void xPortSyncInternalYieldUntilBefore( absolute_time_t t )
    {
        TickType_t uxTicksToWait = prvGetTicksToWaitBefore( t );

        if( uxTicksToWait )
        {
            vTaskDelay( uxTicksToWait );
        }
    }
#endif /* configSUPPORT_PICO_TIME_INTEROP */

/*-----------------------------------------------------------*/

#if ( configNUMBER_OF_CORES != 1 )
    void vYieldCore( int xCoreID )
    {
        /* Remove warning if configASSERT is not defined.
         * xCoreID is not used in this function due to this is a dual-core system. The yielding core must be different from the current core. */
        ( void ) xCoreID;

        configASSERT( xCoreID != ( int ) portGET_CORE_ID() );

    #if ( configNUMBER_OF_CORES != 1 )

        /* will cause interrupt on other core if not already pending */
        configASSERT( cDoorbellNum >= 0);
        multicore_doorbell_set_other_core(cDoorbellNum);
    #endif
    }
#endif // ( configNUMBER_OF_CORES != 1 )

/*-----------------------------------------------------------*/

/* todo need to move portASM.s back into portasm.c so we can  inline this */
UBaseType_t callTaskEnterCriticalFromISR(void) {
    /** todo note on ARM port we call this anyway for single core, but it seems unnecessary */
    #if ( configNUMBER_OF_CORES > 1 )
        return taskENTER_CRITICAL_FROM_ISR();
    #else
        return 0;
    #endif
}

/* todo need to move portASM.s back into portasm.c so we can  inline this */
void callTaskExitCriticalFromISR(UBaseType_t uxSavedInterruptStatus) {
    /** todo note on ARM port we call this anyway for single core, but it seems unnecessary */
    #if ( configNUMBER_OF_CORES > 1 )
        taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );
    #endif
}