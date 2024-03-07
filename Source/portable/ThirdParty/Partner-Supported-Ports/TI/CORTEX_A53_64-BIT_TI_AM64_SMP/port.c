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
/*
 *  Copyright (C) 2018-2022 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#include <kernel/dpl/ClockP.h>
#include <drivers/hw_include/cslr.h>

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
	/* Check the configuration. */
	#if( configMAX_PRIORITIES > 32 )
		#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
	#endif
#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/* A critical section is exited when the critical section nesting count reaches
this value. */
#define portNO_CRITICAL_NESTING			( ( size_t ) 0 )

/* Tasks are not created with a floating point context, but can be given a
floating point context after they have been created.  A variable is stored as
part of the tasks context that holds portNO_FLOATING_POINT_CONTEXT if the task
does not have an FPU context, or any other value if the task does have an FPU
context. */
#define portNO_FLOATING_POINT_CONTEXT	( ( StackType_t ) 0 )

/* Constants required to setup the initial task context. */
#define portSP_EL0						( ( StackType_t ) 0x00 )
#define portEL1						    ( ( StackType_t ) 0x04 )
#define portINITIAL_PSTATE				( portEL1 | portSP_EL0 )

/* Masks all bits in the APSR other than the mode bits. */
#define portAPSR_MODE_BITS_MASK			( 0x0C )

/*
 * Starts the first task executing.  This function is necessarily written in
 * assembly code so is implemented in portASM.s.
 */
extern void vPortRestoreTaskContext( void );

/* Saved as part of the task context.  If ullPortTaskHasFPUContext is non-zero
then floating point context must be saved and restored for the task. */
volatile uint64_t ullPortTaskHasFPUContext[configNUM_CORES] = {pdFALSE};

/* Set to 1 to pend a context switch from an ISR. */
uint64_t ullPortYieldRequired[configNUM_CORES] = { pdFALSE };

/* Counts the interrupt nesting depth.  A context switch is only performed if
if the nesting depth is 0. */
uint64_t ullPortInterruptNesting[configNUM_CORES] = { 0 };

/* flag to control tick ISR handling, this is made true just before schedular start */
volatile uint64_t ullPortSchedularRunning = pdFALSE;

#define portPRIORITY_SHIFT 4
__attribute__(( used )) const uint64_t ullMaxAPIPriorityMask = ( configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT );

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Setup the initial stack of the task.  The stack is set exactly as
	expected by the portRESTORE_CONTEXT() macro. */

	/* First all the general purpose registers. */
	pxTopOfStack--;
	*pxTopOfStack = 0x0101010101010101ULL;	/* R1 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0303030303030303ULL;	/* R3 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0202020202020202ULL;	/* R2 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0505050505050505ULL;	/* R5 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0404040404040404ULL;	/* R4 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0707070707070707ULL;	/* R7 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0606060606060606ULL;	/* R6 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0909090909090909ULL;	/* R9 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0808080808080808ULL;	/* R8 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1111111111111111ULL;	/* R11 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1010101010101010ULL;	/* R10 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1313131313131313ULL;	/* R13 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1212121212121212ULL;	/* R12 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1515151515151515ULL;	/* R15 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1414141414141414ULL;	/* R14 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1717171717171717ULL;	/* R17 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1616161616161616ULL;	/* R16 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1919191919191919ULL;	/* R19 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1818181818181818ULL;	/* R18 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2121212121212121ULL;	/* R21 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2020202020202020ULL;	/* R20 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2323232323232323ULL;	/* R23 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2222222222222222ULL;	/* R22 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2525252525252525ULL;	/* R25 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2424242424242424ULL;	/* R24 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2727272727272727ULL;	/* R27 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2626262626262626ULL;	/* R26 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2929292929292929ULL;	/* R29 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2828282828282828ULL;	/* R28 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00;	/* XZR - has no effect, used so there are an even number of registers. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00;	/* R30 - procedure call link register. */
	pxTopOfStack--;

	*pxTopOfStack = portINITIAL_PSTATE;
	pxTopOfStack--;

	*pxTopOfStack = ( StackType_t ) pxCode; /* Exception return address. */
	pxTopOfStack--;

	/* The task will start with a critical nesting count of 0 as interrupts are
	enabled. */
	*pxTopOfStack = portNO_CRITICAL_NESTING;
	pxTopOfStack--;

	/* The task will start without a floating point context.  A task that uses
	the floating point hardware must call vPortTaskUsesFPU() before executing
	any floating point instructions. */
	*pxTopOfStack = portNO_FLOATING_POINT_CONTEXT;

	return pxTopOfStack;
}

BaseType_t xPortStartScheduler( void )
{
    uint32_t ulAPSR;

	__asm volatile ( "MRS %0, CurrentEL" : "=r" ( ulAPSR ) );
	ulAPSR &= portAPSR_MODE_BITS_MASK;

	configASSERT( ulAPSR == portEL1 );
	if( ulAPSR == portEL1 )
	{
		/* Interrupts are turned off in the CPU itself to ensure a tick does
		not execute	while the scheduler is being started.  Interrupts are
		automatically turned back on in the CPU when the first task starts
		executing. */
		portDISABLE_INTERRUPTS();

        if (0 == portGET_CORE_ID())
        {
            ullPortSchedularRunning = pdTRUE;
        }

		/* Start the first task executing. */
		vPortRestoreTaskContext();
	}

	return 0;
}

void vPortEndScheduler( void )
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT( 0 );
}

void vPortTimerTickHandler()
{
    if( ullPortSchedularRunning == pdTRUE )
    {
        /* Increment the RTOS tick. */
        if( xTaskIncrementTick() != pdFALSE )
        {
            ullPortYieldRequired[portGET_CORE_ID()] = pdTRUE;
        }
    }
}

void vPortTaskUsesFPU( void )
{
	/* A task is registering the fact that it needs an FPU context.  Set the
	FPU flag (which is saved as part of the task context). */
	ullPortTaskHasFPUContext[portGET_CORE_ID()] = pdTRUE;

	/* Consider initialising the FPSR here - but probably not necessary in
	AArch64. */
}

/* configCHECK_FOR_STACK_OVERFLOW is set to 1, so the application must provide an
 * implementation of vApplicationStackOverflowHook()
 */
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char * pcTaskName )
{
    DebugP_logError("[FreeRTOS] Stack overflow detected for task [%s]", pcTaskName);
    DebugP_assertNoLog(0);
}

#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configKERNEL_PROVIDED_STATIC_MEMORY == 0 )
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
    /* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
     * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
     * used by the Idle task.
     */
    void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                        StackType_t **ppxIdleTaskStackBuffer,
                                        uint32_t *pulIdleTaskStackSize )
    {
        /* Pass out a pointer to the StaticTask_t structure in which the Idle task’s
         * state will be stored.
         */
        *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

        /* Pass out the array that will be used as the Idle task’s stack. */
        *ppxIdleTaskStackBuffer = uxIdleTaskStack;

        /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
         * Note that, as the array is necessarily of type StackType_t,
         * configMINIMAL_STACK_SIZE is specified in words, not bytes.
         */
        *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    }

    #if ( configNUMBER_OF_CORES > 1 )
        static StaticTask_t xPassiveIdleTaskTCBs[ configNUMBER_OF_CORES - 1 ];
        static StackType_t uxPassiveIdleTaskStacks[ configNUMBER_OF_CORES - 1 ][ configMINIMAL_STACK_SIZE ];
        void vApplicationGetPassiveIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                                   StackType_t ** ppxIdleTaskStackBuffer,
                                                   uint32_t * pulIdleTaskStackSize,
                                                   BaseType_t xPassiveIdleTaskIndex )
        {
            *ppxIdleTaskTCBBuffer = &( xPassiveIdleTaskTCBs[ xPassiveIdleTaskIndex ] );
            *ppxIdleTaskStackBuffer = &( uxPassiveIdleTaskStacks[ xPassiveIdleTaskIndex ][ 0 ] );
            *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
        }

    #endif /* #if ( configNUMBER_OF_CORES > 1 ) */

    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];
    /* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
     * application must provide an implementation of vApplicationGetTimerTaskMemory()
     * to provide the memory that is used by the Timer service task.
     */
    void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                         StackType_t **ppxTimerTaskStackBuffer,
                                         uint32_t *pulTimerTaskStackSize )
    {
        /* Pass out a pointer to the StaticTask_t structure in which the Timer
         * task’s state will be stored.
         */
        *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

        /* Pass out the array that will be used as the Timer task’s stack. */
        *ppxTimerTaskStackBuffer = uxTimerTaskStack;

        /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
         * Note that, as the array is necessarily of type StackType_t,
         * configTIMER_TASK_STACK_DEPTH is specified in words, not bytes.
         */
        *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
    }
#endif

/* This function is called when configUSE_IDLE_HOOK is 1 in FreeRTOSConfig.h */
void vApplicationIdleHook( void )
{
    void vApplicationLoadHook();

    vApplicationLoadHook();

    __asm__ volatile ("wfi");
}

/* This function is called when configUSE_MINIMAL_IDLE_HOOK is 1 in FreeRTOSConfig.h */
void vApplicationMinimalIdleHook( void )
{
    void vApplicationLoadHook();

    vApplicationLoadHook();

    __asm__ volatile ("wfi");
}

/* initialize high resolution timer for CPU and task load calculation */
void vPortConfigTimerForRunTimeStats()
{
    /* we assume clock is initialized before the schedular is started */
}

/* return current counter value of high speed counter in units of usecs */
uint32_t uiPortGetRunTimeCounterValue()
{
    uint64_t timeInUsecs = ClockP_getTimeUsec();

    /* note, there is no overflow protection for this 32b value in FreeRTOS
     *
     * This value will overflow in
     * ((0xFFFFFFFF)/(1000000*60)) minutes ~ 71 minutes
     *
     * We call vApplicationLoadHook() in idle loop to accumlate the task load into a 64b value.
     * The implementation of vApplicationLoadHook() is in source\kernel\freertos\dpl\common\TaskP_freertos.c
     */
    return (uint32_t)(timeInUsecs);
}

int32_t Signal_coreIntr( CSL_gic500_gicrRegs *pGic500GicrRegs, uint32_t coreId, uint32_t intrNum )
{
	if ( coreId < configNUM_CORES )
	{
		if ( intrNum < HWIP_GICD_SGI_PPI_INTR_ID_MAX )
		{
            uint32_t mask = 1 << intrNum;
            pGic500GicrRegs->CORE[coreId].SGI_PPI.ISPENDR0 = mask;
		}
	}

    __asm("dsb sy");

	return SystemP_SUCCESS;
}

/* Read 64b value shared between cores */
uint64_t Get_64(volatile uint64_t* x)
{
    __asm("dsb sy");
    return *x;
}

/* Write 64b value shared between cores */
void Set_64(volatile uint64_t* x, uint64_t value)
{
    *x = value;
    __asm("dsb sy");
}

