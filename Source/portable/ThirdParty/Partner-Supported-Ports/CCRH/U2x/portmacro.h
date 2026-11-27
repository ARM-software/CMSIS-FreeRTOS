/*
 * FreeRTOS Kernel V11.2.0
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

#ifndef PORTMACRO_H
    #define PORTMACRO_H

    #ifdef __cplusplus
        extern "C"
        {
    #endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions - These are a bit legacy and not really used now, other
 * than portSTACK_TYPE and portBASE_TYPE. */
    #define portCHAR              char
    #define portFLOAT             float
    #define portDOUBLE            double
    #define portLONG              long
    #define portSHORT             short
    #define portSTACK_TYPE        uint32_t
    #define portBASE_TYPE         long
    #define portREGISTER_SEL_0    0

    typedef portSTACK_TYPE   StackType_t;
    typedef long             BaseType_t;
    typedef unsigned long    UBaseType_t;

/* Defines the maximum time when using a wait command in a task */
    #if ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS )
        typedef uint16_t     TickType_t;
        #define portMAX_DELAY              ( TickType_t ) 0xffff
    #elif ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS )
        typedef uint32_t     TickType_t;
        #define portMAX_DELAY              ( TickType_t ) 0xffffffffUL

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
 * not need to be guarded with a critical section. */
        #define portTICK_TYPE_IS_ATOMIC    1
    #else
        #error configTICK_TYPE_WIDTH_IN_BITS set to unsupported tick type width.
    #endif

/*-----------------------------------------------------------*/

/* Architecture specifics */
    #define portLDSR( reg, sel, val )    __ldsr_rh( ( reg ), ( sel ), ( val ) )
    #define portSTSR( reg, sel )         __stsr_rh( ( reg ), ( sel ) )
    #define portSYNCM()                  __syncm()

/* Specify 16 interrupt priority levels */
    #define portINT_PRIORITY_HIGHEST    ( 0x0000U ) /* Level 0 (highest) */
    #define portINT_PRIORITY_LEVEL1     ( 0x0001U ) /* Level 1 */
    #define portINT_PRIORITY_LEVEL2     ( 0x0002U ) /* Level 2 */
    #define portINT_PRIORITY_LEVEL3     ( 0x0003U ) /* Level 3 */
    #define portINT_PRIORITY_LEVEL4     ( 0x0004U ) /* Level 4 */
    #define portINT_PRIORITY_LEVEL5     ( 0x0005U ) /* Level 5 */
    #define portINT_PRIORITY_LEVEL6     ( 0x0006U ) /* Level 6 */
    #define portINT_PRIORITY_LEVEL7     ( 0x0007U ) /* Level 7 */
    #define portINT_PRIORITY_LEVEL8     ( 0x0008U ) /* Level 8 */
    #define portINT_PRIORITY_LEVEL9     ( 0x0009U ) /* Level 9 */
    #define portINT_PRIORITY_LEVEL10    ( 0x000AU ) /* Level 10 */
    #define portINT_PRIORITY_LEVEL11    ( 0x000BU ) /* Level 11 */
    #define portINT_PRIORITY_LEVEL12    ( 0x000CU ) /* Level 12 */
    #define portINT_PRIORITY_LEVEL13    ( 0x000DU ) /* Level 13 */
    #define portINT_PRIORITY_LEVEL14    ( 0x000EU ) /* Level 14 */
    #define portINT_PRIORITY_LOWEST     ( 0x000FU ) /* Level 15 (lowest) */

/* Determine the descending of the stack from high address to address */
    #define portSTACK_GROWTH            ( -1 )

/* Determine the time (in milliseconds) corresponding to each tick */
    #define portTICK_PERIOD_MS          ( ( TickType_t ) 1000 / configTICK_RATE_HZ )

/* It is a multiple of 4 (the two lower-order bits of the address = 0),
 * otherwise it will cause MAE (Misaligned Exception) according to the manual */
    #define portBYTE_ALIGNMENT          ( 4 )

/* Interrupt control macros. */

    #define portENABLE_INTERRUPTS()     __EI() /* Macro to enable all maskable interrupts. */
    #define portDISABLE_INTERRUPTS()    __DI() /* Macro to disable all maskable interrupts. */
    #define taskENABLE_INTERRUPTS()     portENABLE_INTERRUPTS()
    #define taskDISABLE_INTERRUPTS()    portDISABLE_INTERRUPTS()

/* SMP build which means configNUM_CORES is relevant */
    #define portSUPPORT_SMP              1

    #ifndef configNUMBER_OF_CORES
        #define configNUMBER_OF_CORES    1
    #endif

/*-----------------------------------------------------------*/
/* Scheduler utilities */

/* Called at the end of an ISR that can cause a context switch */
    extern void vPortSetSwitch( BaseType_t xSwitchRequired );

    #define portEND_SWITCHING_ISR( x )    vPortSetSwitch( x )

    #define portYIELD_FROM_ISR( x )       portEND_SWITCHING_ISR( x )

/* Use to transfer control from one task to perform other tasks of higher priority */
    extern void vPortYield( void );

    #define portYIELD()    vPortYield()

/* Return the core ID on which the code is running. */
    extern BaseType_t xPortGET_CORE_ID();

    #define portGET_CORE_ID()    xPortGET_CORE_ID()
    #define coreid    xPortGET_CORE_ID()

    #if ( configNUMBER_OF_CORES > 1 )

/* Request the core ID x to yield. */
        extern void vPortYieldCore( uint32_t coreID );

        #define portYIELD_CORE( x )                vPortYieldCore( x )

        #define portENTER_CRITICAL_FROM_ISR()      vTaskEnterCriticalFromISR()
        #define portEXIT_CRITICAL_FROM_ISR( x )    vTaskExitCriticalFromISR( x )

    #endif /* if ( configNUMBER_OF_CORES > 1 ) */

    #if ( configNUMBER_OF_CORES == 1 )
        #define portGET_ISR_LOCK( xCoreID )
        #define portRELEASE_ISR_LOCK( xCoreID )
        #define portGET_TASK_LOCK( xCoreID )
        #define portRELEASE_TASK_LOCK( xCoreID )
    #else
        extern void vPortRecursiveLockAcquire( BaseType_t xCoreID,
                                               BaseType_t xFromIsr );
        extern void vPortRecursiveLockRelease( BaseType_t xCoreID,
                                               BaseType_t xFromIsr );

        #define portGET_ISR_LOCK( xCoreID )         vPortRecursiveLockAcquire( ( xCoreID ), pdTRUE )
        #define portRELEASE_ISR_LOCK( xCoreID )     vPortRecursiveLockRelease( ( xCoreID ), pdTRUE )
        #define portGET_TASK_LOCK( xCoreID )        vPortRecursiveLockAcquire( ( xCoreID ), pdFALSE )
        #define portRELEASE_TASK_LOCK( xCoreID )    vPortRecursiveLockRelease( ( xCoreID ), pdFALSE )
    #endif /* if ( configNUMBER_OF_CORES == 1 ) */

/*-----------------------------------------------------------*/
/* Critical section management. */

/* The critical nesting functions defined within tasks.c */

    extern void vTaskEnterCritical( void );
    extern void vTaskExitCritical( void );

/* Macro to mark the start of a critical code region */
    #define portENTER_CRITICAL()    vTaskEnterCritical()

/* Macro to mark the end of a critical code region */
    #define portEXIT_CRITICAL()     vTaskExitCritical()

/*-----------------------------------------------------------*/
/* Macros to set and clear the interrupt mask. */
    portLONG xPortSetInterruptMask();
    void vPortClearInterruptMask( portLONG );

    #define portSET_INTERRUPT_MASK()                  xPortSetInterruptMask()
    #define portCLEAR_INTERRUPT_MASK( x )             vPortClearInterruptMask( ( x ) )
    #define portSET_INTERRUPT_MASK_FROM_ISR()         xPortSetInterruptMask()
    #define portCLEAR_INTERRUPT_MASK_FROM_ISR( x )    vPortClearInterruptMask( ( x ) )

/*-----------------------------------------------------------*/
/* Task function macros as described on the FreeRTOS.org WEB site. */

    #define portTASK_FUNCTION_PROTO( vFunction, pvParameters )    void vFunction( void * pvParameters )
    #define portTASK_FUNCTION( vFunction, pvParameters )          void vFunction( void * pvParameters )

/*-----------------------------------------------------------*/

    #ifdef __cplusplus
    }
    #endif
#endif /* PORTMACRO_H */
