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

#ifndef PORTMACRO_H
 #define PORTMACRO_H

 #if __cplusplus
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
 #define portCHAR          char
 #define portFLOAT         float
 #define portDOUBLE        double
 #define portLONG          long
 #define portSHORT         short
 #define portSTACK_TYPE    uint32_t
 #define portBASE_TYPE     long

typedef portSTACK_TYPE StackType_t;
typedef long           BaseType_t;
typedef unsigned long  UBaseType_t;

/* Defines the maximum time when using a wait command in a task */
 #if (configUSE_16_BIT_TICKS == 1)
typedef uint16_t TickType_t;
  #define portMAX_DELAY              (TickType_t) 0xffff
 #else
typedef uint32_t TickType_t;
  #define portMAX_DELAY              (TickType_t) 0xffffffffUL

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
 * not need to be guarded with a critical section. */
  #define portTICK_TYPE_IS_ATOMIC    (1)
 #endif

/* Structure passed to pvParameters when task creation code is generated. */
typedef struct st_rm_freertos_port_parameters
{
    void * p_context;                  // Pointer to user data
} rm_freertos_port_parameters_t;

/*-----------------------------------------------------------*/

/* Architecture specifics */

/* Determine the descending of the stack from high address to address */
 #define portSTACK_GROWTH      (-1)

/* Determine the time (in milliseconds) corresponding to each tick */
 #define portTICK_PERIOD_MS    ((TickType_t) 1000 / configTICK_RATE_HZ)

/* It is a multiple of 4 (the two lower-order bits of the address = 0),
 * otherwise it will cause MAE (Misaligned Exception) according to the manual */
 #define portBYTE_ALIGNMENT    (8)

/* Interrupt control macros. */

 #define portENABLE_INTERRUPTS()     __EI() /* Macro to enable all maskable interrupts. */
 #define portDISABLE_INTERRUPTS()    __DI() /* Macro to disable all maskable interrupts. */

/*-----------------------------------------------------------*/

/* Scheduler utilities */

/* Use to transfer control from one task to perform other tasks of higher priority */
extern void vPortYield(void);

/* Called at the end of an ISR that can cause a context switch */

 #define portEND_SWITCHING_ISR(xSwitchRequired)         \
    {                                                   \
        extern volatile BaseType_t xPortSwitchRequired; \
                                                        \
        if (xSwitchRequired != pdFALSE)                 \
        {                                               \
            xPortSwitchRequired = pdTRUE;               \
        }                                               \
    }

 #define portYIELD_FROM_ISR(x)    portEND_SWITCHING_ISR(x)
 #define portYIELD()              vPortYield()

/* Critical section management. */

/* Critical nesting counts are stored in the TCB. */

 #define portCRITICAL_NESTING_IN_TCB    (1)

/* The critical nesting functions defined within tasks.c */

extern void vTaskEnterCritical(void);
extern void vTaskExitCritical(void);

/* Macro to mark the start of a critical code region */
 #define portENTER_CRITICAL()    vTaskEnterCritical()

/* Macro to mark the end of a critical code region */
 #define portEXIT_CRITICAL()     vTaskExitCritical()

/*-----------------------------------------------------------*/

/* As this port allows interrupt nesting... */

 #define portSET_INTERRUPT_MASK_FROM_ISR()                   __stsr_rh(5, 0); __DI() /* Stores the contents of the system register,
                                                                                      * and disable all maskable interrupts */

 #define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)                __ldsr_rh(5, 0, x)      /* Load the contents into the system register */

/*-----------------------------------------------------------*/
/* Task function macros as described on the FreeRTOS.org WEB site. */

 #define portTASK_FUNCTION_PROTO(vFunction, pvParameters)    void vFunction(void * pvParameters)
 #define portTASK_FUNCTION(vFunction, pvParameters)          void vFunction(void * pvParameters)

 #ifdef __cplusplus
}
 #endif

#endif                                 /* PORTMACRO_H */
