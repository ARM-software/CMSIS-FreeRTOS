/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
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

 //	Copyright: Copyright (C) Texas Instruments Incorporated
//	All rights reserved not granted herein.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//  Redistributions of source code must retain the above copyright 
//  notice, this list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the 
//  documentation and/or other materials provided with the   
//  distribution.
//
//  Neither the name of Texas Instruments Incorporated nor the names of
//  its contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef PORTMACRO_H
#define PORTMACRO_H

//-------------------------------------------------------------------------------------------------
// Port specific definitions.
//
// The settings in this file configure FreeRTOS correctly for the
// given hardware and compiler.
//
// These settings should not be altered.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Hardware includes
//-------------------------------------------------------------------------------------------------
#include "interrupt.h"
#include "cputimer.h"
#include "portdefines.h"

//-------------------------------------------------------------------------------------------------
// Type definitions.
//-------------------------------------------------------------------------------------------------
#define portCHAR        uint8_t
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        uint32_t
#define portSHORT       uint16_t
#define portBASE_TYPE   long
#define portSTACK_TYPE  uint32_t

typedef portSTACK_TYPE  StackType_t;
typedef int32_t         BaseType_t;
typedef uint32_t        UBaseType_t;

#if ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_16_BITS )
    typedef uint16_t        TickType_t;
    #define portMAX_DELAY   ( TickType_t ) 0xffff
#elif ( configTICK_TYPE_WIDTH_IN_BITS == TICK_TYPE_WIDTH_32_BITS )
    typedef uint32_t        TickType_t;
    #define portMAX_DELAY   ( TickType_t ) 0xffffffffUL
#else
    #error configTICK_TYPE_WIDTH_IN_BITS set to unsupported tick type width.
#endif

//-------------------------------------------------------------------------------------------------
// Interrupt control macros.
//-------------------------------------------------------------------------------------------------
#define portDISABLE_INTERRUPTS()  __asm__(" DISINT")
#define portENABLE_INTERRUPTS()   __asm__(" ENINT")

static inline UBaseType_t xPortSetInterruptMask()
{
   volatile UBaseType_t dsts;
   __asm__(" ST.32 *(A15-#8), DSTS");
   __asm__(" DISINT");
   return ( ( dsts >> 16 ) & 1 );
}

#define portCLEAR_INTERRUPT_MASK_FROM_ISR( x )  if( x ) __asm__(" ENINT") // Enable only if x is set
#define portSET_INTERRUPT_MASK_FROM_ISR( )      xPortSetInterruptMask( )

/* Any task that uses the floating point unit MUST call vPortTaskUsesFPU()
before any floating point instructions are executed. */
void vPortTaskUsesFPU( void );

//-------------------------------------------------------------------------------------------------
// Architecture specific optimisations.
//-------------------------------------------------------------------------------------------------
#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

/* Store/clear the ready priorities in a bit map. */
    #define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities )    ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
    #define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities )     ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

/*-----------------------------------------------------------*/

    #define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities )  uxTopPriority = ( 31 - __builtin_c29_i32_clzeros_d( ( uxReadyPriorities ) ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

//-------------------------------------------------------------------------------------------------
// Architecture specific checks.
//-------------------------------------------------------------------------------------------------
#if ( configUSE_MINI_LIST_ITEM == 1 )
    #error configUSE_MINI_LIST_ITEM must be set to 0 for this port.
#endif

//-------------------------------------------------------------------------------------------------
// Critical section control macros.
//-------------------------------------------------------------------------------------------------
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

#define portENTER_CRITICAL()  vPortEnterCritical()
#define portEXIT_CRITICAL()   vPortExitCritical()

//-------------------------------------------------------------------------------------------------
// Task utilities.
//-------------------------------------------------------------------------------------------------
#define portYIELD()               {Interrupt_force(PORT_TASK_SWITCH_INT); asm(" NOP #8"); asm(" NOP #5");}
#define portYIELD_FROM_ISR( x )   if( x ) portYIELD()

extern void portTICK_ISR( void );
extern void vPortYield( void );
extern void portRESTORE_FIRST_CONTEXT( void );
extern void vTaskSwitchContext( void );

//-------------------------------------------------------------------------------------------------
// Hardware specifics.
//-------------------------------------------------------------------------------------------------
#define portBYTE_ALIGNMENT      8
#define portSTACK_GROWTH        ( 1 )
#define portTICK_PERIOD_MS      ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portNOP()               __asm(" NOP")

//-------------------------------------------------------------------------------------------------
// Task function macros as described on the FreeRTOS.org WEB site.
//-------------------------------------------------------------------------------------------------
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )  void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )        void vFunction( void *pvParameters )

#endif /* PORTMACRO_H */