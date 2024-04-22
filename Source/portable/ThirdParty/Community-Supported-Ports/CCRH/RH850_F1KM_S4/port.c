/*
 * FreeRTOS Kernel V11.0.1
 * Copyright (C) 2023 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_common.h"
/*-----------------------------------------------------------*/

/* Constants required to set up the initial stack. */

/* Task should start with interrupts enabled and in Supervisor mode, therefore
 * PSW is set with ID clear. */
#define portINITIAL_PSW                ((StackType_t) 0x00000000) /* PSW.ID bit */

#define portSTACK_INITIAL_VALUE_R1     ((StackType_t) 0x01010101UL)
#define portSTACK_INITIAL_VALUE_R2     ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x02UL)
#define portSTACK_INITIAL_VALUE_R3     ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x03UL)
#define portSTACK_INITIAL_VALUE_R4     ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x04UL)
#define portSTACK_INITIAL_VALUE_R5     ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x05UL)
#define portSTACK_INITIAL_VALUE_R6     ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x06UL)
#define portSTACK_INITIAL_VALUE_R7     ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x07UL)
#define portSTACK_INITIAL_VALUE_R8     ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x08UL)
#define portSTACK_INITIAL_VALUE_R9     ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x09UL)
#define portSTACK_INITIAL_VALUE_R10    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x10UL)
#define portSTACK_INITIAL_VALUE_R11    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x11UL)
#define portSTACK_INITIAL_VALUE_R12    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x12UL)
#define portSTACK_INITIAL_VALUE_R13    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x13UL)
#define portSTACK_INITIAL_VALUE_R14    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x14UL)
#define portSTACK_INITIAL_VALUE_R15    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x15UL)
#define portSTACK_INITIAL_VALUE_R16    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x16UL)
#define portSTACK_INITIAL_VALUE_R17    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x17UL)
#define portSTACK_INITIAL_VALUE_R18    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x18UL)
#define portSTACK_INITIAL_VALUE_R19    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x19UL)
#define portSTACK_INITIAL_VALUE_R20    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x20UL)
#define portSTACK_INITIAL_VALUE_R21    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x21UL)
#define portSTACK_INITIAL_VALUE_R22    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x22UL)
#define portSTACK_INITIAL_VALUE_R23    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x23UL)
#define portSTACK_INITIAL_VALUE_R24    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x24UL)
#define portSTACK_INITIAL_VALUE_R25    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x25UL)
#define portSTACK_INITIAL_VALUE_R26    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x26UL)
#define portSTACK_INITIAL_VALUE_R27    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x27UL)
#define portSTACK_INITIAL_VALUE_R28    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x28UL)
#define portSTACK_INITIAL_VALUE_R29    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x29UL)
#define portSTACK_INITIAL_VALUE_R30    ((StackType_t) portSTACK_INITIAL_VALUE_R1 * 0x30UL)

/*-----------------------------------------------------------*/

/* Constants required to set up OSTM0 timer. */

/* OSTM Count Start Trigger Register (OSTMnTS) */
#define portOSTM_COUNTER_START              (0x01U)   /* Starts the counter */

/* OSTM Count Stop Trigger Register (OSTMnTT) */
#define portOSTM_COUNTER_STOP               (0x01U)   /* Stops the counter */

/* OSTM Control Register (OSTMnCTL) */
#define portOSTM_MODE_INTERVAL_TIMER        (0x00U)   /* Interval timer mode */
#define portOSTM_MODE_FREE_RUNNING          (0x02U)   /* Free-running comparison mode */
#define portOSTM_START_INTERRUPT_DISABLE    (0x00U)   /* Disables the interrupts when counting starts */
#define portOSTM_START_INTERRUPT_ENABLE     (0x01U)   /* Enables the interrupts when counting starts */

/* Interrupt request flag (RFxxx) */
#define portINT_REQUEST_NOT_OCCUR           (0x0U)    /* No interrupt request is made */

/* Interrupt mask (MKxxx) */
#define portINT_PROCESSING_ENABLED          (0x0U)    /* Enables interrupt processing */
#define portINT_PROCESSING_DISABLED         (0x1U)    /* Disables interrupt processing */

/* Interrupt vector method select (TBxxx) */
#define portINT_DIRECT_VECTOR               (0x0U)    /* Direct jumping to an address by the level of priority */
#define portINT_TABLE_VECTOR                (0x1U)    /* Table reference */

/* Specify 16 interrupt priority levels */
#define portINT_PRIORITY_HIGHEST            (0x00C0U) /* Level 0 (highest) */
#define portINT_PRIORITY_LEVEL1             (0x00C1U) /* Level 1 */
#define portINT_PRIORITY_LEVEL2             (0x00C2U) /* Level 2 */
#define portINT_PRIORITY_LEVEL3             (0x00C3U) /* Level 3 */
#define portINT_PRIORITY_LEVEL4             (0x00C4U) /* Level 4 */
#define portINT_PRIORITY_LEVEL5             (0x00C5U) /* Level 5 */
#define portINT_PRIORITY_LEVEL6             (0x00C6U) /* Level 6 */
#define portINT_PRIORITY_LEVEL7             (0x00C7U) /* Level 7 */
#define portINT_PRIORITY_LEVEL8             (0x00C8U) /* Level 8 */
#define portINT_PRIORITY_LEVEL9             (0x00C9U) /* Level 9 */
#define portINT_PRIORITY_LEVEL10            (0x00CAU) /* Level 10 */
#define portINT_PRIORITY_LEVEL11            (0x00CBU) /* Level 11 */
#define portINT_PRIORITY_LEVEL12            (0x00CCU) /* Level 12 */
#define portINT_PRIORITY_LEVEL13            (0x00CDU) /* Level 13 */
#define portINT_PRIORITY_LEVEL14            (0x00CEU) /* Level 14 */
#define portINT_PRIORITY_LOWEST             (0x00CFU) /* Level 15 (lowest) */

/*-----------------------------------------------------------*/

/* Counts the interrupt nesting depth. A context switch is only performed
 * if the nesting depth is 0. */
volatile BaseType_t xInterruptNesting = 0;

/* Set to 1 to pend a context switch from an ISR. */
volatile BaseType_t xPortSwitchRequired = pdFALSE;

/* Function to start the first task executing */
extern void vPortStartFirstTask(void);

/*
 * Sets up the periodic ISR used for the RTOS tick using the OSTM.
 * The application writer can define configSETUP_TICK_INTERRUPT() (in
 * FreeRTOSConfig.h) such that their own tick interrupt configuration is used
 * in place of prvSetupTimerInterrupt().
 */
static void prvSetupTimerInterrupt(void);

#ifndef configSETUP_TICK_INTERRUPT

/* The user has not provided their own tick interrupt configuration so use
 * the definition in this file (which uses the interval timer). */
 #define configSETUP_TICK_INTERRUPT()    prvSetupTimerInterrupt()
#endif                                 /* configSETUP_TICK_INTERRUPT */

/******************************************************************************
 * Setup the stack of a new task so it is ready to be placed under the
 * scheduler control. The registers have to be placed on the stack in the
 * order that the port expects to find them.
 *
 * @param[in]  pxTopOfStack  Pointer to top of this task's stack
 * @param[in]  pxCode        Task function, stored as initial PC for the task
 * @param[in]  pvParameters  Parameters for task
 ******************************************************************************/
StackType_t * pxPortInitialiseStack (StackType_t * pxTopOfStack, TaskFunction_t pxCode, void * pvParameters)
{
    /* Simulate the stack frame as it would be created by a context switch
     * interrupt. */
    *pxTopOfStack = (StackType_t) vPortEndScheduler;           /* R31 (LP) */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R5;  /* R5 (TP)  */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) pvParameters;                /* R6       */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R7;  /* R7       */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R8;  /* R8       */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R9;  /* R9       */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R10; /* R10      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R11; /* R11      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R12; /* R12      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R13; /* R13      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R14; /* R14      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R15; /* R15      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R16; /* R16      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R17; /* R17      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R18; /* R18      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R19; /* R19      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R20; /* R20      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R21; /* R21      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R22; /* R22      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R23; /* R23      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R24; /* R24      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R25; /* R25      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R26; /* R26      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R27; /* R27      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R28; /* R28      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R29; /* R29      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R30; /* R30      */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R1;  /* R1        */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portSTACK_INITIAL_VALUE_R2;  /* R2        */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) portINITIAL_PSW;             /* EIPSW     */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t) pxCode;                      /* EIPC      */

    return pxTopOfStack;
}

/******************************************************************************
 * Configures the tick frequency and starts the first task.
 *****************************************************************************/
portBASE_TYPE xPortStartScheduler (void)
{
    /* Setup the hardware to generate the tick.  Interrupts are disabled when
     * this function is called. */
    configSETUP_TICK_INTERRUPT();

    /* Restore the context of the first task that is going to run. */
    vPortStartFirstTask();

    /* Execution should not reach here as the tasks are now running! */
    return pdTRUE;
}

/******************************************************************************
 * Execution should never get here.
 *****************************************************************************/
void vPortEndScheduler (void)
{
    /* Not implemented in ports where there is nothing to return to.
     * Artificially force an assert. */
    configASSERT(pdFALSE);
}

/******************************************************************************
 * Port Tick ISR.  This is installed as the OSTM timer interrupt handler.
 *****************************************************************************/
void vPortTickISR (void)
{
    BaseType_t xSavedInterruptStatus;

    xSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        /* Increment the RTOS tick. */
        if (xTaskIncrementTick() != pdFALSE)
        {
            xPortSwitchRequired = pdTRUE;
        }
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR(xSavedInterruptStatus);
}

/******************************************************************************
 * Enables the OS timer to generate the tick interrupts.
 *****************************************************************************/
static void prvSetupTimerInterrupt (void)
{
    /* Disable OSTM0 operation */
    OSTM0.TT = portOSTM_COUNTER_STOP;

    /* Clear interrupt flag */
    INTC2.ICOSTM0.BIT.RFOSTM0 = portINT_REQUEST_NOT_OCCUR;

    /* Enables interrupt processing */
    INTC2.ICOSTM0.BIT.MKOSTM0 = portINT_PROCESSING_ENABLED;

    /*  Table method */
    INTC2.ICOSTM0.BIT.TBOSTM0 = portINT_TABLE_VECTOR;

    /* interrupt priority lowest */
    INTC2.ICOSTM0.UINT16 &= portINT_PRIORITY_LOWEST;

    /* Set OSTM0 control setting */
    OSTM0.CTL = portOSTM_MODE_INTERVAL_TIMER | portOSTM_START_INTERRUPT_DISABLE;
    OSTM0.CMP = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ) - 1;

    /* Enable OSTM0 operation */
    OSTM0.TS = portOSTM_COUNTER_START;
}
