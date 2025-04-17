/* --------------------------------------------------------------------------
 * Copyright (c) 2013-2025 Arm Limited. All rights reserved.
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
 *      Name:    freertos_evr.c
 *      Purpose: FreeRTOS Event Recorder implementation
 *
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>

#include "FreeRTOSConfig.h"

/* Temporarily define FreeRTOS object types */
#define TCB_t                   void*
#define Queue_t                 void*
#define Timer_t                 void*
#define PendedFunction_t        void*
#define EventGroup_t            void*
#define StreamBuffer_t          void*

#define TaskHandle_t            void*
#define TimerHandle_t           void*
#define ConstTaskHandle_t       void*
#define EventGroupHandle_t      void*
#define TaskHookFunction_t      void*
#define TimerCallbackFunction_t void*

#define StaticTask_t            void
#define StaticTimer_t           void
#define StaticEventGroup_t      void
#define TaskParameters_t        void
#define TaskStatus_t            void
#define List_t                  void
#define ListItem_t              void
#define TimeOut_t               void
#define MemoryRegion_t          void

#define BaseType_t              long
#define UBaseType_t             uint32_t
#define eTaskState              uint32_t
#define StackType_t             uint32_t
#define TickType_t              uint32_t
#define EventBits_t             uint32_t
#define eSleepModeStatus        uint32_t
#define eNotifyAction           uint32_t

/* FreeRTOS component number */
#define EvtFreeRTOSTasksNo              (0xF0U)
#define EvtFreeRTOSQueueNo              (0xF1U)
#define EvtFreeRTOSTimersNo             (0xF2U)
#define EvtFreeRTOSEventGroupsNo        (0xF3U)
#define EvtFreeRTOSHeapNo               (0xF4U)
#define EvtFreeRTOSStreamBufNo          (0xF5U)

#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)

#include "EventRecorder.h"

#if !defined(EVR_FREERTOS_DISABLE)

/* Ensure default configuration for FreeRTOSConfig files without EVR configuration */
#ifndef configEVR_INITIALIZE
#define configEVR_INITIALIZE          1
#endif
#ifndef configEVR_SETUP_LEVEL
#define configEVR_SETUP_LEVEL         1
#endif
#ifndef configEVR_LEVEL_TASKS
#define configEVR_LEVEL_TASKS         0x05
#endif
#ifndef configEVR_LEVEL_QUEUE
#define configEVR_LEVEL_QUEUE         0x05
#endif
#ifndef configEVR_LEVEL_TIMERS
#define configEVR_LEVEL_TIMERS        0x05
#endif
#ifndef configEVR_LEVEL_EVENTGROUPS
#define configEVR_LEVEL_EVENTGROUPS   0x05
#endif
#ifndef configEVR_LEVEL_HEAP
#define configEVR_LEVEL_HEAP          0x05
#endif
#ifndef configEVR_LEVEL_STREAMBUFFER
#define configEVR_LEVEL_STREAMBUFFER  0x05
#endif

/* Event Recorder level mask for event filter set during initialization */
#ifndef EVR_INIT_RECORDING
#define EVR_INIT_RECORDING            0x01
#endif

/* Start Event Recorder during initialization (1-start, 0-stop) */
#ifndef EVR_INIT_START
#define EVR_INIT_START                1
#endif

/* Verify configuration */
#if ((configEVR_LEVEL_TASKS & (~0x0F)) != 0)
  #error "FreeRTOSConfig.h: invalid bitmask value - configEVR_LEVEL_TASKS"
#endif
#if ((configEVR_LEVEL_QUEUE & (~0x0F)) != 0)
  #error "FreeRTOSConfig.h: invalid bitmask value - configEVR_LEVEL_QUEUE."
#endif
#if ((configEVR_LEVEL_TIMERS & (~0x0F)) != 0)
  #error "FreeRTOSConfig.h: invalid bitmask value - configEVR_LEVEL_TIMERS."
#endif
#if ((configEVR_LEVEL_EVENTGROUPS & (~0x0F)) != 0)
  #error "FreeRTOSConfig.h: invalid bitmask value - configEVR_LEVEL_EVENTGROUPS."
#endif
#if ((configEVR_LEVEL_HEAP & (~0x0F)) != 0)
  #error "FreeRTOSConfig.h: invalid bitmask value - configEVR_LEVEL_HEAP."
#endif
#if ((configEVR_LEVEL_STREAMBUFFER & (~0x0F)) != 0)
  #error "FreeRTOSConfig.h: invalid bitmask value - configEVR_LEVEL_STREAMBUFFER."
#endif

#endif /* !defined(EVR_FREERTOS_DISABLE) */

/* Event IDs for "FreeRTOS Tasks" */
#define EvtFreeRTOSTasks_TaskTrackingReset                             EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xFFU)
#define EvtFreeRTOSTasks_TaskCreate                                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x00U)
#define EvtFreeRTOSTasks_TaskCreateFailed                              EventID(EventLevelError,  EvtFreeRTOSTasksNo, 0x01U)
#define EvtFreeRTOSTasks_TaskDelete                                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x02U)
#define EvtFreeRTOSTasks_TaskDelayUntil                                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x03U)
#define EvtFreeRTOSTasks_TaskDelay                                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x04U)
#define EvtFreeRTOSTasks_TaskPrioritySet                               EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x05U)
#define EvtFreeRTOSTasks_TaskSuspend                                   EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x06U)
#define EvtFreeRTOSTasks_TaskResume                                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x07U)
#define EvtFreeRTOSTasks_TaskResumeFromIsr                             EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x08U)
#define EvtFreeRTOSTasks_TaskIncrementTick                             EventID(EventLevelDetail, EvtFreeRTOSTasksNo, 0x09U)
#define EvtFreeRTOSTasks_IncreaseTickCount                             EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0AU)
#define EvtFreeRTOSTasks_TaskSwitchedOut                               EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0BU)
#define EvtFreeRTOSTasks_TaskSwitchedIn                                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0CU)
#define EvtFreeRTOSTasks_TaskPriorityInherit                           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0DU)
#define EvtFreeRTOSTasks_TaskPriorityDisinherit                        EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0EU)
#define EvtFreeRTOSTasks_MovedTaskToReadyState                         EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0FU)
#define EvtFreeRTOSTasks_PostMovedTaskToReadyState                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x10U)
#define EvtFreeRTOSTasks_LowPowerIdleBegin                             EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x11U)
#define EvtFreeRTOSTasks_LowPowerIdleEnd                               EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x12U)
#define EvtFreeRTOSTasks_TaskNotifyTakeBlock                           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x13U)
#define EvtFreeRTOSTasks_TaskNotifyTake                                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x14U)
#define EvtFreeRTOSTasks_TaskNotifyWaitBlock                           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x15U)
#define EvtFreeRTOSTasks_TaskNotifyWait                                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x16U)
#define EvtFreeRTOSTasks_TaskNotify                                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x17U)
#define EvtFreeRTOSTasks_TaskNotifyFromIsr                             EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x18U)
#define EvtFreeRTOSTasks_TaskNotifyGiveFromIsr                         EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x19U)
#define EvtFreeRTOSTasks_MovedTaskToDelayedList                        EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x1AU)
#define EvtFreeRTOSTasks_MovedTaskToOverflowDelayedList                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x1BU)

#define EvtFreeRTOSTasks_xTaskCreateStatic_0                           EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x20U)
#define EvtFreeRTOSTasks_xTaskCreateStatic_1                           EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x21U)
#define EvtFreeRTOSTasks_xTaskCreateStatic_Return                      EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x22U)
#define EvtFreeRTOSTasks_xTaskCreateStaticAffinitySet_0                EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x23U)
#define EvtFreeRTOSTasks_xTaskCreateStaticAffinitySet_1                EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x24U)
#define EvtFreeRTOSTasks_xTaskCreateStaticAffinitySet_Return           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x25U)
#define EvtFreeRTOSTasks_xTaskCreateRestrictedStatic                   EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x26U)
#define EvtFreeRTOSTasks_xTaskCreateRestrictedStatic_Return            EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x27U)
#define EvtFreeRTOSTasks_xTaskCreateRestrictedStaticAffinitySet        EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x28U)
#define EvtFreeRTOSTasks_xTaskCreateRestrictedStaticAffinitySet_Return EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x29U)
#define EvtFreeRTOSTasks_xTaskCreateRestricted                         EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x2AU)
#define EvtFreeRTOSTasks_xTaskCreateRestricted_Return                  EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x2BU)
#define EvtFreeRTOSTasks_xTaskCreateRestrictedAffinitySet              EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x2CU)
#define EvtFreeRTOSTasks_xTaskCreateRestrictedAffinitySet_Return       EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x2DU)
#define EvtFreeRTOSTasks_xTaskCreate_0                                 EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x2EU)
#define EvtFreeRTOSTasks_xTaskCreate_1                                 EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x2FU)
#define EvtFreeRTOSTasks_xTaskCreate_Return                            EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x30U)
#define EvtFreeRTOSTasks_xTaskCreateAffinitySet_0                      EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x31U)
#define EvtFreeRTOSTasks_xTaskCreateAffinitySet_1                      EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x32U)
#define EvtFreeRTOSTasks_xTaskCreateAffinitySet_Return                 EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x33U)
#define EvtFreeRTOSTasks_vTaskDelete                                   EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x34U)
#define EvtFreeRTOSTasks_vTaskDelete_Return                            EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x35U)
#define EvtFreeRTOSTasks_xTaskDelayUntil                               EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x36U)
#define EvtFreeRTOSTasks_xTaskDelayUntil_Return                        EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x37U)
#define EvtFreeRTOSTasks_vTaskDelay                                    EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x38U)
#define EvtFreeRTOSTasks_vTaskDelay_Return                             EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x39U)
#define EvtFreeRTOSTasks_eTaskGetState                                 EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x3AU)
#define EvtFreeRTOSTasks_eTaskGetState_Return                          EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x3BU)
#define EvtFreeRTOSTasks_uxTaskPriorityGet                             EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x3CU)
#define EvtFreeRTOSTasks_uxTaskPriorityGet_Return                      EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x3DU)
#define EvtFreeRTOSTasks_uxTaskPriorityGetFromISR                      EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x3EU)
#define EvtFreeRTOSTasks_uxTaskPriorityGetFromISR_Return               EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x3FU)
#define EvtFreeRTOSTasks_uxTaskBasePriorityGet                         EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x40U)
#define EvtFreeRTOSTasks_uxTaskBasePriorityGet_Return                  EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x41U)
#define EvtFreeRTOSTasks_uxTaskBasePriorityGetFromISR                  EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x42U)
#define EvtFreeRTOSTasks_uxTaskBasePriorityGetFromISR_Return           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x43U)
#define EvtFreeRTOSTasks_vTaskPrioritySet                              EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x44U)
#define EvtFreeRTOSTasks_vTaskPrioritySet_Return                       EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x45U)
#define EvtFreeRTOSTasks_vTaskCoreAffinitySet                          EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x46U)
#define EvtFreeRTOSTasks_vTaskCoreAffinitySet_Return                   EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x47U)
#define EvtFreeRTOSTasks_vTaskCoreAffinityGet                          EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x48U)
#define EvtFreeRTOSTasks_vTaskCoreAffinityGet_Return                   EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x49U)
#define EvtFreeRTOSTasks_vTaskPreemptionDisable                        EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x4AU)
#define EvtFreeRTOSTasks_vTaskPreemptionDisable_Return                 EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x4BU)
#define EvtFreeRTOSTasks_vTaskPreemptionEnable                         EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x4CU)
#define EvtFreeRTOSTasks_vTaskPreemptionEnable_Return                  EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x4DU)
#define EvtFreeRTOSTasks_vTaskSuspend                                  EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x4EU)
#define EvtFreeRTOSTasks_vTaskSuspend_Return                           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x4FU)
#define EvtFreeRTOSTasks_vTaskResume                                   EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x50U)
#define EvtFreeRTOSTasks_vTaskResume_Return                            EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x51U)
#define EvtFreeRTOSTasks_xTaskResumeFromISR                            EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x52U)
#define EvtFreeRTOSTasks_xTaskResumeFromISR_Return                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x53U)
#define EvtFreeRTOSTasks_vTaskStartScheduler                           EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x54U)
#define EvtFreeRTOSTasks_vTaskStartScheduler_Return                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x55U)
#define EvtFreeRTOSTasks_vTaskEndScheduler                             EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x56U)
#define EvtFreeRTOSTasks_vTaskEndScheduler_Return                      EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x57U)
#define EvtFreeRTOSTasks_vTaskSuspendAll                               EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x58U)
#define EvtFreeRTOSTasks_vTaskSuspendAll_Return                        EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x59U)
#define EvtFreeRTOSTasks_xTaskResumeAll                                EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x5AU)
#define EvtFreeRTOSTasks_xTaskResumeAll_Return                         EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x5BU)
#define EvtFreeRTOSTasks_xTaskGetTickCount                             EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x5CU)
#define EvtFreeRTOSTasks_xTaskGetTickCount_Return                      EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x5DU)
#define EvtFreeRTOSTasks_xTaskGetTickCountFromISR                      EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x5EU)
#define EvtFreeRTOSTasks_xTaskGetTickCountFromISR_Return               EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x5FU)
#define EvtFreeRTOSTasks_uxTaskGetNumberOfTasks                        EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x60U)
#define EvtFreeRTOSTasks_uxTaskGetNumberOfTasks_Return                 EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x61U)
#define EvtFreeRTOSTasks_pcTaskGetName                                 EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x62U)
#define EvtFreeRTOSTasks_pcTaskGetName_Return                          EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x63U)
#define EvtFreeRTOSTasks_xTaskGetHandle                                EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x64U)
#define EvtFreeRTOSTasks_xTaskGetHandle_Return                         EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x65U)
#define EvtFreeRTOSTasks_xTaskGetStaticBuffers                         EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x66U)
#define EvtFreeRTOSTasks_xTaskGetStaticBuffers_Return                  EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x67U)
#define EvtFreeRTOSTasks_uxTaskGetSystemState                          EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x68U)
#define EvtFreeRTOSTasks_uxTaskGetSystemState_Return                   EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x69U)
#define EvtFreeRTOSTasks_xTaskGetIdleTaskHandle                        EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x6AU)
#define EvtFreeRTOSTasks_xTaskGetIdleTaskHandle_Return                 EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x6BU)
#define EvtFreeRTOSTasks_xTaskGetIdleTaskHandleForCore                 EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x6CU)
#define EvtFreeRTOSTasks_xTaskGetIdleTaskHandleForCore_Return          EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x6DU)
#define EvtFreeRTOSTasks_vTaskStepTick                                 EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x6EU)
#define EvtFreeRTOSTasks_vTaskStepTick_Return                          EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x6FU)
#define EvtFreeRTOSTasks_xTaskCatchUpTicks                             EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x70U)
#define EvtFreeRTOSTasks_xTaskCatchUpTicks_Return                      EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x71U)
#define EvtFreeRTOSTasks_xTaskAbortDelay                               EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x72U)
#define EvtFreeRTOSTasks_xTaskAbortDelay_Return                        EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x73U)
#define EvtFreeRTOSTasks_xTaskIncrementTick                            EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x74U)
#define EvtFreeRTOSTasks_xTaskIncrementTick_Return                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x75U)
#define EvtFreeRTOSTasks_vTaskSetApplicationTaskTag                    EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x76U)
#define EvtFreeRTOSTasks_vTaskSetApplicationTaskTag_Return             EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x77U)
#define EvtFreeRTOSTasks_xTaskGetApplicationTaskTag                    EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x78U)
#define EvtFreeRTOSTasks_xTaskGetApplicationTaskTag_Return             EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x79U)
#define EvtFreeRTOSTasks_xTaskGetApplicationTaskTagFromISR             EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x7AU)
#define EvtFreeRTOSTasks_xTaskGetApplicationTaskTagFromISR_Return      EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x7BU)
#define EvtFreeRTOSTasks_xTaskCallApplicationTaskHook                  EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x7CU)
#define EvtFreeRTOSTasks_xTaskCallApplicationTaskHook_Return           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x7DU)
#define EvtFreeRTOSTasks_vTaskSwitchContext                            EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x7EU)
#define EvtFreeRTOSTasks_vTaskSwitchContext_Return                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x7FU)
#define EvtFreeRTOSTasks_vTaskPlaceOnEventList                         EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x80U)
#define EvtFreeRTOSTasks_vTaskPlaceOnEventList_Return                  EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x81U)
#define EvtFreeRTOSTasks_vTaskPlaceOnUnorderedEventList                EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x82U)
#define EvtFreeRTOSTasks_vTaskPlaceOnUnorderedEventList_Return         EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x83U)
#define EvtFreeRTOSTasks_vTaskPlaceOnEventListRestricted               EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x84U)
#define EvtFreeRTOSTasks_vTaskPlaceOnEventListRestricted_Return        EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x85U)
#define EvtFreeRTOSTasks_xTaskRemoveFromEventList                      EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x86U)
#define EvtFreeRTOSTasks_xTaskRemoveFromEventList_Return               EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x87U)
#define EvtFreeRTOSTasks_vTaskRemoveFromUnorderedEventList             EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x88U)
#define EvtFreeRTOSTasks_vTaskRemoveFromUnorderedEventList_Return      EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x89U)
#define EvtFreeRTOSTasks_vTaskSetTimeOutState                          EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x8AU)
#define EvtFreeRTOSTasks_vTaskSetTimeOutState_Return                   EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x8BU)
#define EvtFreeRTOSTasks_vTaskInternalSetTimeOutState                  EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x8CU)
#define EvtFreeRTOSTasks_vTaskInternalSetTimeOutState_Return           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x8DU)
#define EvtFreeRTOSTasks_xTaskCheckForTimeOut                          EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x8EU)
#define EvtFreeRTOSTasks_xTaskCheckForTimeOut_Return                   EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x8FU)
#define EvtFreeRTOSTasks_vTaskMissedYield                              EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x90U)
#define EvtFreeRTOSTasks_vTaskMissedYield_Return                       EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x91U)
#define EvtFreeRTOSTasks_uxTaskGetTaskNumber                           EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x92U)
#define EvtFreeRTOSTasks_uxTaskGetTaskNumber_Return                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x93U)
#define EvtFreeRTOSTasks_vTaskSetTaskNumber                            EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x94U)
#define EvtFreeRTOSTasks_vTaskSetTaskNumber_Return                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x95U)
#define EvtFreeRTOSTasks_eTaskConfirmSleepModeStatus                   EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x96U)
#define EvtFreeRTOSTasks_eTaskConfirmSleepModeStatus_Return            EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x97U)
#define EvtFreeRTOSTasks_vTaskSetThreadLocalStoragePointer             EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x98U)
#define EvtFreeRTOSTasks_vTaskSetThreadLocalStoragePointer_Return      EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x99U)
#define EvtFreeRTOSTasks_pvTaskGetThreadLocalStoragePointer            EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x9AU)
#define EvtFreeRTOSTasks_pvTaskGetThreadLocalStoragePointer_Return     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x9BU)
#define EvtFreeRTOSTasks_vTaskAllocateMPURegions                       EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x9CU)
#define EvtFreeRTOSTasks_vTaskAllocateMPURegions_Return                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x9DU)
#define EvtFreeRTOSTasks_vTaskGetInfo                                  EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0x9EU)
#define EvtFreeRTOSTasks_vTaskGetInfo_Return                           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x9FU)
#define EvtFreeRTOSTasks_uxTaskGetStackHighWaterMark2                  EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xA0U)
#define EvtFreeRTOSTasks_uxTaskGetStackHighWaterMark2_Return           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xA1U)
#define EvtFreeRTOSTasks_uxTaskGetStackHighWaterMark                   EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xA2U)
#define EvtFreeRTOSTasks_uxTaskGetStackHighWaterMark_Return            EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xA3U)
#define EvtFreeRTOSTasks_xTaskGetCurrentTaskHandle                     EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xA4U)
#define EvtFreeRTOSTasks_xTaskGetCurrentTaskHandle_Return              EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xA5U)
#define EvtFreeRTOSTasks_xTaskGetCurrentTaskHandleForCore              EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xA6U)
#define EvtFreeRTOSTasks_xTaskGetCurrentTaskHandleForCore_Return       EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xA7U)
#define EvtFreeRTOSTasks_xTaskGetSchedulerState                        EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xA8U)
#define EvtFreeRTOSTasks_xTaskGetSchedulerState_Return                 EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xA9U)
#define EvtFreeRTOSTasks_xTaskPriorityInherit                          EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xAAU)
#define EvtFreeRTOSTasks_xTaskPriorityInherit_Return                   EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xABU)
#define EvtFreeRTOSTasks_xTaskPriorityDisinherit                       EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xACU)
#define EvtFreeRTOSTasks_xTaskPriorityDisinherit_Return                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xADU)
#define EvtFreeRTOSTasks_vTaskPriorityDisinheritAfterTimeout           EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xAEU)
#define EvtFreeRTOSTasks_vTaskPriorityDisinheritAfterTimeout_Return    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xAFU)
#define EvtFreeRTOSTasks_vTaskYieldWithinAPI                           EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xB0U)
#define EvtFreeRTOSTasks_vTaskYieldWithinAPI_Return                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xB1U)
#define EvtFreeRTOSTasks_vTaskEnterCritical                            EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xB2U)
#define EvtFreeRTOSTasks_vTaskEnterCritical_Return                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xB3U)
#define EvtFreeRTOSTasks_vTaskEnterCriticalFromISR                     EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xB4U)
#define EvtFreeRTOSTasks_vTaskEnterCriticalFromISR_Return              EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xB5U)
#define EvtFreeRTOSTasks_vTaskExitCritical                             EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xB6U)
#define EvtFreeRTOSTasks_vTaskExitCritical_Return                      EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xB7U)
#define EvtFreeRTOSTasks_vTaskExitCriticalFromISR                      EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xB8U)
#define EvtFreeRTOSTasks_vTaskExitCriticalFromISR_Return               EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xB9U)
#define EvtFreeRTOSTasks_vTaskListTasks                                EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xBAU)
#define EvtFreeRTOSTasks_vTaskListTasks_Return                         EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xBBU)
#define EvtFreeRTOSTasks_vTaskGetRunTimeStatistics                     EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xBCU)
#define EvtFreeRTOSTasks_vTaskGetRunTimeStatistics_Return              EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xBDU)
#define EvtFreeRTOSTasks_uxTaskResetEventItemValue                     EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xBEU)
#define EvtFreeRTOSTasks_uxTaskResetEventItemValue_Return              EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xBFU)
#define EvtFreeRTOSTasks_pvTaskIncrementMutexHeldCount                 EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xC0U)
#define EvtFreeRTOSTasks_pvTaskIncrementMutexHeldCount_Return          EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xC1U)
#define EvtFreeRTOSTasks_ulTaskGenericNotifyTake                       EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xC2U)
#define EvtFreeRTOSTasks_ulTaskGenericNotifyTake_Return                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xC3U)
#define EvtFreeRTOSTasks_xTaskGenericNotifyWait_0                      EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xC4U)
#define EvtFreeRTOSTasks_xTaskGenericNotifyWait_1                      EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xC5U)
#define EvtFreeRTOSTasks_xTaskGenericNotifyWait_Return                 EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xC6U)
#define EvtFreeRTOSTasks_xTaskGenericNotify_0                          EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xC7U)
#define EvtFreeRTOSTasks_xTaskGenericNotify_1                          EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xC8U)
#define EvtFreeRTOSTasks_xTaskGenericNotify_Return                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xC9U)
#define EvtFreeRTOSTasks_xTaskGenericNotifyFromISR_0                   EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xCAU)
#define EvtFreeRTOSTasks_xTaskGenericNotifyFromISR_1                   EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xCBU)
#define EvtFreeRTOSTasks_xTaskGenericNotifyFromISR_Return              EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xCCU)
#define EvtFreeRTOSTasks_vTaskGenericNotifyGiveFromISR                 EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xCDU)
#define EvtFreeRTOSTasks_vTaskGenericNotifyGiveFromISR_Return          EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xCEU)
#define EvtFreeRTOSTasks_xTaskGenericNotifyStateClear                  EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xCFU)
#define EvtFreeRTOSTasks_xTaskGenericNotifyStateClear_Return           EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xD0U)
#define EvtFreeRTOSTasks_ulTaskGenericNotifyValueClear                 EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xD1U)
#define EvtFreeRTOSTasks_ulTaskGenericNotifyValueClear_Return          EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xD2U)
#define EvtFreeRTOSTasks_ulTaskGetRunTimeCounter                       EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xD3U)
#define EvtFreeRTOSTasks_ulTaskGetRunTimeCounter_Return                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xD4U)
#define EvtFreeRTOSTasks_ulTaskGetRunTimePercent                       EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xD5U)
#define EvtFreeRTOSTasks_ulTaskGetRunTimePercent_Return                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xD6U)
#define EvtFreeRTOSTasks_ulTaskGetIdleRunTimeCounter                   EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xD7U)
#define EvtFreeRTOSTasks_ulTaskGetIdleRunTimeCounter_Return            EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xD8U)
#define EvtFreeRTOSTasks_ulTaskGetIdleRunTimePercent                   EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xD9U)
#define EvtFreeRTOSTasks_ulTaskGetIdleRunTimePercent_Return            EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xDAU)
#define EvtFreeRTOSTasks_xTaskGetMPUSettings                           EventID(EventLevelAPI,    EvtFreeRTOSTasksNo, 0xDBU)
#define EvtFreeRTOSTasks_xTaskGetMPUSettings_Return                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xDCU)

/* Event IDs for "FreeRTOS Queue" */
#define EvtFreeRTOSQueue_QueueCreate                        EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x00U)
#define EvtFreeRTOSQueue_QueueCreateFailed                  EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x01U)
#define EvtFreeRTOSQueue_CreateMutex                        EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x02U)
#define EvtFreeRTOSQueue_CreateMutexFailed                  EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x03U)
#define EvtFreeRTOSQueue_GiveMutexRecursive                 EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x04U)
#define EvtFreeRTOSQueue_GiveMutexRecursiveFailed           EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x05U)
#define EvtFreeRTOSQueue_TakeMutexRecursive                 EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x06U)
#define EvtFreeRTOSQueue_TakeMutexRecursiveFailed           EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x07U)
#define EvtFreeRTOSQueue_CreateCountingSemaphore            EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x08U)
#define EvtFreeRTOSQueue_CreateCountingSemaphoreFailed      EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x09U)
#define EvtFreeRTOSQueue_QueueSend                          EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x0AU)
#define EvtFreeRTOSQueue_QueueSendFailed                    EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x0BU)
#define EvtFreeRTOSQueue_QueueReceive                       EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x0CU)
#define EvtFreeRTOSQueue_QueuePeek                          EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x0DU)
#define EvtFreeRTOSQueue_QueuePeekFromIsr                   EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x0EU)
#define EvtFreeRTOSQueue_QueueReceiveFailed                 EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x0FU)
#define EvtFreeRTOSQueue_QueueSendFromIsr                   EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x10U)
#define EvtFreeRTOSQueue_QueueSendFromIsrFailed             EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x11U)
#define EvtFreeRTOSQueue_QueueReceiveFromIsr                EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x12U)
#define EvtFreeRTOSQueue_QueueReceiveFromIsrFailed          EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x13U)
#define EvtFreeRTOSQueue_QueuePeekFromIsrFailed             EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x14U)
#define EvtFreeRTOSQueue_QueueDelete                        EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x15U)
#define EvtFreeRTOSQueue_QueueRegistryAdd                   EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x16U)
#define EvtFreeRTOSQueue_BlockingOnQueueReceive             EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x17U)
#define EvtFreeRTOSQueue_BlockingOnQueueSend                EventID(EventLevelOp,     EvtFreeRTOSQueueNo, 0x18U)
#define EvtFreeRTOSQueue_QueuePeekFailed                    EventID(EventLevelError,  EvtFreeRTOSQueueNo, 0x19U)

/* Event IDs for "FreeRTOS Timers" */
#define EvtFreeRTOSTimers_TimerCreate                           EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x00U)
#define EvtFreeRTOSTimers_TimerCreateFailed                     EventID(EventLevelError,  EvtFreeRTOSTimersNo, 0x01U)
#define EvtFreeRTOSTimers_TimerCommandSend                      EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x02U)
#define EvtFreeRTOSTimers_TimerCommandReceived                  EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x03U)
#define EvtFreeRTOSTimers_TimerExpired                          EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x04U)
#define EvtFreeRTOSTimers_PendFuncCall                          EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x05U)
#define EvtFreeRTOSTimers_PendFuncCallFromIsr                   EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x06U)

#define EvtFreeRTOSTimers_xTimerCreateTimerTask                 EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x10U)
#define EvtFreeRTOSTimers_xTimerCreateTimerTask_Return          EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x11U)
#define EvtFreeRTOSTimers_xTimerCreate_0                        EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x12U)
#define EvtFreeRTOSTimers_xTimerCreate_1                        EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x13U)
#define EvtFreeRTOSTimers_xTimerCreate_Return                   EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x14U)
#define EvtFreeRTOSTimers_xTimerCreateStatic_0                  EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x15U)
#define EvtFreeRTOSTimers_xTimerCreateStatic_1                  EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x16U)
#define EvtFreeRTOSTimers_xTimerCreateStatic_Return             EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x17U)
#define EvtFreeRTOSTimers_xTimerGenericCommandFromTask_0        EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x18U)
#define EvtFreeRTOSTimers_xTimerGenericCommandFromTask_1        EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x19U)
#define EvtFreeRTOSTimers_xTimerGenericCommandFromTask_Return   EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x1AU)
#define EvtFreeRTOSTimers_xTimerGenericCommandFromISR_0         EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x1BU)
#define EvtFreeRTOSTimers_xTimerGenericCommandFromISR_1         EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x1CU)
#define EvtFreeRTOSTimers_xTimerGenericCommandFromISR_Return    EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x1DU)
#define EvtFreeRTOSTimers_xTimerGetTimerDaemonTaskHandle        EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x1EU)
#define EvtFreeRTOSTimers_xTimerGetTimerDaemonTaskHandle_Return EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x1FU)
#define EvtFreeRTOSTimers_xTimerGetPeriod                       EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x20U)
#define EvtFreeRTOSTimers_xTimerGetPeriod_Return                EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x21U)
#define EvtFreeRTOSTimers_vTimerSetReloadMode                   EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x22U)
#define EvtFreeRTOSTimers_vTimerSetReloadMode_Return            EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x23U)
#define EvtFreeRTOSTimers_xTimerGetReloadMode                   EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x24U)
#define EvtFreeRTOSTimers_xTimerGetReloadMode_Return            EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x25U)
#define EvtFreeRTOSTimers_uxTimerGetReloadMode                  EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x26U)
#define EvtFreeRTOSTimers_uxTimerGetReloadMode_Return           EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x27U)
#define EvtFreeRTOSTimers_xTimerGetExpiryTime                   EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x28U)
#define EvtFreeRTOSTimers_xTimerGetExpiryTime_Return            EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x29U)
#define EvtFreeRTOSTimers_xTimerGetStaticBuffer                 EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x2AU)
#define EvtFreeRTOSTimers_xTimerGetStaticBuffer_Return          EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x2BU)
#define EvtFreeRTOSTimers_pcTimerGetName                        EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x2CU)
#define EvtFreeRTOSTimers_pcTimerGetName_Return                 EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x2DU)
#define EvtFreeRTOSTimers_xTimerIsTimerActive                   EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x2EU)
#define EvtFreeRTOSTimers_xTimerIsTimerActive_Return            EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x2FU)
#define EvtFreeRTOSTimers_pvTimerGetTimerID                     EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x30U)
#define EvtFreeRTOSTimers_pvTimerGetTimerID_Return              EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x31U)
#define EvtFreeRTOSTimers_vTimerSetTimerID                      EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x32U)
#define EvtFreeRTOSTimers_vTimerSetTimerID_Return               EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x33U)
#define EvtFreeRTOSTimers_xTimerPendFunctionCallFromISR         EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x34U)
#define EvtFreeRTOSTimers_xTimerPendFunctionCallFromISR_Return  EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x35U)
#define EvtFreeRTOSTimers_xTimerPendFunctionCall                EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x36U)
#define EvtFreeRTOSTimers_xTimerPendFunctionCall_Return         EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x37U)
#define EvtFreeRTOSTimers_uxTimerGetTimerNumber                 EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x38U)
#define EvtFreeRTOSTimers_uxTimerGetTimerNumber_Return          EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x39U)
#define EvtFreeRTOSTimers_vTimerSetTimerNumber                  EventID(EventLevelAPI,    EvtFreeRTOSTimersNo, 0x3AU)
#define EvtFreeRTOSTimers_vTimerSetTimerNumber_Return           EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x3BU)

/* Event IDs for "FreeRTOS EventGroups" */
#define EvtFreeRTOSEventGroups_EventGroupCreate                     EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x00U)
#define EvtFreeRTOSEventGroups_EventGroupCreateFailed               EventID(EventLevelError,  EvtFreeRTOSEventGroupsNo, 0x01U)
#define EvtFreeRTOSEventGroups_EventGroupSyncBlock                  EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x02U)
#define EvtFreeRTOSEventGroups_EventGroupSyncEnd                    EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x03U)
#define EvtFreeRTOSEventGroups_EventGroupWaitBitsBlock              EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x04U)
#define EvtFreeRTOSEventGroups_EventGroupWaitBitsEnd                EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x05U)
#define EvtFreeRTOSEventGroups_EventGroupClearBits                  EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x06U)
#define EvtFreeRTOSEventGroups_EventGroupClearBitsFromIsr           EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x07U)
#define EvtFreeRTOSEventGroups_EventGroupSetBits                    EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x08U)
#define EvtFreeRTOSEventGroups_EventGroupSetBitsFromIsr             EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x09U)
#define EvtFreeRTOSEventGroups_EventGroupDelete                     EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x0AU)

#define EvtFreeRTOSEventGroups_xEventGroupCreateStatic              EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x10U)
#define EvtFreeRTOSEventGroups_xEventGroupCreateStatic_Return       EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x11U)
#define EvtFreeRTOSEventGroups_xEventGroupCreate                    EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x12U)
#define EvtFreeRTOSEventGroups_xEventGroupCreate_Return             EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x13U)
#define EvtFreeRTOSEventGroups_xEventGroupSync                      EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x14U)
#define EvtFreeRTOSEventGroups_xEventGroupSync_Return               EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x15U)
#define EvtFreeRTOSEventGroups_xEventGroupWaitBits_0                EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x16U)
#define EvtFreeRTOSEventGroups_xEventGroupWaitBits_1                EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x17U)
#define EvtFreeRTOSEventGroups_xEventGroupWaitBits_Return           EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x18U)
#define EvtFreeRTOSEventGroups_xEventGroupClearBits                 EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x19U)
#define EvtFreeRTOSEventGroups_xEventGroupClearBits_Return          EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x1AU)
#define EvtFreeRTOSEventGroups_xEventGroupClearBitsFromISR          EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x1BU)
#define EvtFreeRTOSEventGroups_xEventGroupClearBitsFromISR_Return   EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x1CU)
#define EvtFreeRTOSEventGroups_xEventGroupGetBitsFromISR            EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x1DU)
#define EvtFreeRTOSEventGroups_xEventGroupGetBitsFromISR_Return     EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x1EU)
#define EvtFreeRTOSEventGroups_xEventGroupSetBits                   EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x1FU)
#define EvtFreeRTOSEventGroups_xEventGroupSetBits_Return            EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x20U)
#define EvtFreeRTOSEventGroups_vEventGroupDelete                    EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x21U)
#define EvtFreeRTOSEventGroups_vEventGroupDelete_Return             EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x22U)
#define EvtFreeRTOSEventGroups_xEventGroupGetStaticBuffer           EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x23U)
#define EvtFreeRTOSEventGroups_xEventGroupGetStaticBuffer_Return    EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x24U)
#define EvtFreeRTOSEventGroups_vEventGroupSetBitsCallback           EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x25U)
#define EvtFreeRTOSEventGroups_vEventGroupSetBitsCallback_Return    EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x26U)
#define EvtFreeRTOSEventGroups_vEventGroupClearBitsCallback         EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x27U)
#define EvtFreeRTOSEventGroups_vEventGroupClearBitsCallback_Return  EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x28U)
#define EvtFreeRTOSEventGroups_xEventGroupSetBitsFromISR            EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x29U)
#define EvtFreeRTOSEventGroups_xEventGroupSetBitsFromISR_Return     EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x2AU)
#define EvtFreeRTOSEventGroups_uxEventGroupGetNumber                EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x2BU)
#define EvtFreeRTOSEventGroups_uxEventGroupGetNumber_Return         EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x2CU)
#define EvtFreeRTOSEventGroups_vEventGroupSetNumber                 EventID(EventLevelAPI,    EvtFreeRTOSEventGroupsNo, 0x2DU)
#define EvtFreeRTOSEventGroups_vEventGroupSetNumber_Return          EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x2EU)


/* Event IDs for "FreeRTOS Heap" */
#define EvtFreeRTOSHeap_Malloc                              EventID(EventLevelOp,     EvtFreeRTOSHeapNo, 0x00U)
#define EvtFreeRTOSHeap_Free                                EventID(EventLevelOp,     EvtFreeRTOSHeapNo, 0x01U)

/* Event IDs for "FreeRTOS Stream Buffer" */
#define EvtFreeRTOSStreamBuf_StreamBufferCreateFailed       EventID(EventLevelOp,     EvtFreeRTOSStreamBufNo, 0x00U)
#define EvtFreeRTOSStreamBuf_StreamBufferCreateStaticFailed EventID(EventLevelError,  EvtFreeRTOSStreamBufNo, 0x01U)
#define EvtFreeRTOSStreamBuf_StreamBufferCreate             EventID(EventLevelOp,     EvtFreeRTOSStreamBufNo, 0x02U)
#define EvtFreeRTOSStreamBuf_StreamBufferDelete             EventID(EventLevelOp,     EvtFreeRTOSStreamBufNo, 0x03U)
#define EvtFreeRTOSStreamBuf_StreamBufferReset              EventID(EventLevelOp,     EvtFreeRTOSStreamBufNo, 0x04U)
#define EvtFreeRTOSStreamBuf_StreamBufferBlockingOnSend     EventID(EventLevelOp,     EvtFreeRTOSStreamBufNo, 0x05U)
#define EvtFreeRTOSStreamBuf_StreamBufferSend               EventID(EventLevelOp,     EvtFreeRTOSStreamBufNo, 0x06U)
#define EvtFreeRTOSStreamBuf_StreamBufferSendFailed         EventID(EventLevelError,  EvtFreeRTOSStreamBufNo, 0x07U)
#define EvtFreeRTOSStreamBuf_StreamBufferSendFromIsr        EventID(EventLevelOp,     EvtFreeRTOSStreamBufNo, 0x08U)
#define EvtFreeRTOSStreamBuf_StreamBufferBlockingOnReceive  EventID(EventLevelOp,     EvtFreeRTOSStreamBufNo, 0x09U)
#define EvtFreeRTOSStreamBuf_StreamBufferReceive            EventID(EventLevelOp,     EvtFreeRTOSStreamBufNo, 0x0AU)
#define EvtFreeRTOSStreamBuf_StreamBufferReceiveFailed      EventID(EventLevelError,  EvtFreeRTOSStreamBufNo, 0x0BU)
#define EvtFreeRTOSStreamBuf_StreamBufferReceiveFromIsr     EventID(EventLevelOp,     EvtFreeRTOSStreamBufNo, 0x0CU)

/* Event Recorder initialization and level filter setup */
void EvrFreeRTOSSetup (uint32_t reset) {
#if !defined(EVR_FREERTOS_DISABLE)
  static uint8_t init = 0U;

  if (reset != 0U) {
    init = 0U;
  }

  if (init == 0U) {
    init = 1U;
    #if (configEVR_INITIALIZE != 0)
      EventRecorderInitialize(EVR_INIT_RECORDING, EVR_INIT_START);
    #endif

    #if (configEVR_SETUP_LEVEL != 0)
      EventRecorderEnable(configEVR_LEVEL_TASKS,        EvtFreeRTOSTasksNo,       EvtFreeRTOSTasksNo);
      EventRecorderEnable(configEVR_LEVEL_QUEUE,        EvtFreeRTOSQueueNo,       EvtFreeRTOSQueueNo);
      EventRecorderEnable(configEVR_LEVEL_TIMERS,       EvtFreeRTOSTimersNo,      EvtFreeRTOSTimersNo);
      EventRecorderEnable(configEVR_LEVEL_EVENTGROUPS,  EvtFreeRTOSEventGroupsNo, EvtFreeRTOSEventGroupsNo);
      EventRecorderEnable(configEVR_LEVEL_HEAP,         EvtFreeRTOSHeapNo,        EvtFreeRTOSHeapNo);
      EventRecorderEnable(configEVR_LEVEL_STREAMBUFFER, EvtFreeRTOSStreamBufNo,   EvtFreeRTOSStreamBufNo);
    #endif

    EventRecord2(EvtFreeRTOSTasks_TaskTrackingReset, 0U, 0U);
  }
#else
  (void)reset;
#endif
}

#endif /* defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder) */

/* Tasks */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_CREATE_DISABLE))
void EvrFreeRTOSTasks_TaskCreate (/*TCB_t*/void *pxNewTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskCreate, (uint32_t)pxNewTCB, 0U);
#else
  (void)pxNewTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_CREATE_FAILED_DISABLE))
void EvrFreeRTOSTasks_TaskCreateFailed (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskCreateFailed, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_DELETE_DISABLE))
void EvrFreeRTOSTasks_TaskDelete (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskDelete, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_DELAY_UNTIL_DISABLE))
void EvrFreeRTOSTasks_TaskDelayUntil (uint32_t xTimeToWake) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskDelayUntil, xTimeToWake, 0U);
#else
  (void)xTimeToWake;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_DELAY_DISABLE))
void EvrFreeRTOSTasks_TaskDelay (uint32_t xTicksToDelay) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskDelay, xTicksToDelay, 0U);
#else
  (void)xTicksToDelay;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_PRIORITY_SET_DISABLE))
void EvrFreeRTOSTasks_TaskPrioritySet (/*TCB_t*/void *pxTCB, uint32_t uxNewPriority) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskPrioritySet, (uint32_t)pxTCB, uxNewPriority);
#else
  (void)pxTCB;
  (void)uxNewPriority;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_SUSPEND_DISABLE))
void EvrFreeRTOSTasks_TaskSuspend (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskSuspend, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_RESUME_DISABLE))
void EvrFreeRTOSTasks_TaskResume (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskResume, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_RESUME_FROM_ISR_DISABLE))
void EvrFreeRTOSTasks_TaskResumeFromIsr (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskResumeFromIsr, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_INCREMENT_TICK_DISABLE))
void EvrFreeRTOSTasks_TaskIncrementTick (uint32_t xTickCount) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskIncrementTick, xTickCount, 0U);
#else
  (void)xTickCount;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceINCREASE_TICK_COUNT_DISABLE))
void EvrFreeRTOSTasks_IncreaseTickCount (uint32_t xTicksToJump) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_IncreaseTickCount, xTicksToJump, 0U);
#else
  (void)xTicksToJump;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_SWITCHED_OUT_DISABLE))
void EvrFreeRTOSTasks_TaskSwitchedOut (/*TCB_t*/void *pxCurrentTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskSwitchedOut, (uint32_t)pxCurrentTCB, 0U);
#else
  (void)pxCurrentTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_SWITCHED_IN_DISABLE))
void EvrFreeRTOSTasks_TaskSwitchedIn (/*TCB_t*/void *pxCurrentTCB, uint32_t uxTopPriority) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskSwitchedIn, (uint32_t)pxCurrentTCB, uxTopPriority);
#else
  (void)pxCurrentTCB;
  (void)uxTopPriority;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_PRIORITY_INHERIT_DISABLE))
void EvrFreeRTOSTasks_TaskPriorityInherit (/*TCB_t*/void *pxTCBOfMutexHolder, uint32_t uxInheritedPriority) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskPriorityInherit, (uint32_t)pxTCBOfMutexHolder, (uint32_t)uxInheritedPriority);
#else
  (void)pxTCBOfMutexHolder;
  (void)uxInheritedPriority;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_PRIORITY_DISINHERIT_DISABLE))
void EvrFreeRTOSTasks_TaskPriorityDisinherit (/*TCB_t*/void *pxTCBOfMutexHolder, uint32_t uxOriginalPriority) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskPriorityDisinherit, (uint32_t)pxTCBOfMutexHolder, (uint32_t)uxOriginalPriority);
#else
  (void)pxTCBOfMutexHolder;
  (void)uxOriginalPriority;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceMOVED_TASK_TO_READY_STATE_DISABLE))
void EvrFreeRTOSTasks_MovedTaskToReadyState (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_MovedTaskToReadyState, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(tracePOST_MOVED_TASK_TO_READY_STATE_DISABLE))
void EvrFreeRTOSTasks_PostMovedTaskToReadyState (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_PostMovedTaskToReadyState, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceLOW_POWER_IDLE_BEGIN_DISABLE))
void EvrFreeRTOSTasks_LowPowerIdleBegin (uint32_t xExpectedIdleTime) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_LowPowerIdleBegin, xExpectedIdleTime, 0U);
#else
  (void)xExpectedIdleTime;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceLOW_POWER_IDLE_END_DISABLE))
void EvrFreeRTOSTasks_LowPowerIdleEnd (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_LowPowerIdleEnd, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_TAKE_BLOCK_DISABLE))
void EvrFreeRTOSTasks_TaskNotifyTakeBlock (uint32_t uxIndexToWait, uint32_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskNotifyTakeBlock, uxIndexToWait, xTicksToWait);
#else
  (void)uxIndexToWait;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_TAKE_DISABLE))
void EvrFreeRTOSTasks_TaskNotifyTake (uint32_t uxIndexToWait, uint32_t ulNotifiedValue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskNotifyTake, uxIndexToWait, ulNotifiedValue);
#else
  (void)uxIndexToWait;
  (void)ulNotifiedValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_WAIT_BLOCK_DISABLE))
void EvrFreeRTOSTasks_TaskNotifyWaitBlock  (uint32_t uxIndexToWait, uint32_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskNotifyWaitBlock, uxIndexToWait, xTicksToWait);
#else
  (void)uxIndexToWait;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_WAIT_DISABLE))
void EvrFreeRTOSTasks_TaskNotifyWait (uint32_t uxIndexToWait, uint32_t ulNotifiedValue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskNotifyWait, uxIndexToWait, ulNotifiedValue);
#else
  (void)uxIndexToWait;
  (void)ulNotifiedValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_DISABLE))
void EvrFreeRTOSTasks_TaskNotify (/*TCB_t*/void *xTaskToNotify, uint32_t uxIndexToWait, /*eNotifyAction*/uint32_t eAction, uint32_t ulNotifiedValue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_TaskNotify, (uint32_t)xTaskToNotify, uxIndexToWait, eAction, ulNotifiedValue);
#else
  (void)xTaskToNotify;
  (void)uxIndexToWait;
  (void)eAction;
  (void)ulNotifiedValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_FROM_ISR_DISABLE))
void EvrFreeRTOSTasks_TaskNotifyFromIsr (/*TCB_t*/void *xTaskToNotify, uint32_t uxIndexToWait, /*eNotifyAction*/uint32_t eAction, uint32_t ulNotifiedValue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_TaskNotifyFromIsr, (uint32_t)xTaskToNotify, uxIndexToWait, eAction, ulNotifiedValue);
#else
  (void)xTaskToNotify;
  (void)uxIndexToWait;
  (void)eAction;
  (void)ulNotifiedValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_GIVE_FROM_ISR_DISABLE))
void EvrFreeRTOSTasks_TaskNotifyGiveFromIsr (/*TCB_t*/void *xTaskToNotify, uint32_t uxIndexToWait, uint32_t ulNotifiedValue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_TaskNotifyGiveFromIsr, (uint32_t)xTaskToNotify, uxIndexToWait, ulNotifiedValue, 0U);
#else
  (void)xTaskToNotify;
  (void)uxIndexToWait;
  (void)ulNotifiedValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceMOVED_TASK_TO_DELAYED_LIST_DISABLE))
void EvrFreeRTOSTasks_MovedTaskToDelayedList (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_MovedTaskToDelayedList, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceMOVED_TASK_TO_OVERFLOW_DELAYED_LIST_DISABLE))
void EvrFreeRTOSTasks_MovedTaskToOverflowDelayedList (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_MovedTaskToOverflowDelayedList, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif
/* -------------------------------------------------------------------------------------------------------------- */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateStatic_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateStatic (void *pxTaskCode,
                                         const char *pcName,
                                         const uint32_t ulStackDepth,
                                         void *pvParameters,
                                         uint32_t uxPriority,
                                         void *puxStackBuffer,
                                         void *pxTaskBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_xTaskCreateStatic_0, (uint32_t)pxTaskCode, (uint32_t)pcName, ulStackDepth, (uint32_t)pvParameters);
  EventRecord4(EvtFreeRTOSTasks_xTaskCreateStatic_1, uxPriority, (uint32_t)puxStackBuffer, (uint32_t)pxTaskBuffer, 0U);
#else
  (void)pxTaskCode;
  (void)pcName;
  (void)ulStackDepth;
  (void)pvParameters;
  (void)uxPriority;
  (void)puxStackBuffer;
  (void)pxTaskBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateStatic_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateStatic_Return (/*TCB_t*/void *xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCreateStatic_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateStaticAffinitySet_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateStaticAffinitySet (void *pxTaskCode,
                                                   const char *pcName,
                                                   const uint32_t ulStackDepth,
                                                   void *pvParameters,
                                                   uint32_t uxPriority,
                                                   void *puxStackBuffer,
                                                   void *pxTaskBuffer,
                                                   uint32_t uxCoreAffinityMask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_xTaskCreateStaticAffinitySet_0, (uint32_t)pxTaskCode, (uint32_t)pcName, (uint32_t)ulStackDepth, (uint32_t)pvParameters);
  EventRecord4(EvtFreeRTOSTasks_xTaskCreateStaticAffinitySet_1, uxPriority, (uint32_t)puxStackBuffer, (uint32_t)pxTaskBuffer, uxCoreAffinityMask);
#else
  (void)pxTaskCode;
  (void)pcName;
  (void)ulStackDepth;
  (void)pvParameters;
  (void)uxPriority;
  (void)puxStackBuffer;
  (void)pxTaskBuffer;
  (void)uxCoreAffinityMask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateStaticAffinitySet_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateStaticAffinitySet_Return (/*TCB_t*/void *xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCreateStaticAffinitySet_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateRestrictedStatic_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateRestrictedStatic (const TaskParameters_t *pxTaskDefinition, TaskHandle_t *pxCreatedTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCreateRestrictedStatic, (uint32_t)pxTaskDefinition, (uint32_t)pxCreatedTask);
#else
  (void)pxTaskDefinition;
  (void)pxCreatedTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateRestrictedStatic_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateRestrictedStatic_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCreateRestrictedStatic_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateRestrictedStaticAffinitySet_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateRestrictedStaticAffinitySet (const TaskParameters_t *pxTaskDefinition, UBaseType_t uxCoreAffinityMask, TaskHandle_t *pxCreatedTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_xTaskCreateRestrictedStaticAffinitySet, (uint32_t)pxTaskDefinition, uxCoreAffinityMask, (uint32_t)pxCreatedTask, 0U);
#else
  (void)pxTaskDefinition;
  (void)uxCoreAffinityMask;
  (void)pxCreatedTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateRestrictedStaticAffinitySet_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateRestrictedStaticAffinitySet_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCreateRestrictedStaticAffinitySet_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateRestricted_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateRestricted (const TaskParameters_t *pxTaskDefinition, TaskHandle_t *pxCreatedTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCreateRestricted, (uint32_t)pxTaskDefinition, (uint32_t)pxCreatedTask);
#else
  (void)pxTaskDefinition;
  (void)pxCreatedTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateRestricted_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateRestricted_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCreateRestricted_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateRestrictedAffinitySet_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateRestrictedAffinitySet (TaskParameters_t *pxTaskDefinition, uint32_t uxCoreAffinityMask, /*TCB_t*/void *pxCreatedTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_xTaskCreateRestrictedAffinitySet, (uint32_t)pxTaskDefinition, uxCoreAffinityMask, (uint32_t)pxCreatedTask, 0U);
#else
  (void)pxTaskDefinition;
  (void)uxCoreAffinityMask;
  (void)pxCreatedTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateRestrictedAffinitySet_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateRestrictedAffinitySet_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCreateRestrictedAffinitySet_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreate_DISABLE))
void EvrFreeRTOSTasks_xTaskCreate (void *pxTaskCode, const char *pcName, uint32_t uxStackDepth, void *pvParameters, uint32_t uxPriority, /*TCB_t*/void *pxCreatedTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_xTaskCreate_0, (uint32_t)pxTaskCode, (uint32_t)pcName, uxStackDepth, (uint32_t)pvParameters);
  EventRecord2(EvtFreeRTOSTasks_xTaskCreate_1, uxPriority, (uint32_t)pxCreatedTask);
#else
  (void)pxTaskCode;
  (void)pcName;
  (void)uxStackDepth;
  (void)pvParameters;
  (void)uxPriority;
  (void)pxCreatedTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreate_DISABLE))
void EvrFreeRTOSTasks_xTaskCreate_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCreate_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateAffinitySet_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateAffinitySet (void *pxTaskCode,
                                              const char *pcName,
                                              const uint32_t uxStackDepth,
                                              void *pvParameters,
                                              uint32_t uxPriority,
                                              uint32_t uxCoreAffinityMask,
                                              void *pxCreatedTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_xTaskCreateAffinitySet_0, (uint32_t)pxTaskCode, (uint32_t)pcName, uxStackDepth, (uint32_t)pvParameters);
  EventRecord4(EvtFreeRTOSTasks_xTaskCreateAffinitySet_1, uxPriority, uxCoreAffinityMask, (uint32_t)pxCreatedTask, 0U);
#else
  (void)pxTaskCode;
  (void)pcName;
  (void)uxStackDepth;
  (void)pvParameters;
  (void)uxPriority;
  (void)uxCoreAffinityMask;
  (void)pxCreatedTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateAffinitySet_DISABLE))
void EvrFreeRTOSTasks_xTaskCreateAffinitySet_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCreateAffinitySet_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskDelete_DISABLE))
void EvrFreeRTOSTasks_vTaskDelete (void *xTaskToDelete) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskDelete, (uint32_t)xTaskToDelete, 0U);
#else
  (void)xTaskToDelete;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskDelete_DISABLE))
void EvrFreeRTOSTasks_vTaskDelete_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskDelete_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskDelayUntil_DISABLE))
void EvrFreeRTOSTasks_xTaskDelayUntil (uint32_t *pxPreviousWakeTime, const uint32_t xTimeIncrement) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskDelayUntil, (uint32_t)pxPreviousWakeTime, xTimeIncrement);
#else
  (void)pxPreviousWakeTime;
  (void)xTimeIncrement;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskDelayUntil_DISABLE))
void EvrFreeRTOSTasks_xTaskDelayUntil_Return (uint32_t xShouldDelay) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskDelayUntil_Return, xShouldDelay, 0U);
#else
  (void)xShouldDelay;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskDelay_DISABLE))
void EvrFreeRTOSTasks_vTaskDelay (const uint32_t xTicksToDelay) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskDelay, xTicksToDelay, 0U);
#else
  (void)xTicksToDelay;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskDelay_DISABLE))
void EvrFreeRTOSTasks_vTaskDelay_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskDelay_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_eTaskGetState_DISABLE))
void EvrFreeRTOSTasks_eTaskGetState (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_eTaskGetState, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_eTaskGetState_DISABLE))
void EvrFreeRTOSTasks_eTaskGetState_Return (eTaskState eReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_eTaskGetState_Return, (uint32_t)eReturn, 0U);
#else
  (void)eReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskPriorityGet_DISABLE))
void EvrFreeRTOSTasks_uxTaskPriorityGet (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskPriorityGet, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskPriorityGet_DISABLE))
void EvrFreeRTOSTasks_uxTaskPriorityGet_Return (uint32_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskPriorityGet_Return, uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskPriorityGetFromISR_DISABLE))
void EvrFreeRTOSTasks_uxTaskPriorityGetFromISR (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskPriorityGetFromISR, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskPriorityGetFromISR_DISABLE))
void EvrFreeRTOSTasks_uxTaskPriorityGetFromISR_Return (uint32_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskPriorityGetFromISR_Return, uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskBasePriorityGet_DISABLE))
void EvrFreeRTOSTasks_uxTaskBasePriorityGet (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskBasePriorityGet, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskBasePriorityGet_DISABLE))
void EvrFreeRTOSTasks_uxTaskBasePriorityGet_Return (uint32_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskBasePriorityGet_Return, uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskBasePriorityGetFromISR_DISABLE))
void EvrFreeRTOSTasks_uxTaskBasePriorityGetFromISR (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskBasePriorityGetFromISR, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskBasePriorityGetFromISR_DISABLE))
void EvrFreeRTOSTasks_uxTaskBasePriorityGetFromISR_Return (uint32_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskBasePriorityGetFromISR_Return, uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPrioritySet_DISABLE))
void EvrFreeRTOSTasks_vTaskPrioritySet (TaskHandle_t xTask, uint32_t uxNewPriority) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPrioritySet, (uint32_t)xTask, uxNewPriority);
#else
  (void)xTask;
  (void)uxNewPriority;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPrioritySet_DISABLE))
void EvrFreeRTOSTasks_vTaskPrioritySet_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPrioritySet_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskCoreAffinitySet_DISABLE))
void EvrFreeRTOSTasks_vTaskCoreAffinitySet (TaskHandle_t xTask, uint32_t uxCoreAffinityMask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskCoreAffinitySet, (uint32_t)xTask, uxCoreAffinityMask);
#else
  (void)xTask;
  (void)uxCoreAffinityMask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskCoreAffinitySet_DISABLE))
void EvrFreeRTOSTasks_vTaskCoreAffinitySet_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskCoreAffinitySet_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskCoreAffinityGet_DISABLE))
void EvrFreeRTOSTasks_vTaskCoreAffinityGet (ConstTaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskCoreAffinityGet, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskCoreAffinityGet_DISABLE))
void EvrFreeRTOSTasks_vTaskCoreAffinityGet_Return (uint32_t uxCoreAffinityMask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskCoreAffinityGet_Return, uxCoreAffinityMask, 0U);
#else
  (void)uxCoreAffinityMask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPreemptionDisable_DISABLE))
void EvrFreeRTOSTasks_vTaskPreemptionDisable (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPreemptionDisable, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPreemptionDisable_DISABLE))
void EvrFreeRTOSTasks_vTaskPreemptionDisable_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPreemptionDisable_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPreemptionEnable_DISABLE))
void EvrFreeRTOSTasks_vTaskPreemptionEnable (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPreemptionEnable, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPreemptionEnable_DISABLE))
void EvrFreeRTOSTasks_vTaskPreemptionEnable_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPreemptionEnable_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSuspend_DISABLE))
void EvrFreeRTOSTasks_vTaskSuspend (TaskHandle_t xTaskToSuspend) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSuspend, (uint32_t)xTaskToSuspend, 0U);
#else
  (void)xTaskToSuspend;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSuspend_DISABLE))
void EvrFreeRTOSTasks_vTaskSuspend_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSuspend_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskResume_DISABLE))
void EvrFreeRTOSTasks_vTaskResume (TaskHandle_t xTaskToResume) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskResume, (uint32_t)xTaskToResume, 0U);
#else
  (void)xTaskToResume;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskResume_DISABLE))
void EvrFreeRTOSTasks_vTaskResume_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskResume_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskResumeFromISR_DISABLE))
void EvrFreeRTOSTasks_xTaskResumeFromISR (TaskHandle_t xTaskToResume) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskResumeFromISR, (uint32_t)xTaskToResume, 0U);
#else
  (void)xTaskToResume;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskResumeFromISR_DISABLE))
void EvrFreeRTOSTasks_xTaskResumeFromISR_Return (uint32_t xYieldRequired) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskResumeFromISR_Return, xYieldRequired, 0U);
#else
  (void)xYieldRequired;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskStartScheduler_DISABLE))
void EvrFreeRTOSTasks_vTaskStartScheduler (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskStartScheduler, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskStartScheduler_DISABLE))
void EvrFreeRTOSTasks_vTaskStartScheduler_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskStartScheduler_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskEndScheduler_DISABLE))
void EvrFreeRTOSTasks_vTaskEndScheduler (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskEndScheduler, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskEndScheduler_DISABLE))
void EvrFreeRTOSTasks_vTaskEndScheduler_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskEndScheduler_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSuspendAll_DISABLE))
void EvrFreeRTOSTasks_vTaskSuspendAll (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSuspendAll, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSuspendAll_DISABLE))
void EvrFreeRTOSTasks_vTaskSuspendAll_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSuspendAll_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskResumeAll_DISABLE))
void EvrFreeRTOSTasks_xTaskResumeAll (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskResumeAll, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskResumeAll_DISABLE))
void EvrFreeRTOSTasks_xTaskResumeAll_Return (uint32_t xAlreadyYielded) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskResumeAll_Return, xAlreadyYielded, 0U);
#else
  (void)xAlreadyYielded;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetTickCount_DISABLE))
void EvrFreeRTOSTasks_xTaskGetTickCount (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetTickCount, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetTickCount_DISABLE))
void EvrFreeRTOSTasks_xTaskGetTickCount_Return (uint32_t xTicks) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetTickCount_Return, xTicks, 0U);
#else
  (void)xTicks;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetTickCountFromISR_DISABLE))
void EvrFreeRTOSTasks_xTaskGetTickCountFromISR (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetTickCountFromISR, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetTickCountFromISR_DISABLE))
void EvrFreeRTOSTasks_xTaskGetTickCountFromISR_Return (uint32_t xTicks) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetTickCountFromISR_Return, xTicks, 0U);
#else
  (void)xTicks;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskGetNumberOfTasks_DISABLE))
void EvrFreeRTOSTasks_uxTaskGetNumberOfTasks (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskGetNumberOfTasks, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskGetNumberOfTasks_DISABLE))
void EvrFreeRTOSTasks_uxTaskGetNumberOfTasks_Return (uint32_t uxCurrentNumberOfTasks) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskGetNumberOfTasks_Return, uxCurrentNumberOfTasks, 0U);
#else
  (void)uxCurrentNumberOfTasks;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_pcTaskGetName_DISABLE))
void EvrFreeRTOSTasks_pcTaskGetName (TaskHandle_t xTaskToQuery) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_pcTaskGetName, (uint32_t)xTaskToQuery, 0U);
#else
  (void)xTaskToQuery;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_pcTaskGetName_DISABLE))
void EvrFreeRTOSTasks_pcTaskGetName_Return (char *pcTaskName) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_pcTaskGetName_Return, (uint32_t)pcTaskName, 0U);
#else
  (void)pcTaskName;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetHandle_DISABLE))
void EvrFreeRTOSTasks_xTaskGetHandle (const char *pcNameToQuery) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetHandle, (uint32_t)pcNameToQuery, 0U);
#else
  (void)pcNameToQuery;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetHandle_DISABLE))
void EvrFreeRTOSTasks_xTaskGetHandle_Return (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetHandle_Return, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetStaticBuffers_DISABLE))
void EvrFreeRTOSTasks_xTaskGetStaticBuffers (TaskHandle_t xTask, StackType_t **ppuxStackBuffer, /*StaticTask_t** */uint32_t ppxTaskBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_xTaskGetStaticBuffers, (uint32_t)xTask, (uint32_t)ppuxStackBuffer, (uint32_t)ppxTaskBuffer, 0U);
#else
  (void)xTask;
  (void)ppuxStackBuffer;
  (void)ppxTaskBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetStaticBuffers_DISABLE))
void EvrFreeRTOSTasks_xTaskGetStaticBuffers_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetStaticBuffers_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskGetSystemState_DISABLE))
void EvrFreeRTOSTasks_uxTaskGetSystemState (TaskStatus_t *pxTaskStatusArray, const uint32_t uxArraySize, uint32_t *pulTotalRunTime) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_uxTaskGetSystemState, (uint32_t)pxTaskStatusArray, uxArraySize, (uint32_t)pulTotalRunTime, 0U);
#else
  (void)pxTaskStatusArray;
  (void)uxArraySize;
  (void)pulTotalRunTime;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskGetSystemState_DISABLE))
void EvrFreeRTOSTasks_uxTaskGetSystemState_Return (uint32_t uxTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskGetSystemState_Return, uxTask, 0U);
#else
  (void)uxTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetIdleTaskHandle_DISABLE))
void EvrFreeRTOSTasks_xTaskGetIdleTaskHandle (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetIdleTaskHandle, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetIdleTaskHandle_DISABLE))
void EvrFreeRTOSTasks_xTaskGetIdleTaskHandle_Return (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetIdleTaskHandle_Return, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetIdleTaskHandleForCore_DISABLE))
void EvrFreeRTOSTasks_xTaskGetIdleTaskHandleForCore (uint32_t xCoreID) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetIdleTaskHandleForCore, xCoreID, 0U);
#else
  (void)xCoreID;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetIdleTaskHandleForCore_DISABLE))
void EvrFreeRTOSTasks_xTaskGetIdleTaskHandleForCore_Return (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetIdleTaskHandleForCore_Return, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskStepTick_DISABLE))
void EvrFreeRTOSTasks_vTaskStepTick (uint32_t xTicksToJump) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskStepTick, xTicksToJump, 0U);
#else
  (void)xTicksToJump;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskStepTick_DISABLE))
void EvrFreeRTOSTasks_vTaskStepTick_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskStepTick_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCatchUpTicks_DISABLE))
void EvrFreeRTOSTasks_xTaskCatchUpTicks (uint32_t xTicksToCatchUp) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCatchUpTicks, xTicksToCatchUp, 0U);
#else
  (void)xTicksToCatchUp;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCatchUpTicks_DISABLE))
void EvrFreeRTOSTasks_xTaskCatchUpTicks_Return (uint32_t xYieldOccurred) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCatchUpTicks_Return, xYieldOccurred, 0U);
#else
  (void)xYieldOccurred;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskAbortDelay_DISABLE))
void EvrFreeRTOSTasks_xTaskAbortDelay (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskAbortDelay, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskAbortDelay_DISABLE))
void EvrFreeRTOSTasks_xTaskAbortDelay_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskAbortDelay_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskIncrementTick_DISABLE))
void EvrFreeRTOSTasks_xTaskIncrementTick (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskIncrementTick, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskIncrementTick_DISABLE))
void EvrFreeRTOSTasks_xTaskIncrementTick_Return (uint32_t xSwitchRequired) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskIncrementTick_Return, xSwitchRequired, 0U);
#else
  (void)xSwitchRequired;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSetApplicationTaskTag_DISABLE))
void EvrFreeRTOSTasks_vTaskSetApplicationTaskTag (TaskHandle_t xTask, TaskHookFunction_t pxHookFunction) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSetApplicationTaskTag, (uint32_t)xTask, (uint32_t)pxHookFunction);
#else
  (void)xTask;
  (void)pxHookFunction;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSetApplicationTaskTag_DISABLE))
void EvrFreeRTOSTasks_vTaskSetApplicationTaskTag_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSetApplicationTaskTag_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetApplicationTaskTag_DISABLE))
void EvrFreeRTOSTasks_xTaskGetApplicationTaskTag (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetApplicationTaskTag, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetApplicationTaskTag_DISABLE))
void EvrFreeRTOSTasks_xTaskGetApplicationTaskTag_Return (TaskHookFunction_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetApplicationTaskTag_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetApplicationTaskTagFromISR_DISABLE))
void EvrFreeRTOSTasks_xTaskGetApplicationTaskTagFromISR (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetApplicationTaskTagFromISR, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetApplicationTaskTagFromISR_DISABLE))
void EvrFreeRTOSTasks_xTaskGetApplicationTaskTagFromISR_Return (TaskHookFunction_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetApplicationTaskTagFromISR_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCallApplicationTaskHook_DISABLE))
void EvrFreeRTOSTasks_xTaskCallApplicationTaskHook (TaskHandle_t xTask, void *pvParameter) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCallApplicationTaskHook, (uint32_t)xTask, (uint32_t)pvParameter);
#else
  (void)xTask;
  (void)pvParameter;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCallApplicationTaskHook_DISABLE))
void EvrFreeRTOSTasks_xTaskCallApplicationTaskHook_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCallApplicationTaskHook_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSwitchContext_DISABLE))
void EvrFreeRTOSTasks_vTaskSwitchContext (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSwitchContext, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSwitchContext_DISABLE))
void EvrFreeRTOSTasks_vTaskSwitchContext_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSwitchContext_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPlaceOnEventList_DISABLE))
void EvrFreeRTOSTasks_vTaskPlaceOnEventList (List_t *pxEventList, const TickType_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPlaceOnEventList, (uint32_t)pxEventList, (uint32_t)xTicksToWait);
#else
  (void)pxEventList;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPlaceOnEventList_DISABLE))
void EvrFreeRTOSTasks_vTaskPlaceOnEventList_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPlaceOnEventList_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPlaceOnUnorderedEventList_DISABLE))
void EvrFreeRTOSTasks_vTaskPlaceOnUnorderedEventList (List_t *pxEventList, const TickType_t xItemValue, const TickType_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_vTaskPlaceOnUnorderedEventList, (uint32_t)pxEventList, (uint32_t)xItemValue, (uint32_t)xTicksToWait, 0U);
#else
  (void)pxEventList;
  (void)xItemValue;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPlaceOnUnorderedEventList_DISABLE))
void EvrFreeRTOSTasks_vTaskPlaceOnUnorderedEventList_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPlaceOnUnorderedEventList_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPlaceOnEventListRestricted_DISABLE))
void EvrFreeRTOSTasks_vTaskPlaceOnEventListRestricted (List_t *pxEventList, TickType_t xTicksToWait, const uint32_t xWaitIndefinitely) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_vTaskPlaceOnEventListRestricted, (uint32_t)pxEventList, (uint32_t)xTicksToWait, (uint32_t)xWaitIndefinitely, 0U);
#else
  (void)pxEventList;
  (void)xTicksToWait;
  (void)xWaitIndefinitely;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPlaceOnEventListRestricted_DISABLE))
void EvrFreeRTOSTasks_vTaskPlaceOnEventListRestricted_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPlaceOnEventListRestricted_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskRemoveFromEventList_DISABLE))
void EvrFreeRTOSTasks_xTaskRemoveFromEventList (const List_t *pxEventList) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskRemoveFromEventList, (uint32_t)pxEventList, 0U);
#else
  (void)pxEventList;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskRemoveFromEventList_DISABLE))
void EvrFreeRTOSTasks_xTaskRemoveFromEventList_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskRemoveFromEventList_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskRemoveFromUnorderedEventList_DISABLE))
void EvrFreeRTOSTasks_vTaskRemoveFromUnorderedEventList (ListItem_t *pxEventListItem, const TickType_t xItemValue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskRemoveFromUnorderedEventList, (uint32_t)pxEventListItem, (uint32_t)xItemValue);
#else
  (void)pxEventListItem;
  (void)xItemValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskRemoveFromUnorderedEventList_DISABLE))
void EvrFreeRTOSTasks_vTaskRemoveFromUnorderedEventList_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskRemoveFromUnorderedEventList_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSetTimeOutState_DISABLE))
void EvrFreeRTOSTasks_vTaskSetTimeOutState (TimeOut_t *pxTimeOut) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSetTimeOutState, (uint32_t)pxTimeOut, 0U);
#else
  (void)pxTimeOut;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSetTimeOutState_DISABLE))
void EvrFreeRTOSTasks_vTaskSetTimeOutState_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSetTimeOutState_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskInternalSetTimeOutState_DISABLE))
void EvrFreeRTOSTasks_vTaskInternalSetTimeOutState (TimeOut_t *pxTimeOut) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskInternalSetTimeOutState, (uint32_t)pxTimeOut, 0U);
#else
  (void)pxTimeOut;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskInternalSetTimeOutState_DISABLE))
void EvrFreeRTOSTasks_vTaskInternalSetTimeOutState_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskInternalSetTimeOutState_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCheckForTimeOut_DISABLE))
void EvrFreeRTOSTasks_xTaskCheckForTimeOut (TimeOut_t *pxTimeOut, TickType_t *pxTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCheckForTimeOut, (uint32_t)pxTimeOut, (uint32_t)pxTicksToWait);
#else
  (void)pxTimeOut;
  (void)pxTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCheckForTimeOut_DISABLE))
void EvrFreeRTOSTasks_xTaskCheckForTimeOut_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskCheckForTimeOut_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskMissedYield_DISABLE))
void EvrFreeRTOSTasks_vTaskMissedYield (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskMissedYield, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskMissedYield_DISABLE))
void EvrFreeRTOSTasks_vTaskMissedYield_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskMissedYield_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskGetTaskNumber_DISABLE))
void EvrFreeRTOSTasks_uxTaskGetTaskNumber (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskGetTaskNumber, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskGetTaskNumber_DISABLE))
void EvrFreeRTOSTasks_uxTaskGetTaskNumber_Return (uint32_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskGetTaskNumber_Return, uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSetTaskNumber_DISABLE))
void EvrFreeRTOSTasks_vTaskSetTaskNumber (TaskHandle_t xTask, const uint32_t uxHandle) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSetTaskNumber, (uint32_t)xTask, (uint32_t)uxHandle);
#else
  (void)xTask;
  (void)uxHandle;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSetTaskNumber_DISABLE))
void EvrFreeRTOSTasks_vTaskSetTaskNumber_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSetTaskNumber_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_eTaskConfirmSleepModeStatus_DISABLE))
void EvrFreeRTOSTasks_eTaskConfirmSleepModeStatus (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_eTaskConfirmSleepModeStatus, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_eTaskConfirmSleepModeStatus_DISABLE))
void EvrFreeRTOSTasks_eTaskConfirmSleepModeStatus_Return (eSleepModeStatus eReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_eTaskConfirmSleepModeStatus_Return, (uint32_t)eReturn, 0U);
#else
  (void)eReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSetThreadLocalStoragePointer_DISABLE))
void EvrFreeRTOSTasks_vTaskSetThreadLocalStoragePointer (TaskHandle_t xTaskToSet, uint32_t xIndex, void *pvValue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_vTaskSetThreadLocalStoragePointer, (uint32_t)xTaskToSet, (uint32_t)xIndex, (uint32_t)pvValue, 0U);
#else
  (void)xTaskToSet;
  (void)xIndex;
  (void)pvValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSetThreadLocalStoragePointer_DISABLE))
void EvrFreeRTOSTasks_vTaskSetThreadLocalStoragePointer_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskSetThreadLocalStoragePointer_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_pvTaskGetThreadLocalStoragePointer_DISABLE))
void EvrFreeRTOSTasks_pvTaskGetThreadLocalStoragePointer (TaskHandle_t xTaskToQuery, uint32_t xIndex) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_pvTaskGetThreadLocalStoragePointer, (uint32_t)xTaskToQuery, xIndex);
#else
  (void)xTaskToQuery;
  (void)xIndex;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_pvTaskGetThreadLocalStoragePointer_DISABLE))
void EvrFreeRTOSTasks_pvTaskGetThreadLocalStoragePointer_Return (void *pvReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_pvTaskGetThreadLocalStoragePointer_Return, (uint32_t)pvReturn, 0U);
#else
  (void)pvReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskAllocateMPURegions_DISABLE))
void EvrFreeRTOSTasks_vTaskAllocateMPURegions (TaskHandle_t xTaskToModify, const MemoryRegion_t *pxRegions) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskAllocateMPURegions, (uint32_t)xTaskToModify, (uint32_t)pxRegions);
#else
  (void)xTaskToModify;
  (void)pxRegions;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskAllocateMPURegions_DISABLE))
void EvrFreeRTOSTasks_vTaskAllocateMPURegions_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskAllocateMPURegions_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskGetInfo_DISABLE))
void EvrFreeRTOSTasks_vTaskGetInfo (TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, uint32_t xGetFreeStackSpace, eTaskState eState) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_vTaskGetInfo, (uint32_t)xTask, (uint32_t)pxTaskStatus, (uint32_t)xGetFreeStackSpace, (uint32_t)eState);
#else
  (void)xTask;
  (void)pxTaskStatus;
  (void)xGetFreeStackSpace;
  (void)eState;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskGetInfo_DISABLE))
void EvrFreeRTOSTasks_vTaskGetInfo_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskGetInfo_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskGetStackHighWaterMark2_DISABLE))
void EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark2 (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskGetStackHighWaterMark2, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskGetStackHighWaterMark2_DISABLE))
void EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark2_Return (uint32_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskGetStackHighWaterMark2_Return, uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskGetStackHighWaterMark_DISABLE))
void EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskGetStackHighWaterMark, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskGetStackHighWaterMark_DISABLE))
void EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark_Return (uint32_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskGetStackHighWaterMark_Return, uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetCurrentTaskHandle_DISABLE))
void EvrFreeRTOSTasks_xTaskGetCurrentTaskHandle (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetCurrentTaskHandle, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetCurrentTaskHandle_DISABLE))
void EvrFreeRTOSTasks_xTaskGetCurrentTaskHandle_Return (TaskHandle_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetCurrentTaskHandle_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetCurrentTaskHandleForCore_DISABLE))
void EvrFreeRTOSTasks_xTaskGetCurrentTaskHandleForCore (uint32_t xCoreID) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetCurrentTaskHandleForCore, xCoreID, 0U);
#else
  (void)xCoreID;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetCurrentTaskHandleForCore_DISABLE))
void EvrFreeRTOSTasks_xTaskGetCurrentTaskHandleForCore_Return (TaskHandle_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetCurrentTaskHandleForCore_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetSchedulerState_DISABLE))
void EvrFreeRTOSTasks_xTaskGetSchedulerState (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetSchedulerState, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetSchedulerState_DISABLE))
void EvrFreeRTOSTasks_xTaskGetSchedulerState_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetSchedulerState_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskPriorityInherit_DISABLE))
void EvrFreeRTOSTasks_xTaskPriorityInherit (TaskHandle_t pxMutexHolder) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskPriorityInherit, (uint32_t)pxMutexHolder, 0U);
#else
  (void)pxMutexHolder;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskPriorityInherit_DISABLE))
void EvrFreeRTOSTasks_xTaskPriorityInherit_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskPriorityInherit_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskPriorityDisinherit_DISABLE))
void EvrFreeRTOSTasks_xTaskPriorityDisinherit (TaskHandle_t pxMutexHolder) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskPriorityDisinherit, (uint32_t)pxMutexHolder, 0U);
#else
  (void)pxMutexHolder;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskPriorityDisinherit_DISABLE))
void EvrFreeRTOSTasks_xTaskPriorityDisinherit_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskPriorityDisinherit_Return, xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPriorityDisinheritAfterTimeout_DISABLE))
void EvrFreeRTOSTasks_vTaskPriorityDisinheritAfterTimeout (TaskHandle_t pxMutexHolder, uint32_t uxHighestPriorityWaitingTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPriorityDisinheritAfterTimeout, (uint32_t)pxMutexHolder, uxHighestPriorityWaitingTask);
#else
  (void)pxMutexHolder;
  (void)uxHighestPriorityWaitingTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPriorityDisinheritAfterTimeout_DISABLE))
void EvrFreeRTOSTasks_vTaskPriorityDisinheritAfterTimeout_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskPriorityDisinheritAfterTimeout_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskYieldWithinAPI_DISABLE))
void EvrFreeRTOSTasks_vTaskYieldWithinAPI (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskYieldWithinAPI, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskYieldWithinAPI_DISABLE))
void EvrFreeRTOSTasks_vTaskYieldWithinAPI_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskYieldWithinAPI_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskEnterCritical_DISABLE))
void EvrFreeRTOSTasks_vTaskEnterCritical (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskEnterCritical, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskEnterCritical_DISABLE))
void EvrFreeRTOSTasks_vTaskEnterCritical_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskEnterCritical_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskEnterCriticalFromISR_DISABLE))
void EvrFreeRTOSTasks_vTaskEnterCriticalFromISR (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskEnterCriticalFromISR, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskEnterCriticalFromISR_DISABLE))
void EvrFreeRTOSTasks_vTaskEnterCriticalFromISR_Return (uint32_t uxSavedInterruptStatus) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskEnterCriticalFromISR_Return, uxSavedInterruptStatus, 0U);
#else
  (void)uxSavedInterruptStatus;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskExitCritical_DISABLE))
void EvrFreeRTOSTasks_vTaskExitCritical (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskExitCritical, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskExitCritical_DISABLE))
void EvrFreeRTOSTasks_vTaskExitCritical_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskExitCritical_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskExitCriticalFromISR_DISABLE))
void EvrFreeRTOSTasks_vTaskExitCriticalFromISR (uint32_t uxSavedInterruptStatus) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskExitCriticalFromISR, uxSavedInterruptStatus, 0U);
#else
  (void)uxSavedInterruptStatus;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskExitCriticalFromISR_DISABLE))
void EvrFreeRTOSTasks_vTaskExitCriticalFromISR_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskExitCriticalFromISR_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskListTasks_DISABLE))
void EvrFreeRTOSTasks_vTaskListTasks (char *pcWriteBuffer, size_t uxBufferLength) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskListTasks, (uint32_t)pcWriteBuffer, (uint32_t)uxBufferLength);
#else
  (void)pcWriteBuffer;
  (void)uxBufferLength;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskListTasks_DISABLE))
void EvrFreeRTOSTasks_vTaskListTasks_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskListTasks_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskGetRunTimeStatistics_DISABLE))
void EvrFreeRTOSTasks_vTaskGetRunTimeStatistics (char *pcWriteBuffer, size_t uxBufferLength) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskGetRunTimeStatistics, (uint32_t)pcWriteBuffer, (uint32_t)uxBufferLength);
#else
  (void)pcWriteBuffer;
  (void)uxBufferLength;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskGetRunTimeStatistics_DISABLE))
void EvrFreeRTOSTasks_vTaskGetRunTimeStatistics_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskGetRunTimeStatistics_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskResetEventItemValue_DISABLE))
void EvrFreeRTOSTasks_uxTaskResetEventItemValue (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskResetEventItemValue, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskResetEventItemValue_DISABLE))
void EvrFreeRTOSTasks_uxTaskResetEventItemValue_Return (TickType_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_uxTaskResetEventItemValue_Return, (uint32_t)uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_pvTaskIncrementMutexHeldCount_DISABLE))
void EvrFreeRTOSTasks_pvTaskIncrementMutexHeldCount (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_pvTaskIncrementMutexHeldCount, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_pvTaskIncrementMutexHeldCount_DISABLE))
void EvrFreeRTOSTasks_pvTaskIncrementMutexHeldCount_Return (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_pvTaskIncrementMutexHeldCount_Return, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGenericNotifyTake_DISABLE))
void EvrFreeRTOSTasks_ulTaskGenericNotifyTake (UBaseType_t uxIndexToWaitOn, BaseType_t xClearCountOnExit, TickType_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_ulTaskGenericNotifyTake, uxIndexToWaitOn, (uint32_t)xClearCountOnExit, xTicksToWait, 0U);
#else
  (void)uxIndexToWaitOn;
  (void)xClearCountOnExit;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGenericNotifyTake_DISABLE))
void EvrFreeRTOSTasks_ulTaskGenericNotifyTake_Return (uint32_t ulReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_ulTaskGenericNotifyTake_Return, ulReturn, 0U);
#else
  (void)ulReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGenericNotifyWait_DISABLE))
void EvrFreeRTOSTasks_xTaskGenericNotifyWait (UBaseType_t uxIndexToWaitOn,
                                              uint32_t    ulBitsToClearOnEntry,
                                              uint32_t    ulBitsToClearOnExit,
                                              uint32_t   *pulNotificationValue,
                                              TickType_t  xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_xTaskGenericNotifyWait_0, uxIndexToWaitOn, ulBitsToClearOnEntry, ulBitsToClearOnExit, 0U);
  EventRecord2(EvtFreeRTOSTasks_xTaskGenericNotifyWait_1, (uint32_t)pulNotificationValue, xTicksToWait);
#else
  (void)uxIndexToWaitOn;
  (void)ulBitsToClearOnEntry;
  (void)ulBitsToClearOnExit;
  (void)pulNotificationValue;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGenericNotifyWait_DISABLE))
void EvrFreeRTOSTasks_xTaskGenericNotifyWait_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGenericNotifyWait_Return, (uint32_t) xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGenericNotify_DISABLE))
void EvrFreeRTOSTasks_xTaskGenericNotify (TaskHandle_t  xTaskToNotify,
                                          UBaseType_t   uxIndexToNotify,
                                          uint32_t      ulValue,
                                          eNotifyAction eAction,
                                          uint32_t     *pulPreviousNotificationValue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_xTaskGenericNotify_0, (uint32_t)xTaskToNotify, uxIndexToNotify, ulValue, 0U);
  EventRecord2(EvtFreeRTOSTasks_xTaskGenericNotify_1, (uint32_t)eAction, (uint32_t)pulPreviousNotificationValue);
#else
  (void)xTaskToNotify;
  (void)uxIndexToNotify;
  (void)ulValue;
  (void)eAction;
  (void)pulPreviousNotificationValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGenericNotify_DISABLE))
void EvrFreeRTOSTasks_xTaskGenericNotify_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGenericNotify_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGenericNotifyFromISR_DISABLE))
void EvrFreeRTOSTasks_xTaskGenericNotifyFromISR (TaskHandle_t  xTaskToNotify,
                                                 UBaseType_t   uxIndexToNotify,
                                                 uint32_t      ulValue,
                                                 eNotifyAction eAction,
                                                 uint32_t     *pulPreviousNotificationValue,
                                                 BaseType_t   *pxHigherPriorityTaskWoken) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_xTaskGenericNotifyFromISR_0, (uint32_t)xTaskToNotify, uxIndexToNotify, ulValue, 0U);
  EventRecord4(EvtFreeRTOSTasks_xTaskGenericNotifyFromISR_1, (uint32_t)eAction, (uint32_t)pulPreviousNotificationValue, (uint32_t)pxHigherPriorityTaskWoken, 0U);
#else
  (void)xTaskToNotify;
  (void)uxIndexToNotify;
  (void)ulValue;
  (void)eAction;
  (void)pulPreviousNotificationValue;
  (void)pxHigherPriorityTaskWoken;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGenericNotifyFromISR_DISABLE))
void EvrFreeRTOSTasks_xTaskGenericNotifyFromISR_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGenericNotifyFromISR_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskGenericNotifyGiveFromISR_DISABLE))
void EvrFreeRTOSTasks_vTaskGenericNotifyGiveFromISR (TaskHandle_t xTaskToNotify, UBaseType_t uxIndexToNotify, BaseType_t *pxHigherPriorityTaskWoken) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_vTaskGenericNotifyGiveFromISR, (uint32_t)xTaskToNotify, uxIndexToNotify, (uint32_t)pxHigherPriorityTaskWoken, 0U);
#else
  (void)xTaskToNotify;
  (void)uxIndexToNotify;
  (void)pxHigherPriorityTaskWoken;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskGenericNotifyGiveFromISR_DISABLE))
void EvrFreeRTOSTasks_vTaskGenericNotifyGiveFromISR_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_vTaskGenericNotifyGiveFromISR_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGenericNotifyStateClear_DISABLE))
void EvrFreeRTOSTasks_xTaskGenericNotifyStateClear (TaskHandle_t xTask, UBaseType_t uxIndexToClear) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGenericNotifyStateClear, (uint32_t)xTask, (uint32_t)uxIndexToClear);
#else
  (void)xTask;
  (void)uxIndexToClear;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGenericNotifyStateClear_DISABLE))
void EvrFreeRTOSTasks_xTaskGenericNotifyStateClear_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGenericNotifyStateClear_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGenericNotifyValueClear_DISABLE))
void EvrFreeRTOSTasks_ulTaskGenericNotifyValueClear (TaskHandle_t xTask, UBaseType_t uxIndexToClear, uint32_t ulBitsToClear) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_ulTaskGenericNotifyValueClear, (uint32_t)xTask, uxIndexToClear, ulBitsToClear, 0U);
#else
  (void)xTask;
  (void)uxIndexToClear;
  (void)ulBitsToClear;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGenericNotifyValueClear_DISABLE))
void EvrFreeRTOSTasks_ulTaskGenericNotifyValueClear_Return (uint32_t ulReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_ulTaskGenericNotifyValueClear_Return, ulReturn, 0U);
#else
  (void)ulReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGetRunTimeCounter_DISABLE))
void EvrFreeRTOSTasks_ulTaskGetRunTimeCounter (const TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_ulTaskGetRunTimeCounter, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGetRunTimeCounter_DISABLE))
void EvrFreeRTOSTasks_ulTaskGetRunTimeCounter_Return (uint32_t ulRunTimeCounter) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_ulTaskGetRunTimeCounter_Return, ulRunTimeCounter, 0U);
#else
  (void)ulRunTimeCounter;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGetRunTimePercent_DISABLE))
void EvrFreeRTOSTasks_ulTaskGetRunTimePercent (const TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_ulTaskGetRunTimePercent, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGetRunTimePercent_DISABLE))
void EvrFreeRTOSTasks_ulTaskGetRunTimePercent_Return (uint32_t ulReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_ulTaskGetRunTimePercent_Return, ulReturn, 0U);
#else
  (void)ulReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGetIdleRunTimeCounter_DISABLE))
void EvrFreeRTOSTasks_ulTaskGetIdleRunTimeCounter (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_ulTaskGetIdleRunTimeCounter, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGetIdleRunTimeCounter_DISABLE))
void EvrFreeRTOSTasks_ulTaskGetIdleRunTimeCounter_Return (uint32_t ulReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_ulTaskGetIdleRunTimeCounter_Return, ulReturn, 0U);
#else
  (void)ulReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGetIdleRunTimePercent_DISABLE))
void EvrFreeRTOSTasks_ulTaskGetIdleRunTimePercent (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_ulTaskGetIdleRunTimePercent, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGetIdleRunTimePercent_DISABLE))
void EvrFreeRTOSTasks_ulTaskGetIdleRunTimePercent_Return (uint32_t ulReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_ulTaskGetIdleRunTimePercent_Return, ulReturn, 0U);
#else
  (void)ulReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetMPUSettings_DISABLE))
void EvrFreeRTOSTasks_xTaskGetMPUSettings (TaskHandle_t xTask) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetMPUSettings, (uint32_t)xTask, 0U);
#else
  (void)xTask;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetMPUSettings_DISABLE))
void EvrFreeRTOSTasks_xTaskGetMPUSettings_Return (void *xMPUSettings) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_xTaskGetMPUSettings_Return, (uint32_t)xMPUSettings, 0U);
#else
  (void)xMPUSettings;
#endif
}
#endif


/* Queue */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_CREATE_DISABLE))
void EvrFreeRTOSQueue_QueueCreate (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueCreate, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_CREATE_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueueCreateFailed (uint32_t ucQueueType) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueCreateFailed, ucQueueType, 0U);
#else
  (void)ucQueueType;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_MUTEX_DISABLE))
void EvrFreeRTOSQueue_CreateMutex (/*Queue_t*/void *pxNewQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_CreateMutex, (uint32_t)pxNewQueue, 0U);
#else
  (void)pxNewQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_MUTEX_FAILED_DISABLE))
void EvrFreeRTOSQueue_CreateMutexFailed (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_CreateMutexFailed, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceGIVE_MUTEX_RECURSIVE_DISABLE))
void EvrFreeRTOSQueue_GiveMutexRecursive (/*Queue_t*/void *pxMutex) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_GiveMutexRecursive, (uint32_t)pxMutex, 0U);
#else
  (void)pxMutex;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceGIVE_MUTEX_RECURSIVE_FAILED_DISABLE))
void EvrFreeRTOSQueue_GiveMutexRecursiveFailed (/*Queue_t*/void *pxMutex) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_GiveMutexRecursiveFailed, (uint32_t)pxMutex, 0U);
#else
  (void)pxMutex;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTAKE_MUTEX_RECURSIVE_DISABLE))
void EvrFreeRTOSQueue_TakeMutexRecursive (/*Queue_t*/void *pxMutex) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_TakeMutexRecursive, (uint32_t)pxMutex, 0U);
#else
  (void)pxMutex;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTAKE_MUTEX_RECURSIVE_FAILED_DISABLE))
void EvrFreeRTOSQueue_TakeMutexRecursiveFailed (/*Queue_t*/void *pxMutex) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_TakeMutexRecursiveFailed, (uint32_t)pxMutex, 0U);
#else
  (void)pxMutex;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_COUNTING_SEMAPHORE_DISABLE))
void EvrFreeRTOSQueue_CreateCountingSemaphore (/*Queue_t*/void *xHandle) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_CreateCountingSemaphore, (uint32_t)xHandle, 0U);
#else
  (void)xHandle;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_COUNTING_SEMAPHORE_FAILED_DISABLE))
void EvrFreeRTOSQueue_CreateCountingSemaphoreFailed (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_CreateCountingSemaphoreFailed, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_DISABLE))
void EvrFreeRTOSQueue_QueueSend (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueSend, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueueSendFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueSendFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_DISABLE))
void EvrFreeRTOSQueue_QueueReceive (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueReceive, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_DISABLE))
void EvrFreeRTOSQueue_QueuePeek (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueuePeek, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueuePeekFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueuePeekFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_FROM_ISR_DISABLE))
void EvrFreeRTOSQueue_QueuePeekFromIsr (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueuePeekFromIsr, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueueReceiveFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueReceiveFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_FROM_ISR_DISABLE))
void EvrFreeRTOSQueue_QueueSendFromIsr (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueSendFromIsr, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_FROM_ISR_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueueSendFromIsrFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueSendFromIsrFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_FROM_ISR_DISABLE))
void EvrFreeRTOSQueue_QueueReceiveFromIsr (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueReceiveFromIsr, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_FROM_ISR_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueueReceiveFromIsrFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueReceiveFromIsrFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_FROM_ISR_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueuePeekFromIsrFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueuePeekFromIsrFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_DELETE_DISABLE))
void EvrFreeRTOSQueue_QueueDelete (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueDelete, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_REGISTRY_ADD_DISABLE))
void EvrFreeRTOSQueue_QueueRegistryAdd (/*Queue_t*/void *pxQueue, const char *pcQueueName) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueRegistryAdd, (uint32_t)pxQueue, (uint32_t)pcQueueName);
#else
  (void)pxQueue;
  (void)pcQueueName;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_QUEUE_RECEIVE_DISABLE))
void EvrFreeRTOSQueue_BlockingOnQueueReceive (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_BlockingOnQueueReceive, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_QUEUE_SEND_DISABLE))
void EvrFreeRTOSQueue_BlockingOnQueueSend (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_BlockingOnQueueSend, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif


/* Timers */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_CREATE_DISABLE))
void EvrFreeRTOSTimers_TimerCreate (/*Timer_t*/void *pxNewTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_TimerCreate, (uint32_t)pxNewTimer, 0U);
#else
  (void)pxNewTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_CREATE_FAILED_DISABLE))
void EvrFreeRTOSTimers_TimerCreateFailed (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_TimerCreateFailed, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_COMMAND_SEND_DISABLE))
void EvrFreeRTOSTimers_TimerCommandSend (/*Timer_t*/void *pxTimer, uint32_t xCommandID, uint32_t xOptionalValue, uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_TimerCommandSend, (uint32_t)pxTimer, xCommandID, xOptionalValue, xReturn);
#else
  (void)pxTimer;
  (void)xCommandID;
  (void)xOptionalValue;
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_COMMAND_RECEIVED_DISABLE))
void EvrFreeRTOSTimers_TimerCommandReceived (/*Timer_t*/void *pxTimer, uint32_t xCommandID, uint32_t xOptionalValue) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_TimerCommandReceived, (uint32_t)pxTimer, xCommandID, xOptionalValue, 0U);
#else
  (void)pxTimer;
  (void)xCommandID;
  (void)xOptionalValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_EXPIRED_DISABLE))
void EvrFreeRTOSTimers_TimerExpired (/*Timer_t*/void *pxTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_TimerExpired, (uint32_t)pxTimer, 0U);
#else
  (void)pxTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(tracePEND_FUNC_CALL_DISABLE))
void EvrFreeRTOSTimers_PendFuncCall (/*PendedFunction_t*/void *xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_PendFuncCall, (uint32_t)xFunctionToPend, (uint32_t)pvParameter1, ulParameter2, xReturn);
#else
  (void)xFunctionToPend;
  (void)pvParameter1;
  (void)ulParameter2;
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(tracePEND_FUNC_CALL_FROM_ISR_DISABLE))
void EvrFreeRTOSTimers_PendFuncCallFromIsr (/*PendedFunction_t*/void *xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_PendFuncCallFromIsr, (uint32_t)xFunctionToPend, (uint32_t)pvParameter1, ulParameter2, xReturn);
#else
  (void)xFunctionToPend;
  (void)pvParameter1;
  (void)ulParameter2;
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerCreateTimerTask_DISABLE))
void EvrFreeRTOSTimers_xTimerCreateTimerTask (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerCreateTimerTask, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerCreateTimerTask_DISABLE))
void EvrFreeRTOSTimers_xTimerCreateTimerTask_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerCreateTimerTask_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerCreate_DISABLE))
void EvrFreeRTOSTimers_xTimerCreate (const char *pcTimerName,
                                     const TickType_t xTimerPeriodInTicks,
                                     const BaseType_t xAutoReload,
                                     void *pvTimerID,
                                     TimerCallbackFunction_t pxCallbackFunction) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_xTimerCreate_0, (uint32_t)pcTimerName, (uint32_t)xTimerPeriodInTicks, (uint32_t)xAutoReload, 0U);
  EventRecord2(EvtFreeRTOSTimers_xTimerCreate_1, (uint32_t)pvTimerID, (uint32_t)pxCallbackFunction);
#else
  (void)pcTimerName;
  (void)xTimerPeriodInTicks;
  (void)xAutoReload;
  (void)pvTimerID;
  (void)pxCallbackFunction;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerCreate_DISABLE))
void EvrFreeRTOSTimers_xTimerCreate_Return (TimerHandle_t pxNewTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerCreate_Return, (uint32_t)pxNewTimer, 0U);
#else
  (void)pxNewTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerCreateStatic_DISABLE))
void EvrFreeRTOSTimers_xTimerCreateStatic (const char *pcTimerName,
                                          const TickType_t xTimerPeriodInTicks,
                                          const BaseType_t xAutoReload,
                                          void *pvTimerID,
                                          TimerCallbackFunction_t pxCallbackFunction,
                                          StaticTimer_t *pxTimerBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_xTimerCreateStatic_0, (uint32_t)pcTimerName, (uint32_t)xTimerPeriodInTicks, (uint32_t)xAutoReload, 0U);
  EventRecord4(EvtFreeRTOSTimers_xTimerCreateStatic_1, (uint32_t)pvTimerID, (uint32_t)pxCallbackFunction, (uint32_t)pxTimerBuffer, 0U);
#else
  (void)pcTimerName;
  (void)xTimerPeriodInTicks;
  (void)xAutoReload;
  (void)pvTimerID;
  (void)pxCallbackFunction;
  (void)pxTimerBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerCreateStatic_DISABLE))
void EvrFreeRTOSTimers_xTimerCreateStatic_Return (TimerHandle_t pxNewTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerCreateStatic_Return, (uint32_t)pxNewTimer, 0U);
#else
  (void)pxNewTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGenericCommandFromTask_DISABLE))
void EvrFreeRTOSTimers_xTimerGenericCommandFromTask (TimerHandle_t xTimer,
                                                     const BaseType_t xCommandID,
                                                     const TickType_t xOptionalValue,
                                                     BaseType_t *pxHigherPriorityTaskWoken,
                                                     const TickType_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_xTimerGenericCommandFromTask_0, (uint32_t)xTimer, (uint32_t)xCommandID, (uint32_t)xOptionalValue, 0U);
  EventRecord2(EvtFreeRTOSTimers_xTimerGenericCommandFromTask_1, (uint32_t)pxHigherPriorityTaskWoken, (uint32_t)xTicksToWait);
#else
  (void)xTimer;
  (void)xCommandID;
  (void)xOptionalValue;
  (void)pxHigherPriorityTaskWoken;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGenericCommandFromTask_DISABLE))
void EvrFreeRTOSTimers_xTimerGenericCommandFromTask_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGenericCommandFromTask_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGenericCommandFromISR_DISABLE))
void EvrFreeRTOSTimers_xTimerGenericCommandFromISR (TimerHandle_t xTimer,
                                                    const BaseType_t xCommandID,
                                                    const TickType_t xOptionalValue,
                                                    BaseType_t *pxHigherPriorityTaskWoken,
                                                    const TickType_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_xTimerGenericCommandFromISR_0, (uint32_t)xTimer, (uint32_t)xCommandID, (uint32_t)xOptionalValue, 0U);
  EventRecord2(EvtFreeRTOSTimers_xTimerGenericCommandFromISR_1, (uint32_t)pxHigherPriorityTaskWoken, (uint32_t)xTicksToWait);
#else
  (void)xTimer;
  (void)xCommandID;
  (void)xOptionalValue;
  (void)pxHigherPriorityTaskWoken;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGenericCommandFromISR_DISABLE))
void EvrFreeRTOSTimers_xTimerGenericCommandFromISR_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGenericCommandFromISR_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGetTimerDaemonTaskHandle_DISABLE))
void EvrFreeRTOSTimers_xTimerGetTimerDaemonTaskHandle (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGetTimerDaemonTaskHandle, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGetTimerDaemonTaskHandle_DISABLE))
void EvrFreeRTOSTimers_xTimerGetTimerDaemonTaskHandle_Return (TaskHandle_t xTimerTaskHandle) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGetTimerDaemonTaskHandle_Return, (uint32_t)xTimerTaskHandle, 0U);
#else
  (void)xTimerTaskHandle;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGetPeriod_DISABLE))
void EvrFreeRTOSTimers_xTimerGetPeriod (TimerHandle_t xTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGetPeriod, (uint32_t)xTimer, 0U);
#else
  (void)xTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGetPeriod_DISABLE))
void EvrFreeRTOSTimers_xTimerGetPeriod_Return (uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGetPeriod_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTimerSetReloadMode_DISABLE))
void EvrFreeRTOSTimers_vTimerSetReloadMode (TimerHandle_t xTimer, const BaseType_t xAutoReload) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_vTimerSetReloadMode, (uint32_t)xTimer, (uint32_t)xAutoReload);
#else
  (void)xTimer;
  (void)xAutoReload;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTimerSetReloadMode_DISABLE))
void EvrFreeRTOSTimers_vTimerSetReloadMode_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_vTimerSetReloadMode_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGetReloadMode_DISABLE))
void EvrFreeRTOSTimers_xTimerGetReloadMode (TimerHandle_t xTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGetReloadMode, (uint32_t)xTimer, 0U);
#else
  (void)xTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGetReloadMode_DISABLE))
void EvrFreeRTOSTimers_xTimerGetReloadMode_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGetReloadMode_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTimerGetReloadMode_DISABLE))
void EvrFreeRTOSTimers_uxTimerGetReloadMode (TimerHandle_t xTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_uxTimerGetReloadMode, (uint32_t)xTimer, 0U);
#else
  (void)xTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTimerGetReloadMode_DISABLE))
void EvrFreeRTOSTimers_uxTimerGetReloadMode_Return (UBaseType_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_uxTimerGetReloadMode_Return, (uint32_t)uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGetExpiryTime_DISABLE))
void EvrFreeRTOSTimers_xTimerGetExpiryTime (TimerHandle_t xTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGetExpiryTime, (uint32_t)xTimer, 0U);
#else
  (void)xTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGetExpiryTime_DISABLE))
void EvrFreeRTOSTimers_xTimerGetExpiryTime_Return (TickType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGetExpiryTime_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGetStaticBuffer_DISABLE))
void EvrFreeRTOSTimers_xTimerGetStaticBuffer (TimerHandle_t xTimer, /*StaticTimer_t** */uint32_t ppxTimerBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGetStaticBuffer, (uint32_t)xTimer, (uint32_t)ppxTimerBuffer);
#else
  (void)xTimer;
  (void)ppxTimerBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGetStaticBuffer_DISABLE))
void EvrFreeRTOSTimers_xTimerGetStaticBuffer_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerGetStaticBuffer_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_pcTimerGetName_DISABLE))
void EvrFreeRTOSTimers_pcTimerGetName (TimerHandle_t xTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_pcTimerGetName, (uint32_t)xTimer, 0U);
#else
  (void)xTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_pcTimerGetName_DISABLE))
void EvrFreeRTOSTimers_pcTimerGetName_Return (const char *pcTimerName) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_pcTimerGetName_Return, (uint32_t)pcTimerName, 0U);
#else
  (void)pcTimerName;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerIsTimerActive_DISABLE))
void EvrFreeRTOSTimers_xTimerIsTimerActive (TimerHandle_t xTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerIsTimerActive, (uint32_t)xTimer, 0U);
#else
  (void)xTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerIsTimerActive_DISABLE))
void EvrFreeRTOSTimers_xTimerIsTimerActive_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerIsTimerActive_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_pvTimerGetTimerID_DISABLE))
void EvrFreeRTOSTimers_pvTimerGetTimerID (const TimerHandle_t xTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_pvTimerGetTimerID, (uint32_t)xTimer, 0U);
#else
  (void)xTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_pvTimerGetTimerID_DISABLE))
void EvrFreeRTOSTimers_pvTimerGetTimerID_Return (void *pvReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_pvTimerGetTimerID_Return, (uint32_t)pvReturn, 0U);
#else
  (void)pvReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTimerSetTimerID_DISABLE))
void EvrFreeRTOSTimers_vTimerSetTimerID (TimerHandle_t xTimer, void *pvNewID) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_vTimerSetTimerID, (uint32_t)xTimer, (uint32_t)pvNewID);
#else
  (void)xTimer;
  (void)pvNewID;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTimerSetTimerID_DISABLE))
void EvrFreeRTOSTimers_vTimerSetTimerID_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_vTimerSetTimerID_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerPendFunctionCallFromISR_DISABLE))
void EvrFreeRTOSTimers_xTimerPendFunctionCallFromISR (PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, BaseType_t *pxHigherPriorityTaskWoken) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_xTimerPendFunctionCallFromISR, (uint32_t)xFunctionToPend, (uint32_t)pvParameter1, (uint32_t)ulParameter2, (uint32_t)pxHigherPriorityTaskWoken);
#else
  (void)xFunctionToPend;
  (void)pvParameter1;
  (void)ulParameter2;
  (void)pxHigherPriorityTaskWoken;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerPendFunctionCallFromISR_DISABLE))
void EvrFreeRTOSTimers_xTimerPendFunctionCallFromISR_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerPendFunctionCallFromISR_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerPendFunctionCall_DISABLE))
void EvrFreeRTOSTimers_xTimerPendFunctionCall (PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, TickType_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_xTimerPendFunctionCall, (uint32_t)xFunctionToPend, (uint32_t)pvParameter1, (uint32_t)ulParameter2, (uint32_t)xTicksToWait);
#else
  (void)xFunctionToPend;
  (void)pvParameter1;
  (void)ulParameter2;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerPendFunctionCall_DISABLE))
void EvrFreeRTOSTimers_xTimerPendFunctionCall_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_xTimerPendFunctionCall_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTimerGetTimerNumber_DISABLE))
void EvrFreeRTOSTimers_uxTimerGetTimerNumber (TimerHandle_t xTimer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_uxTimerGetTimerNumber, (uint32_t)xTimer, 0U);
#else
  (void)xTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTimerGetTimerNumber_DISABLE))
void EvrFreeRTOSTimers_uxTimerGetTimerNumber_Return (UBaseType_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_uxTimerGetTimerNumber_Return, (uint32_t)uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTimerSetTimerNumber_DISABLE))
void EvrFreeRTOSTimers_vTimerSetTimerNumber (TimerHandle_t xTimer, UBaseType_t uxTimerNumber) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_vTimerSetTimerNumber, (uint32_t)xTimer, (uint32_t)uxTimerNumber);
#else
  (void)xTimer;
  (void)uxTimerNumber;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTimerSetTimerNumber_DISABLE))
void EvrFreeRTOSTimers_vTimerSetTimerNumber_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_vTimerSetTimerNumber_Return, 0U, 0U);
#endif
}
#endif



/* Event Groups */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CREATE_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupCreate (/*EventGroup_t*/void *pxEventGroup) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupCreate, (uint32_t)pxEventGroup, 0U);
#else
  (void)pxEventGroup;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CREATE_FAILED_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupCreateFailed (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupCreateFailed, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SYNC_BLOCK_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupSyncBlock (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToSet, uint32_t uxBitsToWaitFor) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSEventGroups_EventGroupSyncBlock, (uint32_t)pxEventGroup, uxBitsToSet, uxBitsToWaitFor, 0U);
#else
  (void)pxEventGroup;
  (void)uxBitsToSet;
  (void)uxBitsToWaitFor;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SYNC_END_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupSyncEnd (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToSet, uint32_t uxBitsToWaitFor, uint32_t xTimeoutOccurred) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSEventGroups_EventGroupSyncEnd, (uint32_t)pxEventGroup, uxBitsToSet, uxBitsToWaitFor, xTimeoutOccurred);
#else
  (void)pxEventGroup;
  (void)uxBitsToSet;
  (void)uxBitsToWaitFor;
  (void)xTimeoutOccurred;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_WAIT_BITS_BLOCK_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupWaitBitsBlock (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToWaitFor) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupWaitBitsBlock, (uint32_t)pxEventGroup, uxBitsToWaitFor);
#else
  (void)pxEventGroup;
  (void)uxBitsToWaitFor;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_WAIT_BITS_END_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupWaitBitsEnd (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToWaitFor, uint32_t xTimeoutOccurred) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSEventGroups_EventGroupWaitBitsEnd, (uint32_t)pxEventGroup, uxBitsToWaitFor, xTimeoutOccurred, 0U);
#else
  (void)pxEventGroup;
  (void)uxBitsToWaitFor;
  (void)xTimeoutOccurred;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CLEAR_BITS_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupClearBits (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToClear) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupClearBits, (uint32_t)pxEventGroup, uxBitsToClear);
#else
  (void)pxEventGroup;
  (void)uxBitsToClear;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupClearBitsFromIsr (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToClear) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupClearBitsFromIsr, (uint32_t)pxEventGroup, uxBitsToClear);
#else
  (void)pxEventGroup;
  (void)uxBitsToClear;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SET_BITS_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupSetBits (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToSet) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupSetBits, (uint32_t)pxEventGroup, uxBitsToSet);
#else
  (void)pxEventGroup;
  (void)uxBitsToSet;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SET_BITS_FROM_ISR_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupSetBitsFromIsr (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToSet) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupSetBitsFromIsr, (uint32_t)pxEventGroup, uxBitsToSet);
#else
  (void)pxEventGroup;
  (void)uxBitsToSet;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_DELETE_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupDelete (/*EventGroup_t*/void *pxEventGroup) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupDelete, (uint32_t)pxEventGroup, 0U);
#else
  (void)pxEventGroup;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupCreateStatic_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupCreateStatic (StaticEventGroup_t *pxEventGroupBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupCreateStatic, (uint32_t)pxEventGroupBuffer, 0U);
#else
  (void)pxEventGroupBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupCreateStatic_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupCreateStatic_Return (EventGroupHandle_t xEventGroup) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupCreateStatic_Return, (uint32_t)xEventGroup, 0U);
#else
  (void)xEventGroup;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupCreate_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupCreate (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupCreate, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupCreate_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupCreate_Return (EventGroupHandle_t xEventGroup) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupCreate_Return, (uint32_t)xEventGroup, 0U);
#else
  (void)xEventGroup;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupSync_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupSync (EventGroupHandle_t xEventGroup,
                                             const EventBits_t uxBitsToSet,
                                             const EventBits_t uxBitsToWaitFor,
                                             TickType_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSEventGroups_xEventGroupSync, (uint32_t)xEventGroup, (uint32_t)uxBitsToSet, (uint32_t)uxBitsToWaitFor, (uint32_t)xTicksToWait);
#else
  (void)xEventGroup;
  (void)uxBitsToSet;
  (void)uxBitsToWaitFor;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupSync_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupSync_Return (EventBits_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupSync_Return, (uint32_t)uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupWaitBits_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupWaitBits (EventGroupHandle_t xEventGroup,
                                                 const EventBits_t uxBitsToWaitFor,
                                                 const BaseType_t xClearOnExit,
                                                 const BaseType_t xWaitForAllBits,
                                                 TickType_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSEventGroups_xEventGroupWaitBits_0, (uint32_t)xEventGroup, (uint32_t)uxBitsToWaitFor, (uint32_t)xClearOnExit, 0U);
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupWaitBits_1, (uint32_t)xWaitForAllBits, (uint32_t)xTicksToWait);
#else
  (void)xEventGroup;
  (void)uxBitsToWaitFor;
  (void)xClearOnExit;
  (void)xWaitForAllBits;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupWaitBits_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupWaitBits_Return (EventBits_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupWaitBits_Return, (uint32_t)uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupClearBits_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupClearBits (EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupClearBits, (uint32_t)xEventGroup, (uint32_t)uxBitsToClear);
#else
  (void)xEventGroup;
  (void)uxBitsToClear;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupClearBits_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupClearBits_Return (EventBits_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupClearBits_Return, (uint32_t)uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupClearBitsFromISR_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupClearBitsFromISR (EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupClearBitsFromISR, (uint32_t)xEventGroup, (uint32_t)uxBitsToClear);
#else
  (void)xEventGroup;
  (void)uxBitsToClear;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupClearBitsFromISR_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupClearBitsFromISR_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupClearBitsFromISR_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupGetBitsFromISR_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupGetBitsFromISR (EventGroupHandle_t xEventGroup) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupGetBitsFromISR, (uint32_t)xEventGroup, 0U);
#else
  (void)xEventGroup;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupGetBitsFromISR_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupGetBitsFromISR_Return (EventBits_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupGetBitsFromISR_Return, (uint32_t)uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupSetBits_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupSetBits (EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupSetBits, (uint32_t)xEventGroup, (uint32_t)uxBitsToSet);
#else
  (void)xEventGroup;
  (void)uxBitsToSet;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupSetBits_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupSetBits_Return (EventBits_t uxReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupSetBits_Return, (uint32_t)uxReturn, 0U);
#else
  (void)uxReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vEventGroupDelete_DISABLE))
void EvrFreeRTOSEventGroups_vEventGroupDelete (EventGroupHandle_t xEventGroup) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_vEventGroupDelete, (uint32_t)xEventGroup, 0U);
#else
  (void)xEventGroup;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vEventGroupDelete_DISABLE))
void EvrFreeRTOSEventGroups_vEventGroupDelete_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_vEventGroupDelete_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupGetStaticBuffer_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupGetStaticBuffer (EventGroupHandle_t xEventGroup, /*StaticEventGroup_t** */uint32_t ppxEventGroupBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupGetStaticBuffer, (uint32_t)xEventGroup, (uint32_t)ppxEventGroupBuffer);
#else
  (void)xEventGroup;
  (void)ppxEventGroupBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupGetStaticBuffer_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupGetStaticBuffer_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupGetStaticBuffer_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vEventGroupSetBitsCallback_DISABLE))
void EvrFreeRTOSEventGroups_vEventGroupSetBitsCallback (void *pvEventGroup, uint32_t ulBitsToSet) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_vEventGroupSetBitsCallback, (uint32_t)pvEventGroup, ulBitsToSet);
#else
  (void)pvEventGroup;
  (void)ulBitsToSet;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vEventGroupSetBitsCallback_DISABLE))
void EvrFreeRTOSEventGroups_vEventGroupSetBitsCallback_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_vEventGroupSetBitsCallback_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vEventGroupClearBitsCallback_DISABLE))
void EvrFreeRTOSEventGroups_vEventGroupClearBitsCallback (void *pvEventGroup, uint32_t ulBitsToClear) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_vEventGroupClearBitsCallback, (uint32_t)pvEventGroup, ulBitsToClear);
#else
  (void)pvEventGroup;
  (void)ulBitsToClear;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vEventGroupClearBitsCallback_DISABLE))
void EvrFreeRTOSEventGroups_vEventGroupClearBitsCallback_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_vEventGroupClearBitsCallback_Return, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupSetBitsFromISR_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupSetBitsFromISR (EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t * pxHigherPriorityTaskWoken) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord4(EvtFreeRTOSEventGroups_xEventGroupSetBitsFromISR, (uint32_t)xEventGroup, (uint32_t)uxBitsToSet, (uint32_t)pxHigherPriorityTaskWoken, 0U);
#else
  (void)xEventGroup;
  (void)uxBitsToSet;
  (void)pxHigherPriorityTaskWoken;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupSetBitsFromISR_DISABLE))
void EvrFreeRTOSEventGroups_xEventGroupSetBitsFromISR_Return (BaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_xEventGroupSetBitsFromISR_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxEventGroupGetNumber_DISABLE))
void EvrFreeRTOSEventGroups_uxEventGroupGetNumber (void *xEventGroup) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_uxEventGroupGetNumber, (uint32_t)xEventGroup, 0U);
#else
  (void)xEventGroup;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxEventGroupGetNumber_DISABLE))
void EvrFreeRTOSEventGroups_uxEventGroupGetNumber_Return (UBaseType_t xReturn) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_uxEventGroupGetNumber_Return, (uint32_t)xReturn, 0U);
#else
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vEventGroupSetNumber_DISABLE))
void EvrFreeRTOSEventGroups_vEventGroupSetNumber (void *xEventGroup, UBaseType_t uxEventGroupNumber) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_vEventGroupSetNumber, (uint32_t)xEventGroup, (uint32_t)uxEventGroupNumber);
#else
  (void)xEventGroup;
  (void)uxEventGroupNumber;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vEventGroupSetNumber_DISABLE))
void EvrFreeRTOSEventGroups_vEventGroupSetNumber_Return (void) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_vEventGroupSetNumber_Return, 0U, 0U);
#endif
}
#endif

/* Stream Buffer */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_FAILED_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferCreateFailed (uint32_t uxIsMessageBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferCreateFailed, (uint32_t)uxIsMessageBuffer, 0U);
#else
  (void)uxIsMessageBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_STATIC_FAILED_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferCreateStaticFailed (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t uxIsMessageBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferCreateStaticFailed, (uint32_t)pxStreamBuffer, uxIsMessageBuffer);
#else
  (void)pxStreamBuffer;
  (void)uxIsMessageBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferCreate (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t uxIsMessageBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferCreate, (uint32_t)pxStreamBuffer, uxIsMessageBuffer);
#else
  (void)pxStreamBuffer;
  (void)uxIsMessageBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_DELETE_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferDelete (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferDelete, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RESET_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferReset (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferReset, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_STREAM_BUFFER_SEND_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferBlockingOnSend (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferBlockingOnSend, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_SEND_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferSend (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t xBytesSent) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferSend, (uint32_t)pxStreamBuffer, xBytesSent);
#else
  (void)pxStreamBuffer;
  (void)xBytesSent;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_SEND_FAILED_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferSendFailed (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferSendFailed, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_SEND_FROM_ISR_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferSendFromIsr (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t xBytesSent) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferSendFromIsr, (uint32_t)pxStreamBuffer, xBytesSent);
#else
  (void)pxStreamBuffer;
  (void)xBytesSent;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_STREAM_BUFFER_RECEIVE_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferBlockingOnReceive (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferBlockingOnReceive, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RECEIVE_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferReceive (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t xReceivedLength) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferReceive, (uint32_t)pxStreamBuffer, xReceivedLength);
#else
  (void)pxStreamBuffer;
  (void)xReceivedLength;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RECEIVE_FAILED_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferReceiveFailed (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferReceiveFailed, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RECEIVE_FROM_ISR_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferReceiveFromIsr (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t xReceivedLength) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferReceiveFromIsr, (uint32_t)pxStreamBuffer, xReceivedLength);
#else
  (void)pxStreamBuffer;
  (void)xReceivedLength;
#endif
}
#endif


/* Heap */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceMALLOC_DISABLE))
void EvrFreeRTOSHeap_Malloc (void *pvAddress, uint32_t uiSize) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSHeap_Malloc, (uint32_t)pvAddress, uiSize);
#else
  (void)pvAddress;
  (void)uiSize;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceFREE_DISABLE))
void EvrFreeRTOSHeap_Free (void *pvAddress, uint32_t uiSize) {
#if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
  EventRecord2(EvtFreeRTOSHeap_Free, (uint32_t)pvAddress, uiSize);
#else
  (void)pvAddress;
  (void)uiSize;
#endif
}
#endif
