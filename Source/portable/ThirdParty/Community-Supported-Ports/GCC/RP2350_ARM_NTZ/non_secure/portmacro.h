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
 * The settings in this file configure FreeRTOS correctly for the given hardware
 * and compiler.
 *
 * These settings should not be altered.
 *------------------------------------------------------------------------------
 */

/**
 * Architecture specifics.
 */
#define portARCH_NAME                    "Cortex-M33"
#define portHAS_ARMV8M_MAIN_EXTENSION    1
#define portARMV8M_MINOR_VERSION         0
#define portDONT_DISCARD                 __attribute__( ( used ) )
/*-----------------------------------------------------------*/

/* ARMv8-M common port configurations. */
#include "portmacrocommon.h"
/*-----------------------------------------------------------*/

#ifndef configENABLE_MVE
    #define configENABLE_MVE    0
#elif( configENABLE_MVE != 0 )
    #error configENABLE_MVE must be left undefined, or defined to 0 for the Cortex-M33.
#endif
/*-----------------------------------------------------------*/

/**
 * @brief Critical section management.
 */
#define portSET_INTERRUPT_MASK()        ulSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK(state) vClearInterruptMask( state )
#define portDISABLE_INTERRUPTS()        portSET_INTERRUPT_MASK()
#define portENABLE_INTERRUPTS()         portCLEAR_INTERRUPT_MASK( 0 )

#if ( configNUMBER_OF_CORES == 1 )
    #define portSET_INTERRUPT_MASK_FROM_ISR()         ulSetInterruptMask()
    #define portCLEAR_INTERRUPT_MASK_FROM_ISR( x )    vClearInterruptMask( x )
    #define portENTER_CRITICAL()                      vPortEnterCritical()
    #define portEXIT_CRITICAL()                       vPortExitCritical()
#else /* ( configNUMBER_OF_CORES == 1 ) */
    /* SMP enabled */
    #undef portENTER_CRITICAL
    #undef portEXIT_CRITICAL

    extern void vTaskEnterCritical( void );
    extern void vTaskExitCritical( void );
    extern UBaseType_t vTaskEnterCriticalFromISR( void );
    extern void vTaskExitCriticalFromISR( UBaseType_t uxSavedInterruptStatus );

    #define portENTER_CRITICAL()               vTaskEnterCritical()
    #define portEXIT_CRITICAL()                vTaskExitCritical()
    #define portENTER_CRITICAL_FROM_ISR()      vTaskEnterCriticalFromISR()
    #define portEXIT_CRITICAL_FROM_ISR( x )    vTaskExitCriticalFromISR( x )
#endif /* if ( configNUMBER_OF_CORES != 1 ) */

/*-----------------------------------------------------------*/

#ifdef configUSE_DCP_SAVE_RESTORE
#define portUSE_DCP_SAVE_RESTORE configUSE_DCP_SAVE_RESTORE
#elif (LIB_PICO_DOUBLE_PICO/* || LIB_PICO_FLOAT_PICO*/) // todo we don't use DCP in float yet
#define portUSE_DCP_SAVE_RESTORE 1
#endif

#if portUSE_DCP_SAVE_RESTORE
#define portSTACK_LIMIT_PADDING 6
#endif

/*-----------------------------------------------------------*/

/* Exception handlers */
#if ( configUSE_DYNAMIC_EXCEPTION_HANDLERS == 0 )
/* We only need to override the SDK's weak functions if we want to replace them at compile time */
#define SVC_Handler        isr_svcall
#define PendSV_Handler     isr_pendsv
#define SysTick_Handler    isr_systick
#endif

/*-----------------------------------------------------------*/

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

#define portCHECK_IF_IN_ISR()                                 \
    ( {                                                       \
        uint32_t ulIPSR;                                      \
        __asm volatile ( "mrs %0, IPSR" : "=r" ( ulIPSR )::); \
        ( ( uint8_t ) ulIPSR ) > 0; } )

void vYieldCore( int xCoreID );
#define portYIELD_CORE( a )                  vYieldCore( a )

/*-----------------------------------------------------------*/

/* Critical nesting count management. */
#if configNUMBER_OF_CORES == 1
#define portGET_CRITICAL_NESTING_COUNT( xCoreID )          ulCriticalNesting
#else
extern volatile uint32_t ulCriticalNestings[ configNUMBER_OF_CORES ];
#define portGET_CRITICAL_NESTING_COUNT( xCoreID )          ( ulCriticalNestings[ ( xCoreID ) ] )
#define portSET_CRITICAL_NESTING_COUNT( xCoreID, x )       ( ulCriticalNestings[ ( xCoreID ) ] = ( x ) )
#define portINCREMENT_CRITICAL_NESTING_COUNT( xCoreID )    ( ulCriticalNestings[ ( xCoreID ) ]++ )
#define portDECREMENT_CRITICAL_NESTING_COUNT( xCoreID )    ( ulCriticalNestings[ ( xCoreID ) ]-- )
#endif
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
