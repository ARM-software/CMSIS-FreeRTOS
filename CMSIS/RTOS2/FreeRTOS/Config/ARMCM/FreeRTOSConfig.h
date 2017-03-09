/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#include <stdint.h>

extern uint32_t SystemCoreClock;

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//  <o>Minimal stack size [words] <0-65535>
//  <i> Stack for idle task and default task stack in words.
//  <i> Default: 128
#define configMINIMAL_STACK_SIZE                ((uint16_t)(128))

//  <o>Total heap size [bytes] <0-0xFFFFFFFF>
//  <i> Heap memory size in bytes.
//  <i> Default: 8192
#define configTOTAL_HEAP_SIZE                   ((size_t)8192)

//  <o>Kernel tick frequency [Hz] <0-0xFFFFFFFF>
//  <i> Kernel tick rate in Hz.
//  <i> Default: 1000
#define configTICK_RATE_HZ                      ((TickType_t)1000)

//  <o>Timer task stack depth [words] <0-65535>
//  <i> Stack for timer task in words.
//  <i> Default: 80
#define configTIMER_TASK_STACK_DEPTH            80

//  <o>Timer task priority <0-56>
//  <i> Timer task priority.
//  <i> Default: 40 (High)
#define configTIMER_TASK_PRIORITY               40

//  <o>Timer queue length <0-1024>
//  <i> Timer command queue length.
//  <i> Default: 5
#define configTIMER_QUEUE_LENGTH                5

//  <q>Use time slicing
//  <i> Enable setting to use timeslicing.
//  <i> Default: 1
#define configUSE_TIME_SLICING                  1

//  <q>Idle should yield
//  <i> Control Yield behaviour of the idle task.
//  <i> Default: 1
#define configIDLE_SHOULD_YIELD                 1

//  <o>Check for stack overflow
//    <0=>Disable <1=>Method one <2=>Method two
//  <i> Enable or disable stack overflow checking.
//  <i> Callback function vApplicationStackOverflowHook implementation is required when stack checking is enabled.
//  <i> Default: 0
#define configCHECK_FOR_STACK_OVERFLOW          2

//  <q>Use idle hook
//  <i> Enable callback function call on each idle task iteration.
//  <i> Callback function vApplicationIdleHook implementation is required when idle hook is enabled.
//  <i> Default: 0
#define configUSE_IDLE_HOOK                     0

//  <q>Use tick hook
//  <i> Enable callback function call during each tick interrupt.
//  <i> Callback function vApplicationTickHook implementation is required when tick hook is enabled.
//  <i> Default: 0
#define configUSE_TICK_HOOK                     0

//  <q>Use deamon task startup hook
//  <i> Enable callback function call when timer service starts.
//  <i> Callback function vApplicationDaemonTaskStartupHook implementation is required when deamon task startup hook is enabled.
//  <i> Default: 0
#define configUSE_DAEMON_TASK_STARTUP_HOOK      0

//  <q>Use malloc failed hook
//  <i> Enable callback function call when out of dynamic memory.
//  <i> Callback function vApplicationMallocFailedHook implementation is required when malloc failed hook is enabled.
//  <i> Default: 0
#define configUSE_MALLOC_FAILED_HOOK            0

//------------- <<< end of configuration section >>> ---------------------------

/* Defines needed by FreeRTOS to implement CMSIS RTOS2 API. Do not change! */
#define configCPU_CLOCK_HZ                      (SystemCoreClock)
#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configUSE_PREEMPTION                    1
#define configUSE_TIMERS                        1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_TRACE_FACILITY                1
#define configUSE_16_BIT_TICKS                  0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configMAX_PRIORITIES                    56
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    16
#define configKERNEL_INTERRUPT_PRIORITY         255

/* Defines that include FreeRTOS functions which implement CMSIS RTOS2 API. Do not change! */
#define INCLUDE_xEventGroupSetBitsFromISR       1
#define INCLUDE_xSemaphoreGetMutexHolder        1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xTimerPendFunctionCall          1

/* Map the FreeRTOS port interrupt handlers to their CMSIS standard names. */
#define xPortPendSVHandler                    PendSV_Handler
#define vPortSVCHandler                       SVC_Handler

/* Include kernel tick timer definitions */
#include "FreeRTOS_Tick_Config.h"

/* Include debug event definitions */
#include "freertos_evr.h"

#endif /* FREERTOS_CONFIG_H */
