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
 *      Name:    freertos_evr.h
 *      Purpose: FreeRTOS Event Recorder definitions
 *
 *---------------------------------------------------------------------------*/

#ifndef FREERTOS_EVR_H_
#define FREERTOS_EVR_H_

#include <string.h>
#include <stdint.h>

#include "RTE_Components.h"

#if !defined(RTE_Compiler_EventRecorder)
  /* Disable debug events if Event Recorder is not used */
  #ifndef EVR_FREERTOS_DISABLE
    #define EVR_FREERTOS_DISABLE
  #endif
#endif

/* Temporarily define FreeRTOS object types */
#define TCB_t            void*
#define Queue_t          void*
#define Timer_t          void*
#define PendedFunction_t void*
#define EventGroup_t     void*
#define StreamBuffer_t   void*

/**
  \brief  Setup Event Recorder configuration
  \param[in]  reset             reset if already configured (0:no reset, 1:reset)
*/
extern void EvrFreeRTOSSetup (uint32_t reset);

/**
  \brief  Event on successful task create (Op)
  \param[in]  pxNewTCB          pointer to task handle.
*/
extern void EvrFreeRTOSTasks_TaskCreate (TCB_t pxNewTCB);

/**
  \brief  Event on unsuccessful task create (Error)
*/
extern void EvrFreeRTOSTasks_TaskCreateFailed (void);

/**
  \brief  Event on task delete (Op)
  \param[in]  pxTCB             pointer to task handle.
*/
extern void EvrFreeRTOSTasks_TaskDelete (TCB_t pxTCB);

/**
  \brief  Event before current running task execution is delayed (Op)
  \param[in]  xTimeToWake       wake time (ticks).
*/
extern void EvrFreeRTOSTasks_TaskDelayUntil (uint32_t xTimeToWake);

/**
  \brief  Event on current thread execution delay (Op)
  \param[in]  xTicksToDelay     number of ticks to delay.
*/
extern void EvrFreeRTOSTasks_TaskDelay (uint32_t xTicksToDelay);

/**
  \brief  Event on task priority set (Op)
  \param[in]  pxTCB             pointer to task handle.
  \param[in]  uxNewPriority
*/
extern void EvrFreeRTOSTasks_TaskPrioritySet (TCB_t pxTCB, uint32_t uxNewPriority);

/**
  \brief  Event on task suspend (Op)
  \param[in]  pxTCB             pointer to task handle.
*/
extern void EvrFreeRTOSTasks_TaskSuspend (TCB_t pxTCB);

/**
  \brief  Event on task resume (Op)
  \param[in]  pxTCB             pointer to task handle.
*/
extern void EvrFreeRTOSTasks_TaskResume (TCB_t pxTCB);

/**
  \brief  Event on task resume call from ISR (Op)
  \param[in]  pxTCB             pointer to task handle.
*/
extern void EvrFreeRTOSTasks_TaskResumeFromIsr (TCB_t pxTCB);

/**
  \brief  Event on tick count increment (Detail)
  \param[in]  xTickCount        tick count before increment.
*/
extern void EvrFreeRTOSTasks_TaskIncrementTick (uint32_t xTickCount);

/**
  \brief  Event on tick count increase (Op)
  \param[in]  xTicksToJump      number of ticks by which the tick count is increased.
*/
extern void EvrFreeRTOSTasks_IncreaseTickCount (uint32_t xTicksToJump);

/**
  \brief  Event before a new task is selected to run (Op)
  \param[in]  pxCurrentTCB      handle to the task about to leave the running state.
*/
extern void EvrFreeRTOSTasks_TaskSwitchedOut (TCB_t pxCurrentTCB);

/**
  \brief  Event after a task has been selected to run (Op)
  \param[in]  pxCurrentTCB      handle to the task about to enter the running state.
  \param[in]  uxTopPriority     pxCurrentTCB task priority.
*/
extern void EvrFreeRTOSTasks_TaskSwitchedIn  (TCB_t pxCurrentTCB, uint32_t uxTopPriority);

/**
  \brief  Event on task priority inheritance (Op)
  \param[in]  pxTCBOfMutexHolder  pointer to task handle.
  \param[in]  uxInheritedPriority new (inherited) task priority.
*/
extern void EvrFreeRTOSTasks_TaskPriorityInherit (TCB_t pxTCBOfMutexHolder, uint32_t uxInheritedPriority);

/**
  \brief  Event on task priority disinheritance (Op)
  \param[in]  pxTCBOfMutexHolder  pointer to task handle.
  \param[in]  uxOriginalPriority  old (original) task priority.
*/
extern void EvrFreeRTOSTasks_TaskPriorityDisinherit (TCB_t pxTCBOfMutexHolder, uint32_t uxOriginalPriority);

/**
  \brief  Event sent before transition of a task into the Ready state (Op)
  \param[in]  pxTCB             pointer to task handle.
*/
extern void EvrFreeRTOSTasks_MovedTaskToReadyState (TCB_t pxTCB);

/**
  \brief  Event sent after transition of a task into the Ready state (Op)
  \param[in]  pxTCB             pointer to task handle.
*/
extern void EvrFreeRTOSTasks_PostMovedTaskToReadyState (TCB_t pxTCB);

/**
  \brief  Event on enter to the low power mode (Op)
  \param[in]  xExpectedIdleTime expected idle time in ticks.
*/
extern void EvrFreeRTOSTasks_LowPowerIdleBegin (uint32_t xExpectedIdleTime);

/**
  \brief  Event on exit from the low power mode (Op)
*/
extern void EvrFreeRTOSTasks_LowPowerIdleEnd (void);

/**
  \brief  Event on indicating the current running task is about to block while waiting for notification to be non-zero (Op)
  \param[in]  uxIndexToWait     task notification index.
  \param[in]  xTicksToWait      wait timeout in ticks.
*/
extern void EvrFreeRTOSTasks_TaskNotifyTakeBlock (uint32_t uxIndexToWait, uint32_t xTicksToWait);

/**
  \brief  Event on successful task notify take (Op)
  \param[in]  uxIndexToWait     task notification index.
  \param[in]  ulNotifiedValue   current state of the notification value.
*/
extern void EvrFreeRTOSTasks_TaskNotifyTake (uint32_t uxIndexToWait, uint32_t ulNotifiedValue);

/**
  \brief  Event indicating the current running task is about to block while waiting to receive notification (Op)
  \param[in]  uxIndexToWait     task notification index
  \param[in]  xTicksToWait      wait timeout in ticks.
*/
extern void EvrFreeRTOSTasks_TaskNotifyWaitBlock (uint32_t uxIndexToWait, uint32_t xTicksToWait);

/**
  \brief  Event on successful task notify wait (Op)
  \param[in]  uxIndexToWait     task notification index.
  \param[in]  ulNotifiedValue   current state of the notification value.
*/
extern void EvrFreeRTOSTasks_TaskNotifyWait (uint32_t uxIndexToWait, uint32_t ulNotifiedValue);

/**
  \brief  Event on successful task notify (Op)
  \param[in]  xTaskToNotify     pointer to task to be notified.
  \param[in]  uxIndexToWait     task notification index.
  \param[in]  eAction           task notification action.
  \param[in]  ulNotifiedValue   current state of the notification value.
*/
extern void EvrFreeRTOSTasks_TaskNotify (TCB_t xTaskToNotify, uint32_t uxIndexToWait, uint32_t eAction, uint32_t ulNotifiedValue);

/**
  \brief  Event on successful task notify from ISR (Op)
  \param[in]  xTaskToNotify     pointer to task to be notified.
  \param[in]  uxIndexToWait     task notification index.
  \param[in]  eAction           task notification action.
  \param[in]  ulNotifiedValue   current state of the notification value.
*/
extern void EvrFreeRTOSTasks_TaskNotifyFromIsr (TCB_t xTaskToNotify, uint32_t uxIndexToWait, uint32_t eAction, uint32_t ulNotifiedValue);

/**
  \brief  Event on successful task notify give from ISR (Op)
  \param[in]  xTaskToNotify     pointer to task to be notified.
  \param[in]  uxIndexToWait     task notification index.
  \param[in]  ulNotifiedValue   current state of the notification value.
*/
extern void EvrFreeRTOSTasks_TaskNotifyGiveFromIsr (TCB_t xTaskToNotify, uint32_t uxIndexToWait, uint32_t ulNotifiedValue);

/**
  \brief  Event on successful queue create (Op)
  \param[in]  pxQueue           pointer to mutex object handle.
*/
extern void EvrFreeRTOSQueue_QueueCreate (Queue_t pxQueue);

/**
  \brief  Event on unsuccessful queue create (Error)
  \param[in]  ucQueueType       queue type
*/
extern void EvrFreeRTOSQueue_QueueCreateFailed (uint32_t ucQueueType);

/**
  \brief  Event on successful mutex create (Op)
  \param[in]  pxNewQueue        pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_CreateMutex (Queue_t pxNewQueue);

/**
  \brief  Event on unsuccessful mutex create (Error)
*/
extern void EvrFreeRTOSQueue_CreateMutexFailed (void);

/**
  \brief  Event on successful recursive mutex give (Op)
  \param[in]  pxMutex           pointer to mutex object handle.
*/
extern void EvrFreeRTOSQueue_GiveMutexRecursive (Queue_t pxMutex);

/**
  \brief  Event on unsuccessful recursive mutex give (Error)
  \param[in]  pxMutex           pointer to mutex object handle.
*/
extern void EvrFreeRTOSQueue_GiveMutexRecursiveFailed (Queue_t pxMutex);

/**
  \brief  Event on successful recursive mutex take (Op)
  \param[in]  pxMutex           pointer to mutex object handle.
*/
extern void EvrFreeRTOSQueue_TakeMutexRecursive (Queue_t pxMutex);

/**
  \brief  Event on unsuccessful recursive mutex take (Error)
  \param[in]  pxMutex           pointer to mutex object handle.
*/
extern void EvrFreeRTOSQueue_TakeMutexRecursiveFailed (Queue_t pxMutex);

/**
  \brief  Event on successful counting semaphore create (Op)
  \param[in]  xHandle           pointer to semaphore object handle.
*/
extern void EvrFreeRTOSQueue_CreateCountingSemaphore (Queue_t xHandle);

/**
  \brief  Event on unsuccessful counting semaphore create (Error)
*/
extern void EvrFreeRTOSQueue_CreateCountingSemaphoreFailed (void);

/**
  \brief  Event on queue write (Op)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueueSend (Queue_t pxQueue);

/**
  \brief  Event on write to the full queue (Error)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueueSendFailed (Queue_t pxQueue);

/**
  \brief  Event on queue read (Op)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueueReceive (Queue_t pxQueue);

/**
  \brief  Event on queue peek (Op)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueuePeek (Queue_t pxQueue);

/**
  \brief  Event on queue peek when queue is empty and no block time was specified (Error)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueuePeekFailed (Queue_t pxQueue);

/**
  \brief  Event on queue peek from ISR (Op)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueuePeekFromIsr (Queue_t pxQueue);

/**
  \brief  Event on read from the empty queue (Error)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueueReceiveFailed (Queue_t pxQueue);

/**
  \brief  Event on write to the queue from ISR (Op)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueueSendFromIsr (Queue_t pxQueue);

/**
  \brief  Event on write to the full queue from ISR (Error)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueueSendFromIsrFailed (Queue_t pxQueue);

/**
  \brief  Event on queue read from ISR (Op)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueueReceiveFromIsr (Queue_t pxQueue);

/**
  \brief  Event on empty queue read from ISR (Error)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueueReceiveFromIsrFailed (Queue_t pxQueue);

/**
  \brief  Event on empty queue peek from ISR (Error)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueuePeekFromIsrFailed (Queue_t pxQueue);

/**
  \brief  Event on queue delete (Op)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_QueueDelete (Queue_t pxQueue);

/**
  \brief  Event on assignment of a human readable name to a queue (Op)
  \param[in]  pxQueue           pointer to queue object handle.
  \param[in]  pcQueueName       pointer to queue object name.
*/
extern void EvrFreeRTOSQueue_QueueRegistryAdd (Queue_t pxQueue, const char *pcQueueName);

/**
  \brief  Event indicating the current running task is about to block while reading from an empty queue (Op)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_BlockingOnQueueReceive (Queue_t pxQueue);

/**
  \brief  Event indicating the current running task is about to block while writting to a full queue (Op)
  \param[in]  pxQueue           pointer to queue object handle.
*/
extern void EvrFreeRTOSQueue_BlockingOnQueueSend (Queue_t pxQueue);

/**
  \brief  Event on successful timer object create (Op)
  \param[in]  pxNewTimer        pointer to timer object handle.
*/
extern void EvrFreeRTOSTimers_TimerCreate (Timer_t pxNewTimer);

/**
  \brief  Event on unsuccessful timer object create (Error)
*/
extern void EvrFreeRTOSTimers_TimerCreateFailed (void);

/**
  \brief  Event on timer queue command send (Op)
  \param[in]  pxTimer           pointer to timer object handle.
  \param[in]  xCommandID        timer command ID.
  \param[in]  xOptionalValue    optional command value.
  \param[in]  xReturn           return value.
*/
extern void EvrFreeRTOSTimers_TimerCommandSend (Timer_t pxTimer, uint32_t xCommandID, uint32_t xOptionalValue, uint32_t xReturn);

/**
  \brief  Event on timer queue command receive (Op)
  \param[in]  pxTimer           pointer to timer object handle.
  \param[in]  xCommandID        timer command ID.
  \param[in]  xOptionalValue    optional command value.
*/
extern void EvrFreeRTOSTimers_TimerCommandReceived (Timer_t pxTimer, uint32_t xCommandID, uint32_t xOptionalValue);

/**
  \brief  Event on timer expire (Op)
  \param[in]  pxTimer           pointer to timer object handle.
*/
extern void EvrFreeRTOSTimers_TimerExpired (Timer_t pxTimer);

/**
  \brief  Event on pass of the function execution to the timer service task (Op)
  \param[in]  pxFunctionToPend  pointer to callback function
  \param[in]  pvParameter1      function parameter 1.
  \param[in]  ulParameter2      function parameter 2.
  \param[in]  xReturn           return value.
*/
extern void EvrFreeRTOSTimers_PendFuncCall (PendedFunction_t pxFunctionToPend, void *pvParameter1, uint32_t ulParameter2, uint32_t xReturn);

/**
  \brief  Event on pass of the function execution to the timer service task from the ISR (Op)
  \param[in]  pxFunctionToPend  pointer to callback function
  \param[in]  pvParameter1      function parameter 1.
  \param[in]  ulParameter2      function parameter 2.
  \param[in]  xReturn           return value.
*/
extern void EvrFreeRTOSTimers_PendFuncCallFromIsr (PendedFunction_t pxFunctionToPend, void *pvParameter1, uint32_t ulParameter2, uint32_t xReturn);

/**
  \brief  Event on successful event groups object create (Op)
  \param[in]  pxEventGroup      pointer to Event Groups object handle.
*/
extern void EvrFreeRTOSEventGroups_EventGroupCreate (EventGroup_t pxEventGroup);

/**
  \brief  Event on unsuccessful event groups object create (Error)
*/
extern void EvrFreeRTOSEventGroups_EventGroupCreateFailed (void);

/**
  \brief  Event on event groups sync wait start (Op)
  \param[in]  pxEventGroup      pointer to Event Groups object handle.
  \param[in]  uxBitsToSet       event bits that shall be set.
  \param[in]  uxBitsToWaitFor   event bits to wait for.
*/
extern void EvrFreeRTOSEventGroups_EventGroupSyncBlock (EventGroup_t pxEventGroup, uint32_t uxBitsToSet, uint32_t uxBitsToWaitFor);

/**
  \brief  Event on event groups sync completed (Op)
  \param[in]  pxEventGroup      pointer to Event Groups object handle.
  \param[in]  uxBitsToSet       event bits that shall be set.
  \param[in]  uxBitsToWaitFor   event bits to wait for.
  \param[in]  xTimeoutOccurred  timeout value.
*/
extern void EvrFreeRTOSEventGroups_EventGroupSyncEnd (EventGroup_t pxEventGroup, uint32_t uxBitsToSet, uint32_t uxBitsToWaitFor, uint32_t xTimeoutOccurred);

/**
  \brief  Event on event groups bit wait start (Op)
  \param[in]  pxEventGroup      pointer to Event Groups object handle.
  \param[in]  uxBitsToWaitFor   event bits to wait for.
*/
extern void EvrFreeRTOSEventGroups_EventGroupWaitBitsBlock (EventGroup_t pxEventGroup, uint32_t uxBitsToWaitFor);

/**
  \brief  Event on event groups bit wait completed (Op)
  \param[in]  pxEventGroup      pointer to Event Groups object handle.
  \param[in]  uxBitsToWaitFor   event bits to wait for.
  \param[in]  xTimeoutOccurred  timeout value.
*/
extern void EvrFreeRTOSEventGroups_EventGroupWaitBitsEnd (EventGroup_t pxEventGroup, uint32_t uxBitsToWaitFor, uint32_t xTimeoutOccurred);

/**
  \brief  Event on event groups bit clear (Op)
  \param[in]  pxEventGroup      pointer to Event Groups object handle.
  \param[in]  uxBitsToClear     event bits that shall be cleared.
*/
extern void EvrFreeRTOSEventGroups_EventGroupClearBits (EventGroup_t pxEventGroup, uint32_t uxBitsToClear);

/**
  \brief  Event on event groups bit clear call from ISR (Op)
  \param[in]  pxEventGroup      pointer to Event Groups object handle.
  \param[in]  uxBitsToClear     event bits that shall be cleared.
*/
extern void EvrFreeRTOSEventGroups_EventGroupClearBitsFromIsr (EventGroup_t pxEventGroup, uint32_t uxBitsToClear);

/**
  \brief  Event on event groups bit set (Op)
  \param[in]  pxEventGroup      pointer to Event Groups object handle.
  \param[in]  uxBitsToSet       event bits that shall be set.
*/
extern void EvrFreeRTOSEventGroups_EventGroupSetBits (EventGroup_t pxEventGroup, uint32_t uxBitsToSet);

/**
  \brief  Event on event groups bit set call from ISR (Op)
  \param[in]  pxEventGroup      pointer to Event Groups object handle.
  \param[in]  uxBitsToSet       event bits that shall be set.
*/
extern void EvrFreeRTOSEventGroups_EventGroupSetBitsFromIsr (EventGroup_t pxEventGroup, uint32_t uxBitsToSet);

/**
  \brief  Event on event groups object delete (Op)
  \param[in]  pxEventGroup      pointer to Event Groups object handle.
*/
extern void EvrFreeRTOSEventGroups_EventGroupDelete (EventGroup_t pxEventGroup);

/**
  \brief  Event on unsuccessful stream buffer object create (Error)
  \param[in]  uxIsMessageBuffer buffer type designator (0:stream, 1:message).
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferCreateFailed (uint32_t uxIsMessageBuffer);

/**
  \brief  Event on unsuccessful stream buffer object create (Error)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
  \param[in]  uxIsMessageBuffer buffer type designator (0:stream, 1:message).
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferCreateStaticFailed (StreamBuffer_t pxStreamBuffer, uint32_t uxIsMessageBuffer);

/**
  \brief  Event on successful stream buffer object create (Op)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
  \param[in]  uxIsMessageBuffer buffer type designator (0:stream, 1:message).
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferCreate (StreamBuffer_t pxStreamBuffer, uint32_t uxIsMessageBuffer);

/**
  \brief  Event on stream buffer object delete (Op)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferDelete (StreamBuffer_t pxStreamBuffer);

/**
  \brief  Event on stream buffer object reset (Op)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferReset (StreamBuffer_t pxStreamBuffer);

/**
  \brief  Event on stream buffer send when buffer is full and sending task is blocked (Op)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferBlockingOnSend (StreamBuffer_t pxStreamBuffer);

/**
  \brief  Event on stream buffer send when data is successfully copied into the buffer (Op)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
  \param[in]  xBytesSent        number of bytes copied into the buffer
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferSend (StreamBuffer_t pxStreamBuffer, uint32_t xBytesSent);

/**
  \brief  Event on stream buffer send when data is not copied into the buffer (Error)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferSendFailed (StreamBuffer_t pxStreamBuffer);

/**
  \brief  Event on stream buffer send from ISR when data is successfully copied into the buffer (Op)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
  \param[in]  xBytesSent        number of bytes copied into the buffer
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferSendFromIsr (StreamBuffer_t pxStreamBuffer, uint32_t xBytesSent);

/**
  \brief  Event on stream buffer receive when buffer is empty and receiving task is blocked (Op)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferBlockingOnReceive (StreamBuffer_t pxStreamBuffer);

/**
  \brief  Event on stream buffer receive when data is successfully copied from the buffer (Op)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
  \param[in]  xReceivedLength   number of bytes copied from the buffer
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferReceive (StreamBuffer_t pxStreamBuffer, uint32_t xReceivedLength);

/**
  \brief  Event on stream buffer receive when data is not copied from the buffer (Error)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferReceiveFailed (StreamBuffer_t pxStreamBuffer);

/**
  \brief  Event on stream buffer receive from ISR when data is successfully copied from the buffer (Op)
  \param[in]  pxStreamBuffer    pointer to Stream Buffer object handle.
  \param[in]  xReceivedLength   number of bytes copied from the buffer
*/
extern void EvrFreeRTOSStreamBuf_StreamBufferReceiveFromIsr (StreamBuffer_t pxStreamBuffer, uint32_t xReceivedLength);

/**
  \brief  Event on heap memory block allocation (Op)
  \param[in]  pvAddress         pointer to memory block.
  \param[in]  uiSize            memory block size.
*/
extern void EvrFreeRTOSHeap_Malloc (void *pvAddress, uint32_t uiSize);

/**
  \brief  Event on heap memory block free (Op)
  \param[in]  pvAddress         pointer to memory block.
  \param[in]  uiSize            memory block size.
*/
extern void EvrFreeRTOSHeap_Free (void *pvAddress, uint32_t uiSize);


/* Tasks */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_CREATE_DISABLE))
  #define traceTASK_CREATE(px)                        EvrFreeRTOSTasks_TaskCreate(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_CREATE_FAILED_DISABLE))
  #define traceTASK_CREATE_FAILED()                   EvrFreeRTOSTasks_TaskCreateFailed()
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_DELETE_DISABLE))
  #define traceTASK_DELETE(px)                        EvrFreeRTOSTasks_TaskDelete(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_DELAY_UNTIL_DISABLE))
  #define traceTASK_DELAY_UNTIL(x)                    EvrFreeRTOSTasks_TaskDelayUntil(x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_DELAY_DISABLE))
  #define traceTASK_DELAY()                           EvrFreeRTOSTasks_TaskDelay(xTicksToDelay)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_PRIORITY_SET_DISABLE))
  #define traceTASK_PRIORITY_SET(px,ux)               EvrFreeRTOSTasks_TaskPrioritySet(px,ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_SUSPEND_DISABLE))
  #define traceTASK_SUSPEND(px)                       EvrFreeRTOSTasks_TaskSuspend(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_RESUME_DISABLE))
  #define traceTASK_RESUME(px)                        EvrFreeRTOSTasks_TaskResume(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_RESUME_FROM_ISR_DISABLE))
  #define traceTASK_RESUME_FROM_ISR(px)               EvrFreeRTOSTasks_TaskResumeFromIsr(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_INCREMENT_TICK_DISABLE))
  #define traceTASK_INCREMENT_TICK(x)                 EvrFreeRTOSTasks_TaskIncrementTick(x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceINCREASE_TICK_COUNT_DISABLE))
  #define traceINCREASE_TICK_COUNT(x)                 EvrFreeRTOSTasks_IncreaseTickCount(x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_SWITCHED_OUT_DISABLE))
  #define traceTASK_SWITCHED_OUT()                    EvrFreeRTOSTasks_TaskSwitchedOut(pxCurrentTCB)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_SWITCHED_IN_DISABLE))
  #define traceTASK_SWITCHED_IN()                     EvrFreeRTOSTasks_TaskSwitchedIn(pxCurrentTCB, uxTopReadyPriority)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_PRIORITY_INHERIT_DISABLE))
  #define traceTASK_PRIORITY_INHERIT(px,ux)           EvrFreeRTOSTasks_TaskPriorityInherit(px,ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_PRIORITY_DISINHERIT_DISABLE))
  #define traceTASK_PRIORITY_DISINHERIT(px,ux)        EvrFreeRTOSTasks_TaskPriorityDisinherit(px,ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceMOVED_TASK_TO_READY_STATE_DISABLE))
  #define traceMOVED_TASK_TO_READY_STATE(px)          EvrFreeRTOSTasks_MovedTaskToReadyState(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(tracePOST_MOVED_TASK_TO_READY_STATE_DISABLE))
  #define tracePOST_MOVED_TASK_TO_READY_STATE(px)     EvrFreeRTOSTasks_PostMovedTaskToReadyState(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceLOW_POWER_IDLE_BEGIN_DISABLE))
  #define traceLOW_POWER_IDLE_BEGIN()                 EvrFreeRTOSTasks_LowPowerIdleBegin(xExpectedIdleTime)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceLOW_POWER_IDLE_END_DISABLE))
  #define traceLOW_POWER_IDLE_END()                   EvrFreeRTOSTasks_LowPowerIdleEnd()
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_TAKE_BLOCK_DISABLE))
  #define traceTASK_NOTIFY_TAKE_BLOCK(ux)             EvrFreeRTOSTasks_TaskNotifyTakeBlock(ux, xTicksToWait)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_TAKE_DISABLE))
  #define traceTASK_NOTIFY_TAKE(ux)                   EvrFreeRTOSTasks_TaskNotifyTake(ux, pxCurrentTCB->ulNotifiedValue[ux])
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_WAIT_BLOCK_DISABLE))
  #define traceTASK_NOTIFY_WAIT_BLOCK(ux)             EvrFreeRTOSTasks_TaskNotifyWaitBlock(ux, xTicksToWait)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_WAIT_DISABLE))
  #define traceTASK_NOTIFY_WAIT(ux)                   EvrFreeRTOSTasks_TaskNotifyWait(ux, pxCurrentTCB->ulNotifiedValue[ux])
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_DISABLE))
  #define traceTASK_NOTIFY(ux)                        EvrFreeRTOSTasks_TaskNotify(pxTCB, ux, eAction, pxTCB->ulNotifiedValue[ux])
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_FROM_ISR_DISABLE))
  #define traceTASK_NOTIFY_FROM_ISR(ux)               EvrFreeRTOSTasks_TaskNotifyFromIsr(pxTCB, ux, eAction, pxTCB->ulNotifiedValue[ux])
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTASK_NOTIFY_GIVE_FROM_ISR_DISABLE))
  #define traceTASK_NOTIFY_GIVE_FROM_ISR(ux)          EvrFreeRTOSTasks_TaskNotifyGiveFromIsr(pxTCB, ux, pxTCB->ulNotifiedValue[ux])
#endif


/* Queue */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_CREATE_DISABLE))
  #define traceQUEUE_CREATE(px)                       EvrFreeRTOSQueue_QueueCreate(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_CREATE_FAILED_DISABLE))
  #define traceQUEUE_CREATE_FAILED(uc)                EvrFreeRTOSQueue_QueueCreateFailed(uc)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_MUTEX_DISABLE))
  #define traceCREATE_MUTEX(px)                       EvrFreeRTOSQueue_CreateMutex(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_MUTEX_FAILED_DISABLE))
  #define traceCREATE_MUTEX_FAILED()                  EvrFreeRTOSQueue_CreateMutexFailed()
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceGIVE_MUTEX_RECURSIVE_DISABLE))
  #define traceGIVE_MUTEX_RECURSIVE(px)               EvrFreeRTOSQueue_GiveMutexRecursive(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceGIVE_MUTEX_RECURSIVE_FAILED_DISABLE))
  #define traceGIVE_MUTEX_RECURSIVE_FAILED(px)        EvrFreeRTOSQueue_GiveMutexRecursiveFailed(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTAKE_MUTEX_RECURSIVE_DISABLE))
  #define traceTAKE_MUTEX_RECURSIVE(px)               EvrFreeRTOSQueue_TakeMutexRecursive(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTAKE_MUTEX_RECURSIVE_FAILED_DISABLE))
  #define traceTAKE_MUTEX_RECURSIVE_FAILED(px)        EvrFreeRTOSQueue_TakeMutexRecursiveFailed(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_COUNTING_SEMAPHORE_DISABLE))
  #define traceCREATE_COUNTING_SEMAPHORE()            EvrFreeRTOSQueue_CreateCountingSemaphore(xHandle)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceCREATE_COUNTING_SEMAPHORE_FAILED_DISABLE))
  #define traceCREATE_COUNTING_SEMAPHORE_FAILED()     EvrFreeRTOSQueue_CreateCountingSemaphoreFailed()
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_DISABLE))
  #define traceQUEUE_SEND(px)                         EvrFreeRTOSQueue_QueueSend(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_FAILED_DISABLE))
  #define traceQUEUE_SEND_FAILED(px)                  EvrFreeRTOSQueue_QueueSendFailed(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_DISABLE))
  #define traceQUEUE_RECEIVE(px)                      EvrFreeRTOSQueue_QueueReceive(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_DISABLE))
  #define traceQUEUE_PEEK(px)                         EvrFreeRTOSQueue_QueuePeek(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_DISABLE))
  #define traceQUEUE_PEEK_FAILED(px)                  EvrFreeRTOSQueue_QueuePeekFailed(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_FROM_ISR_DISABLE))
  #define traceQUEUE_PEEK_FROM_ISR(px)                EvrFreeRTOSQueue_QueuePeekFromIsr(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_FAILED_DISABLE))
  #define traceQUEUE_RECEIVE_FAILED(px)               EvrFreeRTOSQueue_QueueReceiveFailed(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_FROM_ISR_DISABLE))
  #define traceQUEUE_SEND_FROM_ISR(px)                EvrFreeRTOSQueue_QueueSendFromIsr(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_SEND_FROM_ISR_FAILED_DISABLE))
  #define traceQUEUE_SEND_FROM_ISR_FAILED(px)         EvrFreeRTOSQueue_QueueSendFromIsrFailed(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_FROM_ISR_DISABLE))
  #define traceQUEUE_RECEIVE_FROM_ISR(px)             EvrFreeRTOSQueue_QueueReceiveFromIsr(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_RECEIVE_FROM_ISR_FAILED_DISABLE))
  #define traceQUEUE_RECEIVE_FROM_ISR_FAILED(px)      EvrFreeRTOSQueue_QueueReceiveFromIsrFailed(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_PEEK_FROM_ISR_FAILED_DISABLE))
  #define traceQUEUE_PEEK_FROM_ISR_FAILED(px)         EvrFreeRTOSQueue_QueuePeekFromIsrFailed(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_DELETE_DISABLE))
  #define traceQUEUE_DELETE(px)                       EvrFreeRTOSQueue_QueueDelete(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceQUEUE_REGISTRY_ADD_DISABLE))
  #define traceQUEUE_REGISTRY_ADD(px,pc)              EvrFreeRTOSQueue_QueueRegistryAdd(px,pc)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_QUEUE_RECEIVE_DISABLE))
  #define traceBLOCKING_ON_QUEUE_RECEIVE(px)          EvrFreeRTOSQueue_BlockingOnQueueReceive(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_QUEUE_SEND_DISABLE))
  #define traceBLOCKING_ON_QUEUE_SEND(px)             EvrFreeRTOSQueue_BlockingOnQueueSend(px)
#endif


/* Timers */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_CREATE_DISABLE))
  #define traceTIMER_CREATE(px)                       EvrFreeRTOSTimers_TimerCreate(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_CREATE_FAILED_DISABLE))
  #define traceTIMER_CREATE_FAILED()                  EvrFreeRTOSTimers_TimerCreateFailed()
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_COMMAND_SEND_DISABLE))
  #define traceTIMER_COMMAND_SEND(pxT,xC,xO,xR)       EvrFreeRTOSTimers_TimerCommandSend(pxT,xC,xO,xR)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_COMMAND_RECEIVED_DISABLE))
  #define traceTIMER_COMMAND_RECEIVED(px,xC,xO)       EvrFreeRTOSTimers_TimerCommandReceived(px,xC,xO)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceTIMER_EXPIRED_DISABLE))
  #define traceTIMER_EXPIRED(px)                      EvrFreeRTOSTimers_TimerExpired(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(tracePEND_FUNC_CALL_DISABLE))
  #define tracePEND_FUNC_CALL(px,pv,ul,x)             EvrFreeRTOSTimers_PendFuncCall(px,pv,ul,x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(tracePEND_FUNC_CALL_FROM_ISR_DISABLE))
  #define tracePEND_FUNC_CALL_FROM_ISR(px,pv,ul,x)    EvrFreeRTOSTimers_PendFuncCallFromIsr(px,pv,ul,x)
#endif


/* Event Groups */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CREATE_DISABLE))
  #define traceEVENT_GROUP_CREATE(px)                 EvrFreeRTOSEventGroups_EventGroupCreate(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CREATE_FAILED_DISABLE))
  #define traceEVENT_GROUP_CREATE_FAILED()            EvrFreeRTOSEventGroups_EventGroupCreateFailed()
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SYNC_BLOCK_DISABLE))
  #define traceEVENT_GROUP_SYNC_BLOCK(px,uxS,uxW)     EvrFreeRTOSEventGroups_EventGroupSyncBlock(px,uxS,uxW)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SYNC_END_DISABLE))
  #define traceEVENT_GROUP_SYNC_END(px,uxS,uxW,x)     EvrFreeRTOSEventGroups_EventGroupSyncEnd(px,uxS,uxW,x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_WAIT_BITS_BLOCK_DISABLE))
  #define traceEVENT_GROUP_WAIT_BITS_BLOCK(px,ux)     EvrFreeRTOSEventGroups_EventGroupWaitBitsBlock(px,ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_WAIT_BITS_END_DISABLE))
  #define traceEVENT_GROUP_WAIT_BITS_END(px,ux,x)     EvrFreeRTOSEventGroups_EventGroupWaitBitsEnd(px,ux,x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CLEAR_BITS_DISABLE))
  #define traceEVENT_GROUP_CLEAR_BITS(px,ux)          EvrFreeRTOSEventGroups_EventGroupClearBits(px,ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_DISABLE))
  #define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(px,ux) EvrFreeRTOSEventGroups_EventGroupClearBitsFromIsr(px,ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SET_BITS_DISABLE))
  #define traceEVENT_GROUP_SET_BITS(px,ux)            EvrFreeRTOSEventGroups_EventGroupSetBits(px,ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_SET_BITS_FROM_ISR_DISABLE))
  #define traceEVENT_GROUP_SET_BITS_FROM_ISR(px,ux)   EvrFreeRTOSEventGroups_EventGroupSetBitsFromIsr(px,ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceEVENT_GROUP_DELETE_DISABLE))
  #define traceEVENT_GROUP_DELETE(px)                 EvrFreeRTOSEventGroups_EventGroupDelete(px)
#endif


/* Stream Buffer */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_FAILED_DISABLE))
  #define traceSTREAM_BUFFER_CREATE_FAILED(ux)        EvrFreeRTOSStreamBuf_StreamBufferCreateFailed(ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_STATIC_FAILED_DISABLE))
  #define traceSTREAM_BUFFER_CREATE_STATIC_FAILED(x,ux)  EvrFreeRTOSStreamBuf_StreamBufferCreateStaticFailed(x,ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_DISABLE))
  #define traceSTREAM_BUFFER_CREATE(px,x)             EvrFreeRTOSStreamBuf_StreamBufferCreate(px,x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_DELETE_DISABLE))
  #define traceSTREAM_BUFFER_DELETE(px)               EvrFreeRTOSStreamBuf_StreamBufferDelete(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RESET_DISABLE))
  #define traceSTREAM_BUFFER_RESET(px)                EvrFreeRTOSStreamBuf_StreamBufferReset(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_STREAM_BUFFER_SEND_DISABLE))
  #define traceBLOCKING_ON_STREAM_BUFFER_SEND(px)     EvrFreeRTOSStreamBuf_StreamBufferBlockingOnSend(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_SEND_DISABLE))
  #define traceSTREAM_BUFFER_SEND(px,x)               EvrFreeRTOSStreamBuf_StreamBufferSend(px,x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_SEND_FAILED_DISABLE))
  #define traceSTREAM_BUFFER_SEND_FAILED(px)          EvrFreeRTOSStreamBuf_StreamBufferSendFailed(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_SEND_FROM_ISR_DISABLE))
  #define traceSTREAM_BUFFER_SEND_FROM_ISR(px,x)      EvrFreeRTOSStreamBuf_StreamBufferSendFromIsr(px,x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceBLOCKING_ON_STREAM_BUFFER_RECEIVE_DISABLE))
  #define traceBLOCKING_ON_STREAM_BUFFER_RECEIVE(px)  EvrFreeRTOSStreamBuf_StreamBufferBlockingOnReceive(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RECEIVE_DISABLE))
  #define traceSTREAM_BUFFER_RECEIVE(px,x)            EvrFreeRTOSStreamBuf_StreamBufferReceive(px,x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RECEIVE_FAILED_DISABLE))
  #define traceSTREAM_BUFFER_RECEIVE_FAILED(px)       EvrFreeRTOSStreamBuf_StreamBufferReceiveFailed(px)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_RECEIVE_FROM_ISR_DISABLE))
  #define traceSTREAM_BUFFER_RECEIVE_FROM_ISR(px,x)   EvrFreeRTOSStreamBuf_StreamBufferReceiveFromIsr(px,x)
#endif


/* Heap */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceMALLOC_DISABLE))
  #define traceMALLOC(pv,x)                           EvrFreeRTOSHeap_Malloc(pv,x)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceFREE_DISABLE))
  #define traceFREE(pv,x)                             EvrFreeRTOSHeap_Free(pv,x)
#endif


/* Undefine FreeRTOS object types */
#undef TCB_t
#undef Queue_t
#undef Timer_t
#undef PendedFunction_t
#undef EventGroup_t
#undef StreamBuffer_t

#endif /* FREERTOS_EVR_H_ */
