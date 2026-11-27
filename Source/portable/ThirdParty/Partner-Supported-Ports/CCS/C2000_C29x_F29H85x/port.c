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

//-------------------------------------------------------------------------------------------------
// Scheduler includes.
//-------------------------------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
    /* Check the configuration. */
    #if ( configMAX_PRIORITIES > 32 )
        #error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.
    #endif
#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

//-------------------------------------------------------------------------------------------------
// Implementation of functions defined in portable.h for the C29x port.
//-------------------------------------------------------------------------------------------------

// Constants required for hardware setup.
#define portINITIAL_CRITICAL_NESTING    ( ( uint16_t ) 10 )
#define portFLAGS_INT_ENABLED           ( ( StackType_t ) 0x010000 )  // DSTS LP INT ENABLE 

// Register descriptions of C29 Architecture
#define A_REGISTERS             16  // Addressing registers
#define D_REGISTERS             16  // Fixed point registers
#define M_REGISTERS             32  // Floating point registers
#define A4_REGISTER_POSITION    4U

//
// DO NOT MAKE as pdFALSE, the C29 compiler currently uses some FPU registers even when FPU
// operations are not done, hence we should save/restore FPU always
//
#define TASK_HAS_FPU_CONTEXT_ON_TASK_START  pdTRUE

void vPortSetupTimerInterrupt( void );
void vPortSetupSWInterrupt( void );

// Each task maintains a count of the critical section nesting depth.  Each
// time a critical section is entered the count is incremented.  Each time a
// critical section is exited the count is decremented - with interrupts only
// being re-enabled if the count is zero.
//
// ulCriticalNesting will get set to zero when the scheduler starts, but must
// not be initialised to zero as this will cause problems during the startup
// sequence.
//
// ulCriticalNesting should be 32 bit value to keep stack alignment unchanged.
volatile uint32_t ulCriticalNesting = portINITIAL_CRITICAL_NESTING;

// 
// Saved as part of the task context. Value set here has no effect.
// Value set in pxPortInitialiseStack is the initial value when a task starts
// 
uint32_t ulTaskHasFPUContext = TASK_HAS_FPU_CONTEXT_ON_TASK_START;

//-------------------------------------------------------------------------------------------------
// Initialise the stack of a task to look exactly as if
// timer interrupt was executed.
//-------------------------------------------------------------------------------------------------
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    uint16_t i;
    uint16_t base = 0;

    pxTopOfStack[base] = (uint32_t)vPortEndScheduler; 
    base+=1;    // Alignment
    pxTopOfStack[base] = 0x07F90001; 
    base+=1;    // Alignment , DSTS if RETI
    pxTopOfStack[base]  = 0xABABABAB; 
    base+=1 ;   // A14
    pxTopOfStack[base] = ((uint32_t)pxCode) & 0xFFFFFFFEU;
    base+=1;    //RPC or PC (first task) after return
    pxTopOfStack[base]  = 0x07F90001; 
    base+=1;    // DSTS
    pxTopOfStack[base]  = 0x00020101; 
    base+=1;    // ESTS

    // Fill the rest of the registers with dummy values.
    for(i = 0; i <= (A_REGISTERS + D_REGISTERS + M_REGISTERS -2 -1); i++)
    {
        uint32_t value  = 0xDEADDEAD;

        // Function parameters are passed in A4.
        if(i == A4_REGISTER_POSITION)
        {
            value = (uint32_t)pvParameters;
        }
        pxTopOfStack[base + i] = value;
    }
    base += i ;

    // Save (or) Don't save FPU registers when a task starts
    pxTopOfStack[base] = TASK_HAS_FPU_CONTEXT_ON_TASK_START; 
    base+=1;    // FPU context
    base+=1;    // Alignment

    // Return a pointer to the top of the stack we have generated so this can
    // be stored in the task control block for the task.
    return pxTopOfStack + base;
}

//-------------------------------------------------------------------------------------------------
// See header file for description.
//-------------------------------------------------------------------------------------------------
BaseType_t xPortStartScheduler(void)
{
    // Yield interrupt
    vPortSetupSWInterrupt();
    // Tick timer interrupt
    vPortSetupTimerInterrupt();

    ulCriticalNesting = 0;

    portENABLE_INTERRUPTS();
    portRESTORE_FIRST_CONTEXT();

    // This line should not be reached
    return pdFAIL;
}

//-------------------------------------------------------------------------------------------------
void vPortEndScheduler( void )
{
    // It is unlikely that the C29x port will get stopped.
    // If required simply disable the tick interrupt here.
}

//-------------------------------------------------------------------------------------------------
void vPortSetupTimerInterrupt( void )
{
    CPUTimer_stopTimer(PORT_TICK_TIMER_BASE);
    CPUTimer_setPeriod(PORT_TICK_TIMER_BASE, ((uint32_t)((configCPU_CLOCK_HZ / configTICK_RATE_HZ))));
    CPUTimer_setPreScaler(PORT_TICK_TIMER_BASE, 0U);
    CPUTimer_reloadTimerCounter(PORT_TICK_TIMER_BASE);
    CPUTimer_setEmulationMode(PORT_TICK_TIMER_BASE, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_clearOverflowFlag(PORT_TICK_TIMER_BASE);
    CPUTimer_enableInterrupt(PORT_TICK_TIMER_BASE);

    Interrupt_disable(PORT_TICK_TIMER_INT);
    Interrupt_clearFlag(PORT_TICK_TIMER_INT);
    Interrupt_clearOverflowFlag(PORT_TICK_TIMER_INT);
    Interrupt_register(PORT_TICK_TIMER_INT, &portTICK_ISR);
    Interrupt_setPriority(PORT_TICK_TIMER_INT, PORT_TICK_TIMER_INT_PRI);
    Interrupt_enable(PORT_TICK_TIMER_INT);

    CPUTimer_startTimer(PORT_TICK_TIMER_BASE);
}

//-------------------------------------------------------------------------------------------------
void vPortSetupSWInterrupt( void )
{
    Interrupt_disable(PORT_TASK_SWITCH_INT);
    Interrupt_clearFlag(PORT_TASK_SWITCH_INT);
    Interrupt_clearOverflowFlag(PORT_TASK_SWITCH_INT);
    Interrupt_register(PORT_TASK_SWITCH_INT, &vPortYield);
    Interrupt_setPriority(PORT_TASK_SWITCH_INT, PORT_TASK_SWITCH_INT_PRI);
    Interrupt_enable(PORT_TASK_SWITCH_INT);
}

//-------------------------------------------------------------------------------------------------
void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    ulCriticalNesting++;
}

//-------------------------------------------------------------------------------------------------
void vPortExitCritical( void )
{
    ulCriticalNesting--;
    if( ulCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }
}

