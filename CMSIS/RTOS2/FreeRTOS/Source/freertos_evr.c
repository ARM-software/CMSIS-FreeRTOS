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
 *      Name:    freertos_evr.c
 *      Purpose: FreeRTOS Event Recorder implementation
 *
 *---------------------------------------------------------------------------*/

#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "stream_buffer.h"              // ARM.FreeRTOS::RTOS:Stream Buffer

/* FreeRTOS component number */
#define EvtFreeRTOSTasksNo              (0xF0U)
#define EvtFreeRTOSQueueNo              (0xF1U)
#define EvtFreeRTOSTimersNo             (0xF2U)
#define EvtFreeRTOSEventGroupsNo        (0xF3U)
#define EvtFreeRTOSHeapNo               (0xF4U)
#define EvtFreeRTOSStreamBufNo          (0xF5U)

#ifdef RTE_Compiler_EventRecorder

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
#define EvtFreeRTOSTasks_TaskTrackingReset                  EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0xFFU)
#define EvtFreeRTOSTasks_TaskCreate                         EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x00U)
#define EvtFreeRTOSTasks_TaskCreateFailed                   EventID(EventLevelError,  EvtFreeRTOSTasksNo, 0x01U)
#define EvtFreeRTOSTasks_TaskDelete                         EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x02U)
#define EvtFreeRTOSTasks_TaskDelayUntil                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x03U)
#define EvtFreeRTOSTasks_TaskDelay                          EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x04U)
#define EvtFreeRTOSTasks_TaskPrioritySet                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x05U)
#define EvtFreeRTOSTasks_TaskSuspend                        EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x06U)
#define EvtFreeRTOSTasks_TaskResume                         EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x07U)
#define EvtFreeRTOSTasks_TaskResumeFromIsr                  EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x08U)
#define EvtFreeRTOSTasks_TaskIncrementTick                  EventID(EventLevelDetail, EvtFreeRTOSTasksNo, 0x09U)
#define EvtFreeRTOSTasks_IncreaseTickCount                  EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0AU)
#define EvtFreeRTOSTasks_TaskSwitchedOut                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0BU)
#define EvtFreeRTOSTasks_TaskSwitchedIn                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0CU)
#define EvtFreeRTOSTasks_TaskPriorityInherit                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0DU)
#define EvtFreeRTOSTasks_TaskPriorityDisinherit             EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0EU)
#define EvtFreeRTOSTasks_MovedTaskToReadyState              EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x0FU)
#define EvtFreeRTOSTasks_PostMovedTaskToReadyState          EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x10U)
#define EvtFreeRTOSTasks_LowPowerIdleBegin                  EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x11U)
#define EvtFreeRTOSTasks_LowPowerIdleEnd                    EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x12U)
#define EvtFreeRTOSTasks_TaskNotifyTakeBlock                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x13U)
#define EvtFreeRTOSTasks_TaskNotifyTake                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x14U)
#define EvtFreeRTOSTasks_TaskNotifyWaitBlock                EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x15U)
#define EvtFreeRTOSTasks_TaskNotifyWait                     EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x16U)
#define EvtFreeRTOSTasks_TaskNotify                         EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x17U)
#define EvtFreeRTOSTasks_TaskNotifyFromIsr                  EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x18U)
#define EvtFreeRTOSTasks_TaskNotifyGiveFromIsr              EventID(EventLevelOp,     EvtFreeRTOSTasksNo, 0x19U)

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
#define EvtFreeRTOSTimers_TimerCreate                       EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x00U)
#define EvtFreeRTOSTimers_TimerCreateFailed                 EventID(EventLevelError,  EvtFreeRTOSTimersNo, 0x01U)
#define EvtFreeRTOSTimers_TimerCommandSend                  EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x02U)
#define EvtFreeRTOSTimers_TimerCommandReceived              EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x03U)
#define EvtFreeRTOSTimers_TimerExpired                      EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x04U)
#define EvtFreeRTOSTimers_PendFuncCall                      EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x05U)
#define EvtFreeRTOSTimers_PendFuncCallFromIsr               EventID(EventLevelOp,     EvtFreeRTOSTimersNo, 0x06U)

/* Event IDs for "FreeRTOS EventGroups" */
#define EvtFreeRTOSEventGroups_EventGroupCreate             EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x00U)
#define EvtFreeRTOSEventGroups_EventGroupCreateFailed       EventID(EventLevelError,  EvtFreeRTOSEventGroupsNo, 0x01U)
#define EvtFreeRTOSEventGroups_EventGroupSyncBlock          EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x02U)
#define EvtFreeRTOSEventGroups_EventGroupSyncEnd            EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x03U)
#define EvtFreeRTOSEventGroups_EventGroupWaitBitsBlock      EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x04U)
#define EvtFreeRTOSEventGroups_EventGroupWaitBitsEnd        EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x05U)
#define EvtFreeRTOSEventGroups_EventGroupClearBits          EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x06U)
#define EvtFreeRTOSEventGroups_EventGroupClearBitsFromIsr   EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x07U)
#define EvtFreeRTOSEventGroups_EventGroupSetBits            EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x08U)
#define EvtFreeRTOSEventGroups_EventGroupSetBitsFromIsr     EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x09U)
#define EvtFreeRTOSEventGroups_EventGroupDelete             EventID(EventLevelOp,     EvtFreeRTOSEventGroupsNo, 0x0AU)

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

#endif /* RTE_Compiler_EventRecorder */

/* Tasks */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_CREATE_DISABLE))
void EvrFreeRTOSTasks_TaskCreate (/*TCB_t*/void *pxNewTCB) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskCreate, (uint32_t)pxNewTCB, 0U);
#else
  (void)pxNewTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_CREATE_FAILED_DISABLE))
void EvrFreeRTOSTasks_TaskCreateFailed (void) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskCreateFailed, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_DELETE_DISABLE))
void EvrFreeRTOSTasks_TaskDelete (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskDelete, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_DELAY_UNTIL_DISABLE))
void EvrFreeRTOSTasks_TaskDelayUntil (uint32_t xTimeToWake) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskDelayUntil, xTimeToWake, 0U);
#else
  (void)xTimeToWake;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_DELAY_DISABLE))
void EvrFreeRTOSTasks_TaskDelay (uint32_t xTicksToDelay) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskDelay, xTicksToDelay, 0U);
#else
  (void)xTicksToDelay;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_PRIORITY_SET_DISABLE))
void EvrFreeRTOSTasks_TaskPrioritySet (/*TCB_t*/void *pxTCB, uint32_t uxNewPriority) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskPrioritySet, (uint32_t)pxTCB, uxNewPriority);
#else
  (void)pxTCB;
  (void)uxNewPriority;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_SUSPEND_DISABLE))
void EvrFreeRTOSTasks_TaskSuspend (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskSuspend, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_RESUME_DISABLE))
void EvrFreeRTOSTasks_TaskResume (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskResume, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_RESUME_FROM_ISR_DISABLE))
void EvrFreeRTOSTasks_TaskResumeFromIsr (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskResumeFromIsr, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_INCREMENT_TICK_DISABLE))
void EvrFreeRTOSTasks_TaskIncrementTick (uint32_t xTickCount) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskIncrementTick, xTickCount, 0U);
#else
  (void)xTickCount;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceINCREASE_TICK_COUNT_DISABLE))
void EvrFreeRTOSTasks_IncreaseTickCount (uint32_t xTicksToJump) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_IncreaseTickCount, xTicksToJump, 0U);
#else
  (void)xTicksToJump;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_SWITCHED_OUT_DISABLE))
void EvrFreeRTOSTasks_TaskSwitchedOut (/*TCB_t*/void *pxCurrentTCB) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskSwitchedOut, (uint32_t)pxCurrentTCB, 0U);
#else
  (void)pxCurrentTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_SWITCHED_IN_DISABLE))
void EvrFreeRTOSTasks_TaskSwitchedIn (/*TCB_t*/void *pxCurrentTCB, uint32_t uxTopPriority) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskSwitchedIn, (uint32_t)pxCurrentTCB, uxTopPriority);
#else
  (void)pxCurrentTCB;
  (void)uxTopPriority;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_PRIORITY_INHERIT_DISABLE))
void EvrFreeRTOSTasks_TaskPriorityInherit (/*TCB_t*/void *pxTCBOfMutexHolder, uint32_t uxInheritedPriority) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskPriorityInherit, (uint32_t)pxTCBOfMutexHolder, (uint32_t)uxInheritedPriority);
#else
  (void)pxTCBOfMutexHolder;
  (void)uxInheritedPriority;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_PRIORITY_DISINHERIT_DISABLE))
void EvrFreeRTOSTasks_TaskPriorityDisinherit (/*TCB_t*/void *pxTCBOfMutexHolder, uint32_t uxOriginalPriority) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskPriorityDisinherit, (uint32_t)pxTCBOfMutexHolder, (uint32_t)uxOriginalPriority);
#else
  (void)pxTCBOfMutexHolder;
  (void)uxOriginalPriority;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceMOVED_TASK_TO_READY_STATE_DISABLE))
void EvrFreeRTOSTasks_MovedTaskToReadyState (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_MovedTaskToReadyState, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(tracePOST_MOVED_TASK_TO_READY_STATE_DISABLE))
void EvrFreeRTOSTasks_PostMovedTaskToReadyState (/*TCB_t*/void *pxTCB) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_PostMovedTaskToReadyState, (uint32_t)pxTCB, 0U);
#else
  (void)pxTCB;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceLOW_POWER_IDLE_BEGIN_DISABLE))
void EvrFreeRTOSTasks_LowPowerIdleBegin (uint32_t xExpectedIdleTime) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_LowPowerIdleBegin, xExpectedIdleTime, 0U);
#else
  (void)xExpectedIdleTime;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceLOW_POWER_IDLE_END_DISABLE))
void EvrFreeRTOSTasks_LowPowerIdleEnd (void) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_LowPowerIdleEnd, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_TAKE_BLOCK_DISABLE))
void EvrFreeRTOSTasks_TaskNotifyTakeBlock (uint32_t uxIndexToWait, uint32_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskNotifyTakeBlock, uxIndexToWait, xTicksToWait);
#else
  (void)uxIndexToWait;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_TAKE_DISABLE))
void EvrFreeRTOSTasks_TaskNotifyTake (uint32_t uxIndexToWait, uint32_t ulNotifiedValue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskNotifyTake, uxIndexToWait, ulNotifiedValue);
#else
  (void)uxIndexToWait;
  (void)ulNotifiedValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_WAIT_BLOCK_DISABLE))
void EvrFreeRTOSTasks_TaskNotifyWaitBlock  (uint32_t uxIndexToWait, uint32_t xTicksToWait) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskNotifyWaitBlock, uxIndexToWait, xTicksToWait);
#else
  (void)uxIndexToWait;
  (void)xTicksToWait;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_WAIT_DISABLE))
void EvrFreeRTOSTasks_TaskNotifyWait (uint32_t uxIndexToWait, uint32_t ulNotifiedValue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTasks_TaskNotifyWait, uxIndexToWait, ulNotifiedValue);
#else
  (void)uxIndexToWait;
  (void)ulNotifiedValue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_DISABLE))
void EvrFreeRTOSTasks_TaskNotify (/*TCB_t*/void *xTaskToNotify, uint32_t uxIndexToWait, /*eNotifyAction*/uint32_t eAction, uint32_t ulNotifiedValue) {
#if defined(RTE_Compiler_EventRecorder)
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
#if defined(RTE_Compiler_EventRecorder)
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
#if defined(RTE_Compiler_EventRecorder)
  EventRecord4(EvtFreeRTOSTasks_TaskNotifyGiveFromIsr, (uint32_t)xTaskToNotify, uxIndexToWait, ulNotifiedValue, 0U);
#else
  (void)xTaskToNotify;
  (void)uxIndexToWait;
  (void)ulNotifiedValue;
#endif
}
#endif


/* Queue */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_CREATE_DISABLE))
void EvrFreeRTOSQueue_QueueCreate (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueCreate, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_CREATE_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueueCreateFailed (uint32_t ucQueueType) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueCreateFailed, ucQueueType, 0U);
#else
  (void)ucQueueType;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_MUTEX_DISABLE))
void EvrFreeRTOSQueue_CreateMutex (/*Queue_t*/void *pxNewQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_CreateMutex, (uint32_t)pxNewQueue, 0U);
#else
  (void)pxNewQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_MUTEX_FAILED_DISABLE))
void EvrFreeRTOSQueue_CreateMutexFailed (void) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_CreateMutexFailed, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceGIVE_MUTEX_RECURSIVE_DISABLE))
void EvrFreeRTOSQueue_GiveMutexRecursive (/*Queue_t*/void *pxMutex) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_GiveMutexRecursive, (uint32_t)pxMutex, 0U);
#else
  (void)pxMutex;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceGIVE_MUTEX_RECURSIVE_FAILED_DISABLE))
void EvrFreeRTOSQueue_GiveMutexRecursiveFailed (/*Queue_t*/void *pxMutex) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_GiveMutexRecursiveFailed, (uint32_t)pxMutex, 0U);
#else
  (void)pxMutex;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTAKE_MUTEX_RECURSIVE_DISABLE))
void EvrFreeRTOSQueue_TakeMutexRecursive (/*Queue_t*/void *pxMutex) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_TakeMutexRecursive, (uint32_t)pxMutex, 0U);
#else
  (void)pxMutex;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTAKE_MUTEX_RECURSIVE_FAILED_DISABLE))
void EvrFreeRTOSQueue_TakeMutexRecursiveFailed (/*Queue_t*/void *pxMutex) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_TakeMutexRecursiveFailed, (uint32_t)pxMutex, 0U);
#else
  (void)pxMutex;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_COUNTING_SEMAPHORE_DISABLE))
void EvrFreeRTOSQueue_CreateCountingSemaphore (/*Queue_t*/void *xHandle) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_CreateCountingSemaphore, (uint32_t)xHandle, 0U);
#else
  (void)xHandle;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_COUNTING_SEMAPHORE_FAILED_DISABLE))
void EvrFreeRTOSQueue_CreateCountingSemaphoreFailed (void) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_CreateCountingSemaphoreFailed, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_DISABLE))
void EvrFreeRTOSQueue_QueueSend (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueSend, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueueSendFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueSendFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_DISABLE))
void EvrFreeRTOSQueue_QueueReceive (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueReceive, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_DISABLE))
void EvrFreeRTOSQueue_QueuePeek (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueuePeek, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueuePeekFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueuePeekFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_FROM_ISR_DISABLE))
void EvrFreeRTOSQueue_QueuePeekFromIsr (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueuePeekFromIsr, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueueReceiveFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueReceiveFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_FROM_ISR_DISABLE))
void EvrFreeRTOSQueue_QueueSendFromIsr (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueSendFromIsr, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_FROM_ISR_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueueSendFromIsrFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueSendFromIsrFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_FROM_ISR_DISABLE))
void EvrFreeRTOSQueue_QueueReceiveFromIsr (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueReceiveFromIsr, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_FROM_ISR_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueueReceiveFromIsrFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueReceiveFromIsrFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_FROM_ISR_FAILED_DISABLE))
void EvrFreeRTOSQueue_QueuePeekFromIsrFailed (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueuePeekFromIsrFailed, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_DELETE_DISABLE))
void EvrFreeRTOSQueue_QueueDelete (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueDelete, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_REGISTRY_ADD_DISABLE))
void EvrFreeRTOSQueue_QueueRegistryAdd (/*Queue_t*/void *pxQueue, const char *pcQueueName) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_QueueRegistryAdd, (uint32_t)pxQueue, (uint32_t)pcQueueName);
#else
  (void)pxQueue;
  (void)pcQueueName;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_QUEUE_RECEIVE_DISABLE))
void EvrFreeRTOSQueue_BlockingOnQueueReceive (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_BlockingOnQueueReceive, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_QUEUE_SEND_DISABLE))
void EvrFreeRTOSQueue_BlockingOnQueueSend (/*Queue_t*/void *pxQueue) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSQueue_BlockingOnQueueSend, (uint32_t)pxQueue, 0U);
#else
  (void)pxQueue;
#endif
}
#endif


/* Timers */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_CREATE_DISABLE))
void EvrFreeRTOSTimers_TimerCreate (/*Timer_t*/void *pxNewTimer) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_TimerCreate, (uint32_t)pxNewTimer, 0U);
#else
  (void)pxNewTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_CREATE_FAILED_DISABLE))
void EvrFreeRTOSTimers_TimerCreateFailed (void) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_TimerCreateFailed, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_COMMAND_SEND_DISABLE))
void EvrFreeRTOSTimers_TimerCommandSend (/*Timer_t*/void *pxTimer, uint32_t xCommandID, uint32_t xOptionalValue, uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder)
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
#if defined(RTE_Compiler_EventRecorder)
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
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSTimers_TimerExpired, (uint32_t)pxTimer, 0U);
#else
  (void)pxTimer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(tracePEND_FUNC_CALL_DISABLE))
void EvrFreeRTOSTimers_PendFuncCall (/*PendedFunction_t*/void *pxFunctionToPend, void *pvParameter1, uint32_t ulParameter2, uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_PendFuncCall, (uint32_t)pxFunctionToPend, (uint32_t)pvParameter1, ulParameter2, xReturn);
#else
  (void)pxFunctionToPend;
  (void)pvParameter1;
  (void)ulParameter2;
  (void)xReturn;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(tracePEND_FUNC_CALL_FROM_ISR_DISABLE))
void EvrFreeRTOSTimers_PendFuncCallFromIsr (/*PendedFunction_t*/void *pxFunctionToPend, void *pvParameter1, uint32_t ulParameter2, uint32_t xReturn) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord4(EvtFreeRTOSTimers_PendFuncCallFromIsr, (uint32_t)pxFunctionToPend, (uint32_t)pvParameter1, ulParameter2, xReturn);
#else
  (void)pxFunctionToPend;
  (void)pvParameter1;
  (void)ulParameter2;
  (void)xReturn;
#endif
}
#endif


/* Event Groups */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CREATE_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupCreate (/*EventGroup_t*/void *pxEventGroup) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupCreate, (uint32_t)pxEventGroup, 0U);
#else
  (void)pxEventGroup;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CREATE_FAILED_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupCreateFailed (void) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupCreateFailed, 0U, 0U);
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SYNC_BLOCK_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupSyncBlock (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToSet, uint32_t uxBitsToWaitFor) {
#if defined(RTE_Compiler_EventRecorder)
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
#if defined(RTE_Compiler_EventRecorder)
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
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupWaitBitsBlock, (uint32_t)pxEventGroup, uxBitsToWaitFor);
#else
  (void)pxEventGroup;
  (void)uxBitsToWaitFor;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_WAIT_BITS_END_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupWaitBitsEnd (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToWaitFor, uint32_t xTimeoutOccurred) {
#if defined(RTE_Compiler_EventRecorder)
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
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupClearBits, (uint32_t)pxEventGroup, uxBitsToClear);
#else
  (void)pxEventGroup;
  (void)uxBitsToClear;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupClearBitsFromIsr (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToClear) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupClearBitsFromIsr, (uint32_t)pxEventGroup, uxBitsToClear);
#else
  (void)pxEventGroup;
  (void)uxBitsToClear;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SET_BITS_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupSetBits (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToSet) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupSetBits, (uint32_t)pxEventGroup, uxBitsToSet);
#else
  (void)pxEventGroup;
  (void)uxBitsToSet;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SET_BITS_FROM_ISR_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupSetBitsFromIsr (/*EventGroup_t*/void *pxEventGroup, uint32_t uxBitsToSet) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupSetBitsFromIsr, (uint32_t)pxEventGroup, uxBitsToSet);
#else
  (void)pxEventGroup;
  (void)uxBitsToSet;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_DELETE_DISABLE))
void EvrFreeRTOSEventGroups_EventGroupDelete (/*EventGroup_t*/void *pxEventGroup) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSEventGroups_EventGroupDelete, (uint32_t)pxEventGroup, 0U);
#else
  (void)pxEventGroup;
#endif
}
#endif


/* Stream Buffer */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_FAILED_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferCreateFailed (uint32_t uxIsMessageBuffer) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferCreateFailed, (uint32_t)uxIsMessageBuffer, 0U);
#else
  (void)uxIsMessageBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_STATIC_FAILED_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferCreateStaticFailed (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t uxIsMessageBuffer) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferCreateStaticFailed, (uint32_t)pxStreamBuffer, uxIsMessageBuffer);
#else
  (void)pxStreamBuffer;
  (void)uxIsMessageBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferCreate (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t uxIsMessageBuffer) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferCreate, (uint32_t)pxStreamBuffer, uxIsMessageBuffer);
#else
  (void)pxStreamBuffer;
  (void)uxIsMessageBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_DELETE_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferDelete (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferDelete, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RESET_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferReset (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferReset, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_STREAM_BUFFER_SEND_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferBlockingOnSend (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferBlockingOnSend, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_SEND_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferSend (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t xBytesSent) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferSend, (uint32_t)pxStreamBuffer, xBytesSent);
#else
  (void)pxStreamBuffer;
  (void)xBytesSent;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_SEND_FAILED_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferSendFailed (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferSendFailed, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_SEND_FROM_ISR_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferSendFromIsr (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t xBytesSent) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferSendFromIsr, (uint32_t)pxStreamBuffer, xBytesSent);
#else
  (void)pxStreamBuffer;
  (void)xBytesSent;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_STREAM_BUFFER_RECEIVE_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferBlockingOnReceive (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferBlockingOnReceive, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RECEIVE_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferReceive (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t xReceivedLength) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferReceive, (uint32_t)pxStreamBuffer, xReceivedLength);
#else
  (void)pxStreamBuffer;
  (void)xReceivedLength;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RECEIVE_FAILED_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferReceiveFailed (/*StreamBuffer_t*/void *pxStreamBuffer) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSStreamBuf_StreamBufferReceiveFailed, (uint32_t)pxStreamBuffer, 0U);
#else
  (void)pxStreamBuffer;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RECEIVE_FROM_ISR_DISABLE))
void EvrFreeRTOSStreamBuf_StreamBufferReceiveFromIsr (/*StreamBuffer_t*/void *pxStreamBuffer, uint32_t xReceivedLength) {
#if defined(RTE_Compiler_EventRecorder)
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
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSHeap_Malloc, (uint32_t)pvAddress, uiSize);
#else
  (void)pvAddress;
  (void)uiSize;
#endif
}
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceFREE_DISABLE))
void EvrFreeRTOSHeap_Free (void *pvAddress, uint32_t uiSize) {
#if defined(RTE_Compiler_EventRecorder)
  EventRecord2(EvtFreeRTOSHeap_Free, (uint32_t)pvAddress, uiSize);
#else
  (void)pvAddress;
  (void)uiSize;
#endif
}
#endif
