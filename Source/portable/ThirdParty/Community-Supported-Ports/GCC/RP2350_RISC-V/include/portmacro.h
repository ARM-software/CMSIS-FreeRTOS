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

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

#include "pico.h"
#include "hardware/sync.h"

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#if __riscv_xlen == 64
    #define portSTACK_TYPE           uint64_t
    #define portBASE_TYPE            int64_t
    #define portUBASE_TYPE           uint64_t
    #define portMAX_DELAY            ( TickType_t ) 0xffffffffffffffffUL
    #define portPOINTER_SIZE_TYPE    uint64_t
#elif __riscv_xlen == 32
    #define portSTACK_TYPE           uint32_t
    // todo amy; these cause compilation failure in the RP2040 FreeRTOS examples; need to go look what the "full" tests do on RISC-V
    //#define portBASE_TYPE            int32_t
    //#define portUBASE_TYPE           uint32_t
    #define portBASE_TYPE            long
    #define portUBASE_TYPE           unsigned long
    #define portMAX_DELAY            ( TickType_t ) 0xffffffffUL
#else /* if __riscv_xlen == 64 */
    #error "Assembler did not define __riscv_xlen"
#endif /* if __riscv_xlen == 64 */

typedef portSTACK_TYPE   StackType_t;
typedef portBASE_TYPE    BaseType_t;
typedef portUBASE_TYPE   UBaseType_t;
typedef portUBASE_TYPE   TickType_t;

/* Legacy type definitions. */
#define portCHAR                   char
#define portFLOAT                  float
#define portDOUBLE                 double
#define portLONG                   long
#define portSHORT                  short

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
 * not need to be guarded with a critical section. */
#define portTICK_TYPE_IS_ATOMIC    1
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH          ( -1 )
#define portTICK_PERIOD_MS        ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#ifdef __riscv_32e
    #define portBYTE_ALIGNMENT    8     /* RV32E uses RISC-V EABI with reduced stack alignment requirements */
#else
    #define portBYTE_ALIGNMENT    16
#endif

/* Multi-core */
#define portMAX_CORE_COUNT    2

/* Check validity of number of cores specified in config */
#if ( configNUMBER_OF_CORES < 1 || portMAX_CORE_COUNT < configNUMBER_OF_CORES )
#error "Invalid number of cores specified in config!"
#endif

#if ( configTICK_CORE < 0 || configTICK_CORE > configNUMBER_OF_CORES )
#error "Invalid tick core specified in config!"
#endif
/* FreeRTOS core id is always zero based, so always 0 if we're running on only one core */
#if configNUMBER_OF_CORES != 1
#define portGET_CORE_ID()    get_core_num()
#else
#define portGET_CORE_ID()    0
#endif

#define portCHECK_IF_IN_ISR()                                                                     \
    ( {                                                                                           \
        uint32_t meicontext;                                                                      \
        __asm volatile ( "csrr %0, %1" : "=r" ( meicontext ): "i" ( RVCSR_MEICONTEXT_OFFSET ):); \
        !( meicontext & RVCSR_MEICONTEXT_NOIRQ_BITS ); } )

void vYieldCore( int xCoreID );
#define portYIELD_CORE( a )                  vYieldCore( a )

/*-----------------------------------------------------------*/

/* Scheduler utilities. */
#if configNUMBER_OF_CORES == 1
#define portTASK_SWITCH_CONTEXT() vTaskSwitchContext()
#else
#define portTASK_SWITCH_CONTEXT() vTaskSwitchContext(portGET_CORE_ID())
#endif

#if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
/* extra assertion */
extern void vPortYield(void);
#define portYIELD()                vPortYield();
#else
#define portYIELD()                __asm volatile ( "ecall" );
#endif /* configSUPPORT_PICO_SYNC_INTEROP */

#define portEND_SWITCHING_ISR( xSwitchRequired ) \
    do                                           \
    {                                            \
        if( xSwitchRequired != pdFALSE )         \
        {                                        \
            traceISR_EXIT_TO_SCHEDULER();        \
            portTASK_SWITCH_CONTEXT();           \
        }                                        \
        else                                     \
        {                                        \
            traceISR_EXIT();                     \
        }                                        \
    } while( 0 )
#define portYIELD_FROM_ISR( x )    portEND_SWITCHING_ISR( x )
/*-----------------------------------------------------------*/

#define portSET_INTERRUPT_MASK()        save_and_disable_interrupts()
#define portCLEAR_INTERRUPT_MASK(state) restore_interrupts( state )
#define portDISABLE_INTERRUPTS()        save_and_disable_interrupts()
#define portENABLE_INTERRUPTS()         restore_interrupts( RVCSR_MIE_MSIE_BITS )

#if ( configNUMBER_OF_CORES == 1 )
extern size_t xCriticalNesting;
#define portGET_CRITICAL_NESTING_COUNT( xCoreID )          xCriticalNesting
#define portENTER_CRITICAL()      \
    {                             \
        portDISABLE_INTERRUPTS(); \
        xCriticalNesting++;       \
    }

#define portEXIT_CRITICAL()          \
    {                                \
        xCriticalNesting--;          \
        if( xCriticalNesting == 0 )  \
        {                            \
            portENABLE_INTERRUPTS(); \
        }                            \
    }
#else /* ( configNUMBER_OF_CORES == 1 ) */
extern size_t xCriticalNestings[ configNUMBER_OF_CORES ];
/*
 * SMP enabled
 */
extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );
extern UBaseType_t vTaskEnterCriticalFromISR( void );
extern void vTaskExitCriticalFromISR( UBaseType_t uxSavedInterruptStatus );
#define portENTER_CRITICAL()               vTaskEnterCritical()
#define portEXIT_CRITICAL()                vTaskExitCritical()
#define portENTER_CRITICAL_FROM_ISR()      vTaskEnterCriticalFromISR()
#define portEXIT_CRITICAL_FROM_ISR( x )    vTaskExitCriticalFromISR( x )

#define portGET_CRITICAL_NESTING_COUNT( xCoreID )          ( xCriticalNestings[ ( xCoreID ) ] )
#define portSET_CRITICAL_NESTING_COUNT( xCoreID, x )       ( xCriticalNestings[ ( xCoreID ) ] = ( x ) )
#define portINCREMENT_CRITICAL_NESTING_COUNT( xCoreID )    ( xCriticalNestings[ ( xCoreID ) ]++ )
#define portDECREMENT_CRITICAL_NESTING_COUNT( xCoreID )    ( xCriticalNestings[ ( xCoreID ) ]-- )

#endif /* if ( configNUMBER_OF_CORES == 1 ) */


/*-----------------------------------------------------------*/

/* Architecture specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
    /* Not supported on SMP */
    #if ( configNUMBER_OF_CORES == 1 )
        #define configUSE_PORT_OPTIMISED_TASK_SELECTION    1
    #endif
#endif

#if ( configUSE_PORT_OPTIMISED_TASK_SELECTION == 1 )

/* Check the configuration. */
    #if ( configMAX_PRIORITIES > 32 )
        #error "configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice."
    #endif

/* Store/clear the ready priorities in a bit map. */
    #define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities )    ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
    #define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities )     ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

/*-----------------------------------------------------------*/

    #define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities )    uxTopPriority = ( 31UL - __builtin_clz( uxReadyPriorities ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */


/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. These are
 * not necessary for to use this port.  They are defined so the common demo
 * files (which build with all the ports) will build. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )    void vFunction( void * pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )          void vFunction( void * pvParameters )

/*-----------------------------------------------------------*/

#define portNOP()    __asm volatile ( " nop " )
#define portINLINE              __inline

#ifndef portFORCE_INLINE
    #define portFORCE_INLINE    inline __attribute__( ( always_inline ) )
#endif

#define portMEMORY_BARRIER()    __asm volatile ( "" ::: "memory" )
/*-----------------------------------------------------------*/

/* Critical section management. */

#define portCRITICAL_NESTING_IN_TCB    0

#define portRTOS_SPINLOCK_COUNT    2

/* Note this is a single method with uxAcquire parameter since we have
 * static vars, the method is always called with a compile time constant for
 * uxAcquire, and the compiler should do the right thing! */
static inline void vPortRecursiveLock( BaseType_t xCoreID,
                                       uint32_t ulLockNum,
                                       spin_lock_t * pxSpinLock,
                                       BaseType_t uxAcquire )
{
    static volatile uint8_t ucOwnedByCore[ portMAX_CORE_COUNT ][portRTOS_SPINLOCK_COUNT];
    static volatile uint8_t ucRecursionCountByLock[ portRTOS_SPINLOCK_COUNT ];

    configASSERT( ulLockNum < portRTOS_SPINLOCK_COUNT );

    if( uxAcquire )
    {
        if (!spin_try_lock_unsafe(pxSpinLock)) {
            if( ucOwnedByCore[ xCoreID ][ ulLockNum ] )
            {
                configASSERT( ucRecursionCountByLock[ ulLockNum ] != 255u );
                ucRecursionCountByLock[ ulLockNum ]++;
                return;
            }
            spin_lock_unsafe_blocking(pxSpinLock);
        }
        configASSERT( ucRecursionCountByLock[ ulLockNum ] == 0 );
        ucRecursionCountByLock[ ulLockNum ] = 1;
        ucOwnedByCore[ xCoreID ][ ulLockNum ] = 1;
    }
    else
    {
        configASSERT( ( ucOwnedByCore[ xCoreID ] [ulLockNum ] ) != 0 );
        configASSERT( ucRecursionCountByLock[ ulLockNum ] != 0 );

        if( !--ucRecursionCountByLock[ ulLockNum ] )
        {
            ucOwnedByCore[ xCoreID ] [ ulLockNum ] = 0;
            spin_unlock_unsafe(pxSpinLock);
        }
    }
}

#if ( configNUMBER_OF_CORES == 1 )
    #define portGET_ISR_LOCK( xCoreID )
    #define portRELEASE_ISR_LOCK( xCoreID )
    #define portGET_TASK_LOCK( xCoreID )
    #define portRELEASE_TASK_LOCK( xCoreID )
#else /* configNUMBER_OF_CORES == 1 */
    #define portGET_ISR_LOCK( xCoreID )         vPortRecursiveLock( ( xCoreID ), 0, spin_lock_instance( configSMP_SPINLOCK_0 ), pdTRUE )
    #define portRELEASE_ISR_LOCK( xCoreID )     vPortRecursiveLock( ( xCoreID ), 0, spin_lock_instance( configSMP_SPINLOCK_0 ), pdFALSE )
    #define portGET_TASK_LOCK( xCoreID )        vPortRecursiveLock( ( xCoreID ), 1, spin_lock_instance( configSMP_SPINLOCK_1 ), pdTRUE )
    #define portRELEASE_TASK_LOCK( xCoreID )    vPortRecursiveLock( ( xCoreID ), 1, spin_lock_instance( configSMP_SPINLOCK_1 ), pdFALSE )
#endif /* configNUMBER_OF_CORES == 1 */

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* PORTMACRO_H */
