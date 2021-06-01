/*
 * FreeRTOS Kernel V10.3.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
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
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#if (defined(__ARMCC_VERSION) || defined(__GNUC__) || defined(__ICCARM__))
#include <stdint.h>
#endif

/* Constants that describe the hardware and memory usage. */
#define configCPU_CLOCK_HZ                    100000000UL
#define configTICK_RATE_HZ                    ((TickType_t)1000)
#define configTOTAL_HEAP_SIZE                 ((size_t)4096)
#define configMINIMAL_STACK_SIZE              ((uint16_t)256)
#define configSUPPORT_DYNAMIC_ALLOCATION      1
#define configSUPPORT_STATIC_ALLOCATION       0

/* Constants related to the behaviour or the scheduler. */
#define configMAX_PRIORITIES                  5
#define configUSE_PREEMPTION                  1
#define configUSE_TIME_SLICING                1
#define configIDLE_SHOULD_YIELD               1
#define configMAX_TASK_NAME_LEN               (10)
#define configUSE_16_BIT_TICKS                0

/* Software timer definitions. */
#define configUSE_TIMERS                      0
#define configTIMER_TASK_PRIORITY             2
#define configTIMER_QUEUE_LENGTH              5
#define configTIMER_TASK_STACK_DEPTH          (configMINIMAL_STACK_SIZE * 2)

/* Constants that build features in or out. */
#define configUSE_MUTEXES                     1
#define configUSE_RECURSIVE_MUTEXES           1
#define configUSE_COUNTING_SEMAPHORES         1
#define configUSE_QUEUE_SETS                  1
#define configUSE_TASK_NOTIFICATIONS          1
#define configUSE_TRACE_FACILITY              1
#define configUSE_TICKLESS_IDLE               1
#define configUSE_APPLICATION_TASK_TAG        0
#define configUSE_NEWLIB_REENTRANT            0
#define configUSE_CO_ROUTINES                 0

/* Constants provided for debugging and optimisation assistance. */
#define configCHECK_FOR_STACK_OVERFLOW        0
#define configQUEUE_REGISTRY_SIZE             0
#define configASSERT( x )                     if( ( x ) == 0 ) { for( ;; ); }

/* Constants that define which hook (callback) functions should be used. */
#define configUSE_IDLE_HOOK                   0
#define configUSE_TICK_HOOK                   0
#define configUSE_DAEMON_TASK_STARTUP_HOOK    0
#define configUSE_MALLOC_FAILED_HOOK          0

/* Cortex A9 port configuration. */

/* Configure interrupt controller specific definitions */
#define configINTERRUPT_CONTROLLER_BASE_ADDRESS         0x2C000000
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET 0x1000
#define configUNIQUE_INTERRUPT_PRIORITIES               32

/*
 * The FreeRTOS Cortex-A port implements a full interrupt nesting model.
 *
 * Interrupts that are assigned a priority at or below
 * configMAX_API_CALL_INTERRUPT_PRIORITY (which counter-intuitively in the ARM
 * generic interrupt controller [GIC] means a priority that has a numerical
 * value above configMAX_API_CALL_INTERRUPT_PRIORITY) can call FreeRTOS safe API
 * functions and will nest.
 *
 * Interrupts that are assigned a priority above
 * configMAX_API_CALL_INTERRUPT_PRIORITY (which in the GIC means a numerical
 * value below configMAX_API_CALL_INTERRUPT_PRIORITY) cannot call any FreeRTOS
 * API functions, will nest, and will not be masked by FreeRTOS critical
 * sections (although it is necessary for interrupts to be globally disabled
 * extremely briefly as the interrupt mask is updated in the GIC).
 *
 * FreeRTOS functions that can be called from an interrupt are those that end in
 * "FromISR".  FreeRTOS maintains a separate interrupt safe API to enable
 * interrupt entry to be shorter, faster, simpler and smaller.
 */
#define configMAX_API_CALL_INTERRUPT_PRIORITY   25

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function.  NOTE:  Setting an INCLUDE_ parameter to 0 is
 * only necessary if the linker does not automatically remove functions that are
 * not referenced anyway. */
#define INCLUDE_vTaskPrioritySet              1
#define INCLUDE_uxTaskPriorityGet             1
#define INCLUDE_vTaskDelete                   1
#define INCLUDE_vTaskSuspend                  1
#define INCLUDE_xTaskDelayUntil               1
#define INCLUDE_vTaskDelay                    1
#define INCLUDE_xTaskGetIdleTaskHandle        1
#define INCLUDE_xTaskAbortDelay               1
#define INCLUDE_xQueueGetMutexHolder          1
#define INCLUDE_xSemaphoreGetMutexHolder      1
#define INCLUDE_xTaskGetHandle                1
#define INCLUDE_uxTaskGetStackHighWaterMark   1
#define INCLUDE_uxTaskGetStackHighWaterMark2  1
#define INCLUDE_eTaskGetState                 1
#define INCLUDE_xTaskResumeFromISR            1
#define INCLUDE_xTimerPendFunctionCall        1
#define INCLUDE_xTaskGetSchedulerState        1
#define INCLUDE_xTaskGetCurrentTaskHandle     1

#if (defined(__ARMCC_VERSION) || defined(__GNUC__) || defined(__ICCARM__))
/*
 * The application must provide a function that configures a peripheral to
 * create the FreeRTOS tick interrupt, then define configSETUP_TICK_INTERRUPT()
 * in FreeRTOSConfig.h to call the function.
 */
void vConfigureTickInterrupt( void );
void vClearTickInterrupt    ( void );

/* Map the FreeRTOS port timer configuration functions to their implementations */
#define configSETUP_TICK_INTERRUPT()          vConfigureTickInterrupt()
#define configCLEAR_TICK_INTERRUPT()          vClearTickInterrupt()

/* Map the FreeRTOS IRQ and SVC/SWI handlers to the names used in the C startup
code (which is where the vector table is defined). */
#define FreeRTOS_IRQ_Handler                  IRQ_Handler
#define FreeRTOS_SWI_Handler                  SWI_Handler
#endif

#endif /* FREERTOS_CONFIG_H */
