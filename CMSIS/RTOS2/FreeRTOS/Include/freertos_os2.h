/* --------------------------------------------------------------------------
 * Copyright (c) 2013-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *      Name:    freertos_os2.h
 *      Purpose: CMSIS RTOS2 wrapper for FreeRTOS
 *
 *---------------------------------------------------------------------------*/

#ifndef FREERTOS_OS2_H_
#define FREERTOS_OS2_H_

#include <string.h>
#include <stdint.h>

#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core

#include "RTE_Components.h"             // Component selection
#include CMSIS_device_header

/* Configuration and component setup check */
#if defined(RTE_Compiler_EventRecorder)
  #define USE_TRACE_EVENT_RECORDER
#endif

#if defined(RTE_RTOS_FreeRTOS_HEAP_1)
  #define USE_FreeRTOS_HEAP_1
#endif

#if defined(RTE_RTOS_FreeRTOS_HEAP_5)
  #define USE_FreeRTOS_HEAP_5
#endif

/* Check FreeRTOSConfig.h include definitions.
   Note: CMSIS-RTOS API requires functions included by using following definitions.
         In case if certain API function is not used compiler will optimize it away.
*/
#if (INCLUDE_xSemaphoreGetMutexHolder == 0)
  #error "Definition INCLUDE_xSemaphoreGetMutexHolder must be non-zero to implement Mutex Management API."
#endif
#if (INCLUDE_vTaskDelay == 0)
  #error "Definition INCLUDE_vTaskDelay must be non-zero to implement Generic Wait Functions API."
#endif
#if (INCLUDE_vTaskDelayUntil == 0)
  #error "Definition INCLUDE_vTaskDelayUntil must be non-zero to implement Generic Wait Functions API."
#endif
#if (INCLUDE_vTaskDelete == 0)
  #error "Definition INCLUDE_vTaskDelete must be non-zero to implement Thread Management API."
#endif
#if (INCLUDE_xTaskGetCurrentTaskHandle == 0)
  #error "Definition INCLUDE_xTaskGetCurrentTaskHandle must be non-zero to implement Thread Management API."
#endif
#if (INCLUDE_xTaskGetSchedulerState == 0)
  #error "Definition INCLUDE_xTaskGetSchedulerState must be non-zero to implement Kernel Information and Control API."
#endif
#if (INCLUDE_uxTaskGetStackHighWaterMark == 0)
  #error "Definition INCLUDE_uxTaskGetStackHighWaterMark must be non-zero to implement Thread Management API."
#endif
#if (INCLUDE_uxTaskPriorityGet == 0)
  #error "Definition INCLUDE_uxTaskPriorityGet must be non-zero to implement Thread Management API."
#endif
#if (INCLUDE_vTaskPrioritySet == 0)
  #error "Definition INCLUDE_vTaskPrioritySet must be non-zero to implement Thread Management API."
#endif
#if (INCLUDE_eTaskGetState == 0)
  #error "Definition INCLUDE_eTaskGetState must be non-zero to implement Thread Management API."
#endif
#if (INCLUDE_vTaskSuspend == 0)
  #error "Definition INCLUDE_vTaskSuspend must be non-zero to implement Kernel Information and Control API."
#endif
#if (INCLUDE_xTimerPendFunctionCall == 0)
  #error "Definition INCLUDE_xTimerPendFunctionCall must be non-zero to implement Event Flags API."
#endif

#endif /* FREERTOS_OS2_H_ */
