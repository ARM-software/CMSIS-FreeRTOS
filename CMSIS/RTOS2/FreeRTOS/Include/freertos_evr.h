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
 *      Name:    freertos_evr.h
 *      Purpose: FreeRTOS Event Recorder definitions
 *
 *---------------------------------------------------------------------------*/

#ifndef FREERTOS_EVR_H_
#define FREERTOS_EVR_H_

#include <stdint.h>
#include <stdlib.h>

#include "RTE_Components.h"

#if !defined(RTE_Compiler_EventRecorder) && !defined(RTE_CMSIS_View_EventRecorder)
  /* Disable debug events if Event Recorder is not used */
  #ifndef EVR_FREERTOS_DISABLE
    #define EVR_FREERTOS_DISABLE
  #endif
#endif

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
  \brief  Event sent before moving a task into the Delayed list (Op)
  \param[in]  pxTCB             pointer to running task handle.
*/
extern void EvrFreeRTOSTasks_MovedTaskToDelayedList (TCB_t pxTCB);

/**
  \brief  Event sent before moving a task into the Overflow Delayed list (Op)
  \param[in]  pxTCB             pointer to running task handle.
*/
extern void EvrFreeRTOSTasks_MovedTaskToOverflowDelayedList (TCB_t pxTCB);

/**
  \brief Event sent when function xTaskCreateStatic is called (API).
  \param[in] pxTaskCode         pointer to task function.
  \param[in] pcName             pointer to task name string.
  \param[in] ulStackDepth       stack depth.
  \param[in] pvParameters       pointer to parameter(s) passed to task funtion.
  \param[in] uxPriority         task priority.
  \param[in] puxStackBuffer     pointer to buffer to be used for task stack.
  \param[in] pxTaskBuffer       pointer to buffer to be used for task control block.
*/
extern void EvrFreeRTOSTasks_xTaskCreateStatic (void *pxTaskCode,
                                                const char *pcName,
                                                const uint32_t ulStackDepth,
                                                void *pvParameters,
                                                uint32_t uxPriority,
                                                void *puxStackBuffer,
                                                void *pxTaskBuffer);

/**
  \brief Event sent before function xTaskCreateStatic returns (Op).
  \param[in]  xReturn           pointer to created task handle.
*/
extern void EvrFreeRTOSTasks_xTaskCreateStatic_Return (TCB_t xReturn);

/**
  \brief Event sent when function xTaskCreateStaticAffinitySet is called (API).
  \param[in] pxTaskCode         pointer to task function.
  \param[in] pcName             pointer to task name string.
  \param[in] ulStackDepth       stack depth.
  \param[in] pvParameters       pointer to parameter(s) passed to task funtion.
  \param[in] uxPriority         task priority.
  \param[in] puxStackBuffer     pointer to buffer to be used for task stack.
  \param[in] pxTaskBuffer       pointer to buffer to be used for task control block.
  \param[in] uxCoreAffinityMask task affinity mask.
*/
extern void EvrFreeRTOSTasks_xTaskCreateStaticAffinitySet (void *pxTaskCode,
                                                           const char *pcName,
                                                           const uint32_t ulStackDepth,
                                                           void *pvParameters,
                                                           uint32_t uxPriority,
                                                           void *puxStackBuffer,
                                                           void *pxTaskBuffer,
                                                           uint32_t uxCoreAffinityMask);

/**
  \brief Event sent before function xTaskCreateStaticAffinitySet returns (Op).
  \param[in]  xReturn           pointer to created task handle.
*/
extern void EvrFreeRTOSTasks_xTaskCreateStaticAffinitySet_Return (TCB_t xReturn);

/**
  \brief Event sent when function xTaskCreateRestrictedStatic is called (API).
  \param[in]  pxTaskDefinition  pointer to parameters required to create an MPU protected task.
  \param[in]  pxCreatedTask     pointer to where the created task handle is stored.
*/
extern void EvrFreeRTOSTasks_xTaskCreateRestrictedStatic (const TaskParameters_t *pxTaskDefinition, TaskHandle_t *pxCreatedTask);

/**
  \brief Event sent before function xTaskCreateRestrictedStatic returns (Op).
  \param[in]  xReturn           return status.
*/
extern void EvrFreeRTOSTasks_xTaskCreateRestrictedStatic_Return (uint32_t xReturn);

/**
  \brief Event sent when function xTaskCreateRestrictedStaticAffinitySet is called (API).
  \param[in]  pxTaskDefinition   pointer to parameters required to create an MPU protected task.
  \param[in]  uxCoreAffinityMask affinity mask
  \param[in]  pxCreatedTask      pointer to where the created task handle is stored.
*/
extern void EvrFreeRTOSTasks_xTaskCreateRestrictedStaticAffinitySet (const TaskParameters_t * pxTaskDefinition, UBaseType_t uxCoreAffinityMask, TaskHandle_t *pxCreatedTask);

/**
  \brief Event sent before function xTaskCreateRestrictedStaticAffinitySet returns (Op).
  \param[in]  xReturn           return status.
*/
extern void EvrFreeRTOSTasks_xTaskCreateRestrictedStaticAffinitySet_Return (uint32_t xReturn);

/**
  \brief Event sent when function xTaskCreateRestricted is called (API).
  \param[in]  pxTaskDefinition   pointer to parameters required to create an MPU protected task.
  \param[in]  pxCreatedTask      pointer to where the created task handle is stored.
*/
extern void EvrFreeRTOSTasks_xTaskCreateRestricted (const TaskParameters_t *pxTaskDefinition, TaskHandle_t *pxCreatedTask);

/**
  \brief Event sent before function xTaskCreateRestricted returns (Op).
  \param[in]  xReturn           return status.
*/
extern void EvrFreeRTOSTasks_xTaskCreateRestricted_Return (uint32_t xReturn);

/**
  \brief Event sent when function xTaskCreateRestrictedAffinitySet is called (API).
  \param[in]  pxTaskDefinition   pointer to parameters required to create an MPU protected task.
  \param[in]  uxCoreAffinityMask affinity mask.
  \param[in]  pxCreatedTask      pointer to where the created task handle is stored.
*/
extern void EvrFreeRTOSTasks_xTaskCreateRestrictedAffinitySet (TaskParameters_t *pxTaskDefinition, uint32_t uxCoreAffinityMask, /*TCB_t*/void *pxCreatedTask);

/**
  \brief Event sent before function xTaskCreateRestrictedAffinitySet returns (Op).
  \param[in]  xReturn           return status.
*/
extern void EvrFreeRTOSTasks_xTaskCreateRestrictedAffinitySet_Return (uint32_t xReturn);

/**
  \brief Event sent when function xTaskCreate is called (API).
  \param[in]  pxTaskCode         pointer to task function.
  \param[in]  pcName             pointer to task name string.
  \param[in]  uxStackDepth       stack depth.
  \param[in]  pvParameters       pointer to parameter(s) passed to task funtion.
  \param[in]  uxPriority         task priority.
  \param[in]  pxCreatedTask      pointer to where the created task handle is stored.
*/
extern void EvrFreeRTOSTasks_xTaskCreate (void *pxTaskCode, const char *pcName, uint32_t uxStackDepth, void *pvParameters, uint32_t uxPriority, /*TCB_t*/void *pxCreatedTask);

/**
  \brief Event sent before function xTaskCreate returns (Op).
  \param[in]  xReturn           return status.
*/
extern void EvrFreeRTOSTasks_xTaskCreate_Return (uint32_t xReturn);

/**
  \brief Event sent when function xTaskCreateAffinitySet is called (API).
  \param[in]  pxTaskCode         pointer to task function.
  \param[in]  pcName             pointer to task name string.
  \param[in]  uxStackDepth       stack depth.
  \param[in]  pvParameters       pointer to parameter(s) passed to task funtion.
  \param[in]  uxPriority         task priority.
  \param[in]  uxCoreAffinityMask task affinity mask
  \param[in]  pxCreatedTask      pointer to where the created task handle is stored.
*/
extern void EvrFreeRTOSTasks_xTaskCreateAffinitySet (void *pxTaskCode,
                                                     const char *pcName,
                                                     const uint32_t uxStackDepth,
                                                     void *pvParameters,
                                                     uint32_t uxPriority,
                                                     uint32_t uxCoreAffinityMask,
                                                     void *pxCreatedTask);

/**
  \brief Event sent before function xTaskCreateAffinitySet returns (Op).
  \param[in]  xReturn           return status.
*/
extern void EvrFreeRTOSTasks_xTaskCreateAffinitySet_Return (uint32_t xReturn);

/**
  \brief Event sent when function vTaskDelete is called (API).
  \param[in] xTaskToDelete       handle to the task to delete.
*/
extern void EvrFreeRTOSTasks_vTaskDelete (void *xTaskToDelete);

/**
  \brief Event sent before function vTaskDelete returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskDelete_Return (void);

/**
  \brief Event sent when function xTaskDelayUntil is called (API).
  \param[in]  pxPreviousWakeTime pointer to variable that holds the task last unblock time.
  \param[in]  xTimeIncrement     the cycle time period.
*/
extern void EvrFreeRTOSTasks_xTaskDelayUntil (uint32_t *pxPreviousWakeTime, const uint32_t xTimeIncrement);

/**
  \brief Event sent before function xTaskDelayUntil returns (Op).
  \param[in]  xShouldDelay       return status.
*/
extern void EvrFreeRTOSTasks_xTaskDelayUntil_Return (uint32_t xShouldDelay);

/**
  \brief Event sent when function vTaskDelay is called (API).
  \param[in]  xTicksToDelay      the number of ticks that the calling task should block.
*/
extern void EvrFreeRTOSTasks_vTaskDelay (const uint32_t xTicksToDelay);

/**
  \brief Event sent before function vTaskDelay returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskDelay_Return (void);

/**
  \brief Event sent when function eTaskGetState is called (API).
  \param[in] xTask               task handle.
*/
extern void EvrFreeRTOSTasks_eTaskGetState (TaskHandle_t xTask);

/**
  \brief Event sent before function eTaskGetState returns (Op).
  \param[in]  eReturn            task state.
*/
extern void EvrFreeRTOSTasks_eTaskGetState_Return (eTaskState eReturn);

/**
  \brief Event sent when function uxTaskPriorityGet is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_uxTaskPriorityGet (TaskHandle_t xTask);

/**
  \brief Event sent before function uxTaskPriorityGet returns (Op).
  \param[in]  uxReturn           task priority.
*/
extern void EvrFreeRTOSTasks_uxTaskPriorityGet_Return (uint32_t uxReturn);

/**
  \brief Event sent when function uxTaskPriorityGetFromISR is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_uxTaskPriorityGetFromISR (TaskHandle_t xTask);

/**
  \brief Event sent before function uxTaskPriorityGetFromISR returns (Op).
  \param[in]  uxReturn           task priority.
*/
extern void EvrFreeRTOSTasks_uxTaskPriorityGetFromISR_Return (uint32_t uxReturn);

/**
  \brief Event sent when function uxTaskBasePriorityGet is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_uxTaskBasePriorityGet (TaskHandle_t xTask);

/**
  \brief Event sent before function uxTaskBasePriorityGet returns (Op).
  \param[in]  uxReturn           task priority.
*/
extern void EvrFreeRTOSTasks_uxTaskBasePriorityGet_Return (uint32_t uxReturn);

/**
  \brief Event sent when function uxTaskBasePriorityGetFromISR is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_uxTaskBasePriorityGetFromISR (TaskHandle_t xTask);

/**
  \brief Event sent before function uxTaskBasePriorityGetFromISR returns (Op).
  \param[in]  uxReturn           task priority.
*/
extern void EvrFreeRTOSTasks_uxTaskBasePriorityGetFromISR_Return (uint32_t uxReturn);

/**
  \brief Event sent when function vTaskPrioritySet is called (API).
  \param[in]  xTask              task handle.
  \param[in]  uxNewPriority      task priority to set.
*/
extern void EvrFreeRTOSTasks_vTaskPrioritySet (TaskHandle_t xTask, uint32_t uxNewPriority);

/**
  \brief Event sent before function vTaskPrioritySet returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskPrioritySet_Return (void);

/**
  \brief Event sent when function vTaskCoreAffinitySet is called (API).
  \param[in]  xTask              task handle.
  \param[in]  uxCoreAffinityMask core affinity to set.
*/
extern void EvrFreeRTOSTasks_vTaskCoreAffinitySet (TaskHandle_t xTask, uint32_t uxCoreAffinityMask);

/**
  \brief Event sent before function vTaskCoreAffinitySet returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskCoreAffinitySet_Return (void);

/**
  \brief Event sent when function vTaskCoreAffinityGet is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_vTaskCoreAffinityGet (ConstTaskHandle_t xTask);

/**
  \brief Event sent before function vTaskCoreAffinityGet returns (Op).
  \param[in]  uxCoreAffinityMask core affinity of a task.
*/
extern void EvrFreeRTOSTasks_vTaskCoreAffinityGet_Return (uint32_t uxCoreAffinityMask);

/**
  \brief Event sent when function vTaskPreemptionDisable is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_vTaskPreemptionDisable (TaskHandle_t xTask);

/**
  \brief Event sent before function vTaskPreemptionDisable returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskPreemptionDisable_Return (void);

/**
  \brief Event sent when function vTaskPreemptionEnable is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_vTaskPreemptionEnable (TaskHandle_t xTask);

/**
  \brief Event sent before function vTaskPreemptionEnable returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskPreemptionEnable_Return (void);

/**
  \brief Event sent when function vTaskSuspend is called (API).
  \param[in]  xTaskToSuspend     task handle.
*/
extern void EvrFreeRTOSTasks_vTaskSuspend (TaskHandle_t xTaskToSuspend);

/**
  \brief Event sent before function vTaskSuspend returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskSuspend_Return (void);

/**
  \brief Event sent when function vTaskResume is called (API).
  \param[in]  xTaskToResume      task handle.
*/
extern void EvrFreeRTOSTasks_vTaskResume (TaskHandle_t xTaskToResume);

/**
  \brief Event sent before function vTaskResume returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskResume_Return (void);

/**
  \brief Event sent when function  is called (API).
  \param[in]  xTaskToResume      task handle.
*/
extern void EvrFreeRTOSTasks_xTaskResumeFromISR (TaskHandle_t xTaskToResume);

/**
  \brief Event sent before function  returns (Op).
  \param[in]  xYieldRequired     info if resuming a task results in a context switch
*/
extern void EvrFreeRTOSTasks_xTaskResumeFromISR_Return (uint32_t xYieldRequired);

/**
  \brief Event sent when function vTaskStartScheduler is called (API).
*/
extern void EvrFreeRTOSTasks_vTaskStartScheduler (void);

/**
  \brief Event sent before function vTaskStartScheduler returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskStartScheduler_Return (void);

/**
  \brief Event sent when function vTaskEndScheduler is called (API).
*/
extern void EvrFreeRTOSTasks_vTaskEndScheduler (void);

/**
  \brief Event sent before function vTaskEndScheduler returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskEndScheduler_Return (void);

/**
  \brief Event sent when function vTaskSuspendAll is called (API).
*/
extern void EvrFreeRTOSTasks_vTaskSuspendAll (void);

/**
  \brief Event sent before function vTaskSuspendAll returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskSuspendAll_Return (void);

/**
  \brief Event sent when function xTaskResumeAll is called (API).
*/
extern void EvrFreeRTOSTasks_xTaskResumeAll (void);

/**
  \brief Event sent before function xTaskResumeAll returns (Op).
  \param[in]  xAlreadyYielded    info if resuming the scheduler caused a context switch.
*/
extern void EvrFreeRTOSTasks_xTaskResumeAll_Return (uint32_t xAlreadyYielded);

/**
  \brief Event sent when function xTaskGetTickCount is called (API).
*/
extern void EvrFreeRTOSTasks_xTaskGetTickCount (void);

/**
  \brief Event sent before function xTaskGetTickCount returns (Op).
  \param[in]  xTicks             count of ticks since scheduler was called.
*/
extern void EvrFreeRTOSTasks_xTaskGetTickCount_Return (uint32_t xTicks);

/**
  \brief Event sent when function xTaskGetTickCountFromISR is called (API).
*/
extern void EvrFreeRTOSTasks_xTaskGetTickCountFromISR (void);

/**
  \brief Event sent before function xTaskGetTickCountFromISR returns (Op).
  \param[in]  xTicks             count of ticks since scheduler was called.
*/
extern void EvrFreeRTOSTasks_xTaskGetTickCountFromISR_Return (uint32_t xTicks);

/**
  \brief Event sent when function uxTaskGetNumberOfTasks is called (API).
*/
extern void EvrFreeRTOSTasks_uxTaskGetNumberOfTasks (void);

/**
  \brief Event sent before function uxTaskGetNumberOfTasks returns (Op).
  \param[in]  uxCurrentNumberOfTasks current number of tasks.
*/
extern void EvrFreeRTOSTasks_uxTaskGetNumberOfTasks_Return (uint32_t uxCurrentNumberOfTasks);

/**
  \brief Event sent when function pcTaskGetName is called (API).
  \param[in]  xTaskToQuery       task handle.
*/
extern void EvrFreeRTOSTasks_pcTaskGetName (TaskHandle_t xTaskToQuery);

/**
  \brief Event sent before function pcTaskGetName returns (Op).
  \param[in]  pcTaskName         pointer to task name.
*/
extern void EvrFreeRTOSTasks_pcTaskGetName_Return (char *pcTaskName);

/**
  \brief Event sent when function xTaskGetHandle is called (API).
  \param[in] pcNameToQuery       task name.
*/
extern void EvrFreeRTOSTasks_xTaskGetHandle (const char *pcNameToQuery);

/**
  \brief Event sent before function xTaskGetHandle returns (Op).
  \param[in]  pxTCB             pointer to task task handle.
*/
extern void EvrFreeRTOSTasks_xTaskGetHandle_Return (/*TCB_t*/void *pxTCB);

/**
  \brief Event sent when function xTaskGetStaticBuffers is called (API).
  \param[in]  xTask              task handle.
  \param[in]  ppuxStackBuffer    pointer to location where pointer to task stack buffer is stored.
  \param[in]  ppxTaskBuffer      pointer to location where pointer to task data structure is stored.
*/
extern void EvrFreeRTOSTasks_xTaskGetStaticBuffers (TaskHandle_t xTask, StackType_t **ppuxStackBuffer, /*StaticTask_t** */uint32_t ppxTaskBuffer);

/**
  \brief Event sent before function xTaskGetStaticBuffers returns (Op).
  \param[in]  xReturn           return status.
*/
extern void EvrFreeRTOSTasks_xTaskGetStaticBuffers_Return (uint32_t xReturn);

/**
  \brief Event sent when function uxTaskGetSystemState is called (API).
  \param[in]  pxTaskStatusArray pointer to an array of TaskStatus_t structures.
  \param[in]  uxArraySize       array size.
  \param[in]  pulTotalRunTime   total run time since the target booted.
*/
extern void EvrFreeRTOSTasks_uxTaskGetSystemState (TaskStatus_t *pxTaskStatusArray, const uint32_t uxArraySize, uint32_t *pulTotalRunTime);

/**
  \brief Event sent before function uxTaskGetSystemState returns (Op).
  \param[in]  uxTask            the number of TaskStatus_t structures populated.
*/
extern void EvrFreeRTOSTasks_uxTaskGetSystemState_Return (uint32_t uxTask);

/**
  \brief Event sent when function xTaskGetIdleTaskHandle is called (API).
*/
extern void EvrFreeRTOSTasks_xTaskGetIdleTaskHandle (void);

/**
  \brief Event sent before function xTaskGetIdleTaskHandle returns (Op).
  \param[in]  xTask             task handle.
*/
extern void EvrFreeRTOSTasks_xTaskGetIdleTaskHandle_Return (TaskHandle_t xTask);

/**
  \brief Event sent when function xTaskGetIdleTaskHandleForCore is called (API).
  \param[in]  xCoreID           core id.
*/
extern void EvrFreeRTOSTasks_xTaskGetIdleTaskHandleForCore (uint32_t xCoreID);

/**
  \brief Event sent before function xTaskGetIdleTaskHandleForCore returns (Op).
  \param[in]  xTask             task handle.
*/
extern void EvrFreeRTOSTasks_xTaskGetIdleTaskHandleForCore_Return (TaskHandle_t xTask);

/**
  \brief Event sent when function vTaskStepTick is called (API).
  \param[in]  xTicksToJump      number of ticks to add to the tick count.
*/
extern void EvrFreeRTOSTasks_vTaskStepTick (uint32_t xTicksToJump);

/**
  \brief Event sent before function vTaskStepTick returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskStepTick_Return (void);

/**
  \brief Event sent when function xTaskCatchUpTicks is called (API).
  \param[in]  xTicksToCatchUp   number of tick interrupts that have been missed.
*/
extern void EvrFreeRTOSTasks_xTaskCatchUpTicks (uint32_t xTicksToCatchUp);

/**
  \brief Event sent before function xTaskCatchUpTicks returns (Op).
  \param[in]  xYieldOccurred    info if any task was unblocked.
*/
extern void EvrFreeRTOSTasks_xTaskCatchUpTicks_Return (uint32_t xYieldOccurred);

/**
  \brief Event sent when function  is called (API).
  \param[in]  xTask             task handle.
*/
extern void EvrFreeRTOSTasks_xTaskAbortDelay (TaskHandle_t xTask);

/**
  \brief Event sent before function  returns (Op).
  \param[in]  xReturn           execution status.
*/
extern void EvrFreeRTOSTasks_xTaskAbortDelay_Return (uint32_t xReturn);

/**
  \brief Event sent when function xTaskIncrementTick is called (API).
*/
extern void EvrFreeRTOSTasks_xTaskIncrementTick (void);

/**
  \brief Event sent before function xTaskIncrementTick returns (Op).
  \param[in]  xSwitchRequired   info if task switch is required.
*/
extern void EvrFreeRTOSTasks_xTaskIncrementTick_Return (uint32_t xSwitchRequired);

/**
  \brief Event sent when function  is called (API).
  \param[in]  xTask             task handle.
  \param[in]  pxHookFunction    hook function used by the task xTask.
*/
extern void EvrFreeRTOSTasks_vTaskSetApplicationTaskTag (TaskHandle_t xTask, TaskHookFunction_t pxHookFunction);

/**
  \brief Event sent before function  returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskSetApplicationTaskTag_Return (void);

/**
  \brief Event sent when function xTaskGetApplicationTaskTag is called (API).
  \param[in]  xTask             task handle.
*/
extern void EvrFreeRTOSTasks_xTaskGetApplicationTaskTag (TaskHandle_t xTask);

/**
  \brief Event sent before function xTaskGetApplicationTaskTag returns (Op).
  \param[in]  xReturn           hook function used by the task xTask.
*/
extern void EvrFreeRTOSTasks_xTaskGetApplicationTaskTag_Return (TaskHookFunction_t xReturn);

/**
  \brief Event sent when function xTaskGetApplicationTaskTagFromISR is called (API).
  \param[in]  xTask             task handle.
*/
extern void EvrFreeRTOSTasks_xTaskGetApplicationTaskTagFromISR (TaskHandle_t xTask);

/**
  \brief Event sent before function xTaskGetApplicationTaskTagFromISR returns (Op).
  \param[in]  xReturn           hook function used by the task xTask.
*/
extern void EvrFreeRTOSTasks_xTaskGetApplicationTaskTagFromISR_Return (TaskHookFunction_t xReturn);

/**
  \brief Event sent when function xTaskCallApplicationTaskHook is called (API).
  \param[in]  xTask             task handle.
  \param[in]  pvParameter       pointer to parameters passed to the hook function.
*/
extern void EvrFreeRTOSTasks_xTaskCallApplicationTaskHook (TaskHandle_t xTask, void *pvParameter);

/**
  \brief Event sent before function xTaskCallApplicationTaskHook returns (Op).
  \param[in]  xReturn           hook function execution status.
*/
extern void EvrFreeRTOSTasks_xTaskCallApplicationTaskHook_Return (uint32_t xReturn);

/**
  \brief Event sent when function  is called (API).
*/
extern void EvrFreeRTOSTasks_vTaskSwitchContext (void);

/**
  \brief Event sent before function  returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskSwitchContext_Return (void);

/**
  \brief Event sent when function vTaskPlaceOnEventList is called (API).
  \param[in]  pxEventList       pointer to the blocked task list waiting for event.
  \param[in]  xTicksToWait      amount of time to wait for an event.
*/
extern void EvrFreeRTOSTasks_vTaskPlaceOnEventList (List_t *pxEventList, const TickType_t xTicksToWait);

/**
  \brief Event sent before function vTaskPlaceOnEventList returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskPlaceOnEventList_Return (void);

/**
  \brief Event sent when function vTaskPlaceOnUnorderedEventList is called (API).
  \param[in]  pxEventList       pointer to the blocked task list waiting for event.
  \param[in]  xItemValue        value to use when event list is not ordered by priority.
  \param[in]  xTicksToWait      amount of time to wait for an event.
*/
extern void EvrFreeRTOSTasks_vTaskPlaceOnUnorderedEventList (List_t *pxEventList, const TickType_t xItemValue, const TickType_t xTicksToWait);

/**
  \brief Event sent before function vTaskPlaceOnUnorderedEventList returns (Op). 
*/
extern void EvrFreeRTOSTasks_vTaskPlaceOnUnorderedEventList_Return (void);

/**
  \brief Event sent when function vTaskPlaceOnEventListRestricted is called (API).
  \param[in]  pxEventList       pointer to the blocked task list waiting for event.
  \param[in]  xTicksToWait      amount of time to wait for an event.
  \param[in]  xWaitIndefinitely whether the task should block indefinitely.
*/
extern void EvrFreeRTOSTasks_vTaskPlaceOnEventListRestricted (List_t *pxEventList, TickType_t xTicksToWait, const uint32_t xWaitIndefinitely);

/**
  \brief Event sent before function vTaskPlaceOnEventListRestricted returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskPlaceOnEventListRestricted_Return (void);

/**
  \brief Event sent when function xTaskRemoveFromEventList is called (API).
  \param[in]  pxEventList       pointer to the event list.
*/
extern void EvrFreeRTOSTasks_xTaskRemoveFromEventList (const List_t *pxEventList);

/**
  \brief Event sent before function xTaskRemoveFromEventList returns (Op).
  \param[in]  xReturn           execution status
*/
extern void EvrFreeRTOSTasks_xTaskRemoveFromEventList_Return (uint32_t xReturn);

/**
  \brief Event sent when function vTaskRemoveFromUnorderedEventList is called (API).
  \param[in]  pxEventListItem   pointer to the event list.
  \param[in]  xItemValue        event list item value.
*/
extern void EvrFreeRTOSTasks_vTaskRemoveFromUnorderedEventList (ListItem_t *pxEventListItem, const TickType_t xItemValue);

/**
  \brief Event sent before function vTaskRemoveFromUnorderedEventList returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskRemoveFromUnorderedEventList_Return (void);

/**
  \brief Event sent when function vTaskSetTimeOutState is called (API).
  \param[in]  pxTimeOut         pointer to the timeout object.
*/
extern void EvrFreeRTOSTasks_vTaskSetTimeOutState (TimeOut_t *pxTimeOut);

/**
  \brief Event sent before function vTaskSetTimeOutState returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskSetTimeOutState_Return (void);

/**
  \brief Event sent when function vTaskInternalSetTimeOutState is called (API).
  \param[in]  pxTimeOut         pointer to the timeout object.
*/
extern void EvrFreeRTOSTasks_vTaskInternalSetTimeOutState (TimeOut_t *pxTimeOut);

/**
  \brief Event sent before function vTaskInternalSetTimeOutState returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskInternalSetTimeOutState_Return (void);

/**
  \brief Event sent when function xTaskCheckForTimeOut is called (API).
  \param[in]  pxTimeOut         pointer to the timeout object.
  \param[in]  pxTicksToWait     pointer to object holding number of ticks to check for timeout.
*/
extern void EvrFreeRTOSTasks_xTaskCheckForTimeOut (TimeOut_t *pxTimeOut, TickType_t *pxTicksToWait);

/**
  \brief Event sent before function xTaskCheckForTimeOut returns (Op).
  \param[in]  xReturn           execution status.
*/
extern void EvrFreeRTOSTasks_xTaskCheckForTimeOut_Return (uint32_t xReturn);

/**
  \brief Event sent when function vTaskMissedYield is called (API).
*/
extern void EvrFreeRTOSTasks_vTaskMissedYield (void);

/**
  \brief Event sent before function vTaskMissedYield returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskMissedYield_Return (void);

/**
  \brief Event sent when function  is called (API).
  \param[in]  xTask             task handle.
*/
extern void EvrFreeRTOSTasks_uxTaskGetTaskNumber (TaskHandle_t xTask);

/**
  \brief Event sent before function  returns (Op).
  \param[in]  uxReturn          number assigned to the task
*/
extern void EvrFreeRTOSTasks_uxTaskGetTaskNumber_Return (uint32_t uxReturn);

/**
  \brief Event sent when function vTaskSetTaskNumber is called (API).
  \param[in]  xTask             task handle.
  \param[in]  uxHandle          the number to assign to the task.
*/
extern void EvrFreeRTOSTasks_vTaskSetTaskNumber (TaskHandle_t xTask, const uint32_t uxHandle);

/**
  \brief Event sent before function vTaskSetTaskNumber returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskSetTaskNumber_Return (void);

/**
  \brief Event sent when function eTaskConfirmSleepModeStatus is called (API).
*/
extern void EvrFreeRTOSTasks_eTaskConfirmSleepModeStatus (void);

/**
  \brief Event sent before function eTaskConfirmSleepModeStatus returns (Op).
  \param[in]  eReturn           sleep mode status.
*/
extern void EvrFreeRTOSTasks_eTaskConfirmSleepModeStatus_Return (eSleepModeStatus eReturn);

/**
  \brief Event sent when function vTaskSetThreadLocalStoragePointer is called (API).
  \param[in]  xTaskToSet        task handle.
  \param[in]  xIndex            array index.
  \param[in]  pvValue           value to set.
*/
extern void EvrFreeRTOSTasks_vTaskSetThreadLocalStoragePointer (TaskHandle_t xTaskToSet, uint32_t xIndex, void * pvValue);

/**
  \brief Event sent before function vTaskSetThreadLocalStoragePointer returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskSetThreadLocalStoragePointer_Return (void);

/**
  \brief Event sent when function pvTaskGetThreadLocalStoragePointer is called (API).
  \param[in]  xTaskToQuery      task handle.
  \param[in]  xIndex            array index.
*/
extern void EvrFreeRTOSTasks_pvTaskGetThreadLocalStoragePointer (TaskHandle_t xTaskToQuery, uint32_t xIndex);

/**
  \brief Event sent before function pvTaskGetThreadLocalStoragePointer returns (Op).
  \param[in]  pvReturn          retrieved value.
*/
extern void EvrFreeRTOSTasks_pvTaskGetThreadLocalStoragePointer_Return (void *pvReturn);

/**
  \brief Event sent when function vTaskAllocateMPURegions is called (API).
  \param[in]  xTaskToModify     task handle.
  \param[in]  pxRegions         pointer to a structure that contains memory region definitions.
*/
extern void EvrFreeRTOSTasks_vTaskAllocateMPURegions (TaskHandle_t xTaskToModify, const MemoryRegion_t *pxRegions);

/**
  \brief Event sent before function vTaskAllocateMPURegions returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskAllocateMPURegions_Return (void);

/**
  \brief Event sent when function vTaskGetInfo is called (API).
  \param[in]  xTask              task handle.
  \param[in]  pxTaskStatus       pointer to the structure that will be filled with info.
  \param[in]  xGetFreeStackSpace parameter that allows the high water mark checking to be skipped.
  \param[in]  eState             parameter that allows the state information to be omitted.
*/
extern void EvrFreeRTOSTasks_vTaskGetInfo (TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, uint32_t xGetFreeStackSpace, eTaskState eState);

/**
  \brief Event sent before function vTaskGetInfo returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskGetInfo_Return (void);

/**
  \brief Event sent when function uxTaskGetStackHighWaterMark2 is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark2 (TaskHandle_t xTask);

/**
  \brief Event sent before function uxTaskGetStackHighWaterMark2 returns (Op).
  \param[in]  uxReturn           the smallest amount (in words) of free stack space there has been.
*/
extern void EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark2_Return (uint32_t uxReturn);

/**
  \brief Event sent when function uxTaskGetStackHighWaterMark is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark (TaskHandle_t xTask);

/**
  \brief Event sent before function uxTaskGetStackHighWaterMark returns (Op).
  \param[in]  uxReturn           the smallest amount (in words) of free stack space there has been.
*/
extern void EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark_Return (uint32_t uxReturn);

/**
  \brief Event sent when function xTaskGetCurrentTaskHandle is called (API).
*/
extern void EvrFreeRTOSTasks_xTaskGetCurrentTaskHandle (void);

/**
  \brief Event sent before function xTaskGetCurrentTaskHandle returns (Op).
  \param[in]  xReturn            task handle.
*/
extern void EvrFreeRTOSTasks_xTaskGetCurrentTaskHandle_Return (TaskHandle_t xReturn);

/**
  \brief Event sent when function xTaskGetCurrentTaskHandleForCore is called (API).
  \param[in]  xCoreID            processor core ID.
*/
extern void EvrFreeRTOSTasks_xTaskGetCurrentTaskHandleForCore (uint32_t xCoreID);

/**
  \brief Event sent before function xTaskGetCurrentTaskHandleForCore returns (Op).
  \param[in]  xReturn            task handle.
*/
extern void EvrFreeRTOSTasks_xTaskGetCurrentTaskHandleForCore_Return (TaskHandle_t xReturn);

/**
  \brief Event sent when function xTaskGetSchedulerState is called (API).
*/
extern void EvrFreeRTOSTasks_xTaskGetSchedulerState (void);

/**
  \brief Event sent before function xTaskGetSchedulerState returns (Op).
  \param[in]  xReturn            scheduler state.
*/
extern void EvrFreeRTOSTasks_xTaskGetSchedulerState_Return (uint32_t xReturn);

/**
  \brief Event sent when function xTaskPriorityInherit is called (API).
  \param[in]  pxMutexHolder      task handle.
*/
extern void EvrFreeRTOSTasks_xTaskPriorityInherit (TaskHandle_t pxMutexHolder);

/**
  \brief Event sent before function xTaskPriorityInherit returns (Op).
  \param[in]  xReturn            execution status.
*/
extern void EvrFreeRTOSTasks_xTaskPriorityInherit_Return (uint32_t xReturn);

/**
  \brief Event sent when function xTaskPriorityDisinherit is called (API).
  \param[in]  pxMutexHolder      task handle.
*/
extern void EvrFreeRTOSTasks_xTaskPriorityDisinherit (TaskHandle_t pxMutexHolder);

/**
  \brief Event sent before function xTaskPriorityDisinherit returns (Op).
  \param[in]  xReturn            execution status.
*/
extern void EvrFreeRTOSTasks_xTaskPriorityDisinherit_Return (uint32_t xReturn);

/**
  \brief Event sent when function vTaskPriorityDisinheritAfterTimeout is called (API).
  \param[in]  pxMutexHolder                task handle.
  \param[in]  uxHighestPriorityWaitingTask the priority of the highest priority task that is waiting for mutex.
*/
extern void EvrFreeRTOSTasks_vTaskPriorityDisinheritAfterTimeout (TaskHandle_t pxMutexHolder, uint32_t uxHighestPriorityWaitingTask);

/**
  \brief Event sent before function vTaskPriorityDisinheritAfterTimeout returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskPriorityDisinheritAfterTimeout_Return (void);

/**
  \brief Event sent when function vTaskYieldWithinAPI is called (API).
*/
extern void EvrFreeRTOSTasks_vTaskYieldWithinAPI (void);

/**
  \brief Event sent before function vTaskYieldWithinAPI returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskYieldWithinAPI_Return (void);

/**
  \brief Event sent when function vTaskEnterCritical is called (API).
*/
extern void EvrFreeRTOSTasks_vTaskEnterCritical (void);

/**
  \brief Event sent before function vTaskEnterCritical returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskEnterCritical_Return (void);

/**
  \brief Event sent when function vTaskEnterCriticalFromISR is called (API).
*/
extern void EvrFreeRTOSTasks_vTaskEnterCriticalFromISR (void);

/**
  \brief Event sent before function vTaskEnterCriticalFromISR returns (Op).
  \param[in]  uxSavedInterruptStatus  saved interrupt status.
*/
extern void EvrFreeRTOSTasks_vTaskEnterCriticalFromISR_Return (uint32_t uxSavedInterruptStatus);

/**
  \brief Event sent when function vTaskExitCritical is called (API).
*/
extern void EvrFreeRTOSTasks_vTaskExitCritical (void);

/**
  \brief Event sent before function vTaskExitCritical returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskExitCritical_Return (void);

/**
  \brief Event sent when function vTaskExitCriticalFromISR is called (API).
  \param[in]  uxSavedInterruptStatus  saved interrupt status.
*/
extern void EvrFreeRTOSTasks_vTaskExitCriticalFromISR (uint32_t uxSavedInterruptStatus);

/**
  \brief Event sent before function vTaskExitCriticalFromISR returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskExitCriticalFromISR_Return (void);

/**
  \brief Event sent when function vTaskListTasks is called (API).
  \param[in]  pcWriteBuffer      pointer to buffer.
  \param[in]  uxBufferLength     length of the pcWriteBuffer.
*/
extern void EvrFreeRTOSTasks_vTaskListTasks (char *pcWriteBuffer, size_t uxBufferLength);

/**
  \brief Event sent before function vTaskListTasks returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskListTasks_Return (void);

/**
  \brief Event sent when function vTaskGetRunTimeStatistics is called (API).
  \param[in]  pcWriteBuffer      pointer to buffer.
  \param[in]  uxBufferLength     length of the pcWriteBuffer.
*/
extern void EvrFreeRTOSTasks_vTaskGetRunTimeStatistics (char *pcWriteBuffer, size_t uxBufferLength);

/**
  \brief Event sent before function vTaskGetRunTimeStatistics returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskGetRunTimeStatistics_Return (void);

/**
  \brief Event sent when function uxTaskResetEventItemValue is called (API).
*/
extern void EvrFreeRTOSTasks_uxTaskResetEventItemValue (void);

/**
  \brief Event sent before function uxTaskResetEventItemValue returns (Op).
  \param[in]  uxReturn           event list item value.
*/
extern void EvrFreeRTOSTasks_uxTaskResetEventItemValue_Return (TickType_t uxReturn);

/**
  \brief Event sent when function pvTaskIncrementMutexHeldCount is called (API).
*/
extern void EvrFreeRTOSTasks_pvTaskIncrementMutexHeldCount (void);

/**
  \brief Event sent before function pvTaskIncrementMutexHeldCount returns (Op).
  \param[in]  pxTCB              running task handle.
*/
extern void EvrFreeRTOSTasks_pvTaskIncrementMutexHeldCount_Return (/*TCB_t*/void *pxTCB);

/**
  \brief Event sent when function ulTaskGenericNotifyTake is called (API).
  \param[in]  uxIndexToWaitOn    index within the tasks array of notification values.
  \param[in]  xClearCountOnExit  indication whether to clear notification value.
  \param[in]  xTicksToWait       amount of time to wait.
*/
extern void EvrFreeRTOSTasks_ulTaskGenericNotifyTake (UBaseType_t uxIndexToWaitOn, BaseType_t xClearCountOnExit, TickType_t xTicksToWait);

/**
  \brief Event sent before function ulTaskGenericNotifyTake returns (Op).
  \param[in]  ulReturn           task notification value before modification.
*/
extern void EvrFreeRTOSTasks_ulTaskGenericNotifyTake_Return (uint32_t ulReturn);

/**
  \brief Event sent when function xTaskGenericNotifyWait is called (API).
  \param[in]  uxIndexToWaitOn      index within the tasks array of notification values.
  \param[in]  ulBitsToClearOnEntry bits that shall be cleared before reading notification value.
  \param[in]  ulBitsToClearOnExit  bits that shall be cleared before returning.
  \param[in]  pulNotificationValue pointer to variable where notification value will be stored.
  \param[in]  xTicksToWait         amount of time to wait.
*/
extern void EvrFreeRTOSTasks_xTaskGenericNotifyWait (UBaseType_t uxIndexToWaitOn,
                                                     uint32_t    ulBitsToClearOnEntry,
                                                     uint32_t    ulBitsToClearOnExit,
                                                     uint32_t   *pulNotificationValue,
                                                     TickType_t  xTicksToWait);

/**
  \brief Event sent before function xTaskGenericNotifyWait returns (Op).
  \param[in]  xReturn            notification receive status.
*/
extern void EvrFreeRTOSTasks_xTaskGenericNotifyWait_Return (BaseType_t xReturn);

/**
  \brief Event sent when function xTaskGenericNotify is called (API).
  \param[in]  xTaskToNotify                task handle.
  \param[in]  uxIndexToNotify              index within the tasks array of notification values.
  \param[in]  ulValue                      notification data value.
  \param[in]  eAction                      specifies how is the task notification value updated.
  \param[in]  pulPreviousNotificationValue pointer to variable where notification value will be stored.
*/
extern void EvrFreeRTOSTasks_xTaskGenericNotify (TaskHandle_t  xTaskToNotify,
                                                 UBaseType_t   uxIndexToNotify,
                                                 uint32_t      ulValue,
                                                 eNotifyAction eAction,
                                                 uint32_t     *pulPreviousNotificationValue);

/**
  \brief Event sent before function xTaskGenericNotify returns (Op).
  \param[in]  xReturn            context dependent return value.
*/
extern void EvrFreeRTOSTasks_xTaskGenericNotify_Return (BaseType_t xReturn);

/**
  \brief Event sent when function xTaskGenericNotifyFromISR is called (API).
  \param[in]  xTaskToNotify                task handle.
  \param[in]  uxIndexToNotify              index within the tasks array of notification values.
  \param[in]  ulValue                      notification data value.
  \param[in]  eAction                      specifies how is the task notification value updated.
  \param[in]  pulPreviousNotificationValue pointer to variable where notification value will be stored.
  \param[in]  pxHigherPriorityTaskWoken    pointer to variable where information if any task was woken will be stored.
*/
extern void EvrFreeRTOSTasks_xTaskGenericNotifyFromISR (TaskHandle_t  xTaskToNotify,
                                                        UBaseType_t   uxIndexToNotify,
                                                        uint32_t      ulValue,
                                                        eNotifyAction eAction,
                                                        uint32_t     *pulPreviousNotificationValue,
                                                        BaseType_t   *pxHigherPriorityTaskWoken);

/**
  \brief Event sent before function xTaskGenericNotifyFromISR returns (Op).
  \param[in]  xReturn            context dependent return value.
*/
extern void EvrFreeRTOSTasks_xTaskGenericNotifyFromISR_Return (BaseType_t xReturn);

/**
  \brief Event sent when function vTaskGenericNotifyGiveFromISR is called (API).
  \param[in]  xTaskToNotify             task handle.
  \param[in]  uxIndexToNotify           index within the tasks array of notification values.
  \param[in]  pxHigherPriorityTaskWoken pointer to variable where information if any task was woken will be stored.
*/
extern void EvrFreeRTOSTasks_vTaskGenericNotifyGiveFromISR (TaskHandle_t xTaskToNotify, UBaseType_t uxIndexToNotify, BaseType_t *pxHigherPriorityTaskWoken);

/**
  \brief Event sent before function vTaskGenericNotifyGiveFromISR returns (Op).
*/
extern void EvrFreeRTOSTasks_vTaskGenericNotifyGiveFromISR_Return (void);

/**
  \brief Event sent when function xTaskGenericNotifyStateClear is called (API).
  \param[in]  xTask              task handle.
  \param[in]  uxIndexToClear     index within the tasks array of notification values.
*/
extern void EvrFreeRTOSTasks_xTaskGenericNotifyStateClear (TaskHandle_t xTask, UBaseType_t uxIndexToClear);

/**
  \brief Event sent before function xTaskGenericNotifyStateClear returns (Op).
  \param[in]  xReturn            execution status.
*/
extern void EvrFreeRTOSTasks_xTaskGenericNotifyStateClear_Return (BaseType_t xReturn);

/**
  \brief Event sent when function ulTaskGenericNotifyValueClear is called (API).
  \param[in]  xTask              task handle.
  \param[in]  uxIndexToClear     index within the tasks array of notification values.
  \param[in]  ulBitsToClear      bits to clear in notification value.
*/
extern void EvrFreeRTOSTasks_ulTaskGenericNotifyValueClear (TaskHandle_t xTask, UBaseType_t uxIndexToClear, uint32_t ulBitsToClear);

/**
  \brief Event sent before function ulTaskGenericNotifyValueClear returns (Op).
  \param[in]  ulReturn           notification value before clearing.
*/
extern void EvrFreeRTOSTasks_ulTaskGenericNotifyValueClear_Return (uint32_t ulReturn);

/**
  \brief Event sent when function ulTaskGetRunTimeCounter is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_ulTaskGetRunTimeCounter (const TaskHandle_t xTask);

/**
  \brief Event sent before function ulTaskGetRunTimeCounter returns (Op).
  \param[in]  ulRunTimeCounter   total run time of the given task.
*/
extern void EvrFreeRTOSTasks_ulTaskGetRunTimeCounter_Return (uint32_t ulRunTimeCounter);

/**
  \brief Event sent when function ulTaskGetRunTimePercent is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_ulTaskGetRunTimePercent (const TaskHandle_t xTask);

/**
  \brief Event sent before function ulTaskGetRunTimePercent returns (Op).
  \param[in]  ulReturn           the percentage of the total run time consumed by the given task.
*/
extern void EvrFreeRTOSTasks_ulTaskGetRunTimePercent_Return (uint32_t ulReturn);

/**
  \brief Event sent when function ulTaskGetIdleRunTimeCounter is called (API).
*/
extern void EvrFreeRTOSTasks_ulTaskGetIdleRunTimeCounter (void);

/**
  \brief Event sent before function ulTaskGetIdleRunTimeCounter returns (Op).
  \param[in]  ulReturn           total run time of the idle task.
*/
extern void EvrFreeRTOSTasks_ulTaskGetIdleRunTimeCounter_Return (uint32_t ulReturn);

/**
  \brief Event sent when function ulTaskGetIdleRunTimePercent is called (API).
*/
extern void EvrFreeRTOSTasks_ulTaskGetIdleRunTimePercent (void);

/**
  \brief Event sent before function ulTaskGetIdleRunTimePercent returns (Op).
  \param[in]  ulReturn           the percentage of the total run time consumed by the idle task.
*/
extern void EvrFreeRTOSTasks_ulTaskGetIdleRunTimePercent_Return (uint32_t ulReturn);

/**
  \brief Event sent when function xTaskGetMPUSettings is called (API).
  \param[in]  xTask              task handle.
*/
extern void EvrFreeRTOSTasks_xTaskGetMPUSettings (TaskHandle_t xTask);

/**
  \brief Event sent before function xTaskGetMPUSettings returns (Op).
  \param[in]  xMPUSettings       pointer to MPU settings associated with a task.
*/
extern void EvrFreeRTOSTasks_xTaskGetMPUSettings_Return (void *xMPUSettings);

/* --------------------------------------------------------------------------------------------- */

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
  \param[in]  xFunctionToPend   pointer to callback function
  \param[in]  pvParameter1      function parameter 1.
  \param[in]  ulParameter2      function parameter 2.
  \param[in]  xReturn           return value.
*/
extern void EvrFreeRTOSTimers_PendFuncCall (PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, uint32_t xReturn);

/**
  \brief  Event on pass of the function execution to the timer service task from the ISR (Op)
  \param[in]  xFunctionToPend   pointer to callback function
  \param[in]  pvParameter1      function parameter 1.
  \param[in]  ulParameter2      function parameter 2.
  \param[in]  xReturn           return value.
*/
extern void EvrFreeRTOSTimers_PendFuncCallFromIsr (PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, uint32_t xReturn);

/**
  \brief Event sent when function xTimerCreateTimerTask is called (API).
*/
extern void EvrFreeRTOSTimers_xTimerCreateTimerTask (void);

/**
  \brief Event sent before function xTimerCreateTimerTask returns (Op).
  \param[in]  xReturn           execution status.
*/
extern void EvrFreeRTOSTimers_xTimerCreateTimerTask_Return (BaseType_t xReturn);

/**
  \brief Event sent when function xTimerCreate is called (API).
  \param[in]  pcTimerName         human readable timer name.
  \param[in]  xTimerPeriodInTicks the period of the timer in ticks.
  \param[in]  xAutoReload         auto reload flag.
  \param[in]  pvTimerID           identifier assigned to the timer being created.
  \param[in]  pxCallbackFunction  function to call when the timer expired.
*/
extern void EvrFreeRTOSTimers_xTimerCreate (const char *pcTimerName,
                                            const TickType_t xTimerPeriodInTicks,
                                            const BaseType_t xAutoReload,
                                            void *pvTimerID,
                                            TimerCallbackFunction_t pxCallbackFunction);

/**
  \brief Event sent before function xTimerCreate returns (Op).
  \param[in]  pxNewTimer        Timer object handle.
*/
extern void EvrFreeRTOSTimers_xTimerCreate_Return (TimerHandle_t pxNewTimer);

/**
  \brief Event sent when function xTimerCreateStatic is called (API).
  \param[in]  pcTimerName         human readable timer name.
  \param[in]  xTimerPeriodInTicks the period of the timer in ticks.
  \param[in]  xAutoReload         auto reload flag.
  \param[in]  pvTimerID           identifier assigned to the timer being created.
  \param[in]  pxCallbackFunction  function to call when the timer expired.
  \param[in]  pxTimerBuffer       pointer to Timer object memory buffer.
*/
extern void EvrFreeRTOSTimers_xTimerCreateStatic (const char *pcTimerName,
                                                  const TickType_t xTimerPeriodInTicks,
                                                  const BaseType_t xAutoReload,
                                                  void *pvTimerID,
                                                  TimerCallbackFunction_t pxCallbackFunction,
                                                  StaticTimer_t *pxTimerBuffer);

/**
  \brief Event sent before function xTimerCreateStatic returns (Op).
  \param[in]  pxNewTimer        Timer object handle.
*/
extern void EvrFreeRTOSTimers_xTimerCreateStatic_Return (TimerHandle_t pxNewTimer);

/**
  \brief Event sent when function xTimerGenericCommandFromTask is called (API).
  \param[in]  xTimer                    Timer object handle.
  \param[in]  xCommandID                timer command identification.
  \param[in]  xOptionalValue            optional command value.
  \param[in]  pxHigherPriorityTaskWoken pointer to variable where information if any task was woken will be stored.
  \param[in]  xTicksToWait              number of ticks to wait.
*/
extern void EvrFreeRTOSTimers_xTimerGenericCommandFromTask (TimerHandle_t xTimer,
                                                            const BaseType_t xCommandID,
                                                            const TickType_t xOptionalValue,
                                                            BaseType_t *pxHigherPriorityTaskWoken,
                                                            const TickType_t xTicksToWait);

/**
  \brief Event sent before function xTimerGenericCommandFromTask returns (Op).
  \param[in]  xReturn           execution status.
*/
extern void EvrFreeRTOSTimers_xTimerGenericCommandFromTask_Return (BaseType_t xReturn);

/**
  \brief Event sent when function xTimerGenericCommandFromISR is called (API).
  \param[in]  xTimer                    Timer object handle.
  \param[in]  xCommandID                timer command identification.
  \param[in]  xOptionalValue            optional command value.
  \param[in]  pxHigherPriorityTaskWoken pointer to variable where information if any task was woken will be stored.
  \param[in]  xTicksToWait              number of ticks to wait.
*/
extern void EvrFreeRTOSTimers_xTimerGenericCommandFromISR (TimerHandle_t xTimer,
                                                           const BaseType_t xCommandID,
                                                           const TickType_t xOptionalValue,
                                                           BaseType_t *pxHigherPriorityTaskWoken,
                                                           const TickType_t xTicksToWait);

/**
  \brief Event sent before function xTimerGenericCommandFromISR returns (Op).
  \param[in]  xReturn           execution status.
*/
extern void EvrFreeRTOSTimers_xTimerGenericCommandFromISR_Return (BaseType_t xReturn);

/**
  \brief Event sent when function xTimerGetTimerDaemonTaskHandle is called (API).
*/
extern void EvrFreeRTOSTimers_xTimerGetTimerDaemonTaskHandle (void);

/**
  \brief Event sent before function xTimerGetTimerDaemonTaskHandle returns (Op).
  \param[in]  xTimerTaskHandle  Timer task handle.
*/
extern void EvrFreeRTOSTimers_xTimerGetTimerDaemonTaskHandle_Return (TaskHandle_t xTimerTaskHandle);

/**
  \brief Event sent when function xTimerGetPeriod is called (API).
  \param[in]  xTimer            Timer object handle.
*/
extern void EvrFreeRTOSTimers_xTimerGetPeriod (TimerHandle_t xTimer);

/**
  \brief Event sent before function xTimerGetPeriod returns (Op).
  \param[in]  xReturn           timer period in ticks.
*/
extern void EvrFreeRTOSTimers_xTimerGetPeriod_Return (uint32_t xReturn);

/**
  \brief Event sent when function vTimerSetReloadMode is called (API).
  \param[in]  xTimer            Timer object handle.
  \param[in]  xAutoReload       auto reload mode.
*/
extern void EvrFreeRTOSTimers_vTimerSetReloadMode (TimerHandle_t xTimer, const BaseType_t xAutoReload);

/**
  \brief Event sent before function vTimerSetReloadMode returns (Op).
*/
extern void EvrFreeRTOSTimers_vTimerSetReloadMode_Return (void);

/**
  \brief Event sent when function xTimerGetReloadMode is called (API).
  \param[in]  xTimer            Timer object handle.
*/
extern void EvrFreeRTOSTimers_xTimerGetReloadMode (TimerHandle_t xTimer);

/**
  \brief Event sent before function xTimerGetReloadMode returns (Op).
  \param[in]  xReturn           timer auto-reload mode.
*/
extern void EvrFreeRTOSTimers_xTimerGetReloadMode_Return (BaseType_t xReturn);

/**
  \brief Event sent when function  is called (API).
  \param[in]  xTimer            Timer object handle.
*/
extern void EvrFreeRTOSTimers_uxTimerGetReloadMode (TimerHandle_t xTimer);

/**
  \brief Event sent before function  returns (Op).
  \param[in]  uxReturn          timer auto-reload mode.
*/
extern void EvrFreeRTOSTimers_uxTimerGetReloadMode_Return (UBaseType_t uxReturn);

/**
  \brief Event sent when function xTimerGetExpiryTime is called (API).
  \param[in]  xTimer            Timer object handle.
*/
extern void EvrFreeRTOSTimers_xTimerGetExpiryTime (TimerHandle_t xTimer);

/**
  \brief Event sent before function xTimerGetExpiryTime returns (Op).
  \param[in]  xReturn           the time at which the timer will expire.
*/
extern void EvrFreeRTOSTimers_xTimerGetExpiryTime_Return (TickType_t xReturn);

/**
  \brief Event sent when function xTimerGetStaticBuffer is called (API).
  \param[in]  xTimer            Timer object handle.
  \param[in]  ppxTimerBuffer    pointer to memory where object buffer location is stored.
*/
extern void EvrFreeRTOSTimers_xTimerGetStaticBuffer (TimerHandle_t xTimer, /*StaticTimer_t** */uint32_t ppxTimerBuffer);

/**
  \brief Event sent before function xTimerGetStaticBuffer returns (Op).
  \param[in]  xReturn           execution status.
*/
extern void EvrFreeRTOSTimers_xTimerGetStaticBuffer_Return (BaseType_t xReturn);

/**
  \brief Event sent when function pcTimerGetName is called (API).
  \param[in]  xTimer            Timer object handle.
*/
extern void EvrFreeRTOSTimers_pcTimerGetName (TimerHandle_t xTimer);

/**
  \brief Event sent before function pcTimerGetName returns (Op).
  \param[in]  pcTimerName       the name assigned to the Timer object.
*/
extern void EvrFreeRTOSTimers_pcTimerGetName_Return (const char *pcTimerName);

/**
  \brief Event sent when function xTimerIsTimerActive is called (API).
  \param[in]  xTimer            Timer object handle.
*/
extern void EvrFreeRTOSTimers_xTimerIsTimerActive (TimerHandle_t xTimer);

/**
  \brief Event sent before function xTimerIsTimerActive returns (Op).
  \param[in]  xReturn           indication whether the timer is active or dormant.
*/
extern void EvrFreeRTOSTimers_xTimerIsTimerActive_Return (BaseType_t xReturn);

/**
  \brief Event sent when function pvTimerGetTimerID is called (API).
  \param[in]  xTimer            Timer object handle.
*/
extern void EvrFreeRTOSTimers_pvTimerGetTimerID (const TimerHandle_t xTimer);

/**
  \brief Event sent before function pvTimerGetTimerID returns (Op).
  \param[in]  pvReturn          ID assigned to the timer being queried.
*/
extern void EvrFreeRTOSTimers_pvTimerGetTimerID_Return (void *pvReturn);

/**
  \brief Event sent when function vTimerSetTimerID is called (API).
  \param[in]  xTimer            Timer object handle.
  \param[in]  pvNewID           ID to assign to the timer.
*/
extern void EvrFreeRTOSTimers_vTimerSetTimerID (TimerHandle_t xTimer, void *pvNewID);

/**
  \brief Event sent before function vTimerSetTimerID returns (Op).
*/
extern void EvrFreeRTOSTimers_vTimerSetTimerID_Return (void);

/**
  \brief Event sent when function xTimerPendFunctionCallFromISR is called (API).
  \param[in]  xFunctionToPend           the callback function to execute from the timer daemon task.
  \param[in]  pvParameter1              the callback function first argument.
  \param[in]  ulParameter2              the callback function second argument.
  \param[in]  pxHigherPriorityTaskWoken pointer to variable where information if any task was woken will be stored.
*/
extern void EvrFreeRTOSTimers_xTimerPendFunctionCallFromISR (PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, BaseType_t *pxHigherPriorityTaskWoken);

/**
  \brief Event sent before function xTimerPendFunctionCallFromISR returns (Op).
  \param[in]  xReturn           execution status.
*/
extern void EvrFreeRTOSTimers_xTimerPendFunctionCallFromISR_Return (BaseType_t xReturn);

/**
  \brief Event sent when function  is called (API).
  \param[in]  xFunctionToPend   the callback function to execute from the timer daemon task.
  \param[in]  pvParameter1      the callback function first argument.
  \param[in]  ulParameter2      the callback function second argument.
  \param[in]  xTicksToWait      the amount of time to wait until timer queue is free.
*/
extern void EvrFreeRTOSTimers_xTimerPendFunctionCall (PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, TickType_t xTicksToWait);

/**
  \brief Event sent before function  returns (Op).
  \param[in]  xReturn           execution status.
*/
extern void EvrFreeRTOSTimers_xTimerPendFunctionCall_Return (BaseType_t xReturn);

/**
  \brief Event sent when function uxTimerGetTimerNumber is called (API).
  \param[in]  xTimer            Timer object handle.
*/
extern void EvrFreeRTOSTimers_uxTimerGetTimerNumber (TimerHandle_t xTimer);

/**
  \brief Event sent before function uxTimerGetTimerNumber returns (Op).
  \param[in]  uxReturn          the number assigned to the Timer object.
*/
extern void EvrFreeRTOSTimers_uxTimerGetTimerNumber_Return (UBaseType_t uxReturn);

/**
  \brief Event sent when function vTimerSetTimerNumber is called (API).
  \param[in]  xTimer            Timer object handle.
  \param[in]  uxTimerNumber     the number to assign to the Timer object.
*/
extern void EvrFreeRTOSTimers_vTimerSetTimerNumber (TimerHandle_t xTimer, UBaseType_t uxTimerNumber);

/**
  \brief Event sent before function vTimerSetTimerNumber returns (Op).
*/
extern void EvrFreeRTOSTimers_vTimerSetTimerNumber_Return (void);


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
  \brief Event sent when function xEventGroupCreateStatic is called (API).
  \param[in]  pxEventGroupBuffer  pointer to Event Groups data structure.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupCreateStatic (StaticEventGroup_t *pxEventGroupBuffer);

/**
  \brief Event sent before function xEventGroupCreateStatic returns (Op).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupCreateStatic_Return (EventGroupHandle_t xEventGroup);

/**
  \brief Event sent when function xEventGroupCreate is called (API).
*/
extern void EvrFreeRTOSEventGroups_xEventGroupCreate (void);

/**
  \brief Event sent before function xEventGroupCreate returns (Op).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupCreate_Return (EventGroupHandle_t xEventGroup);

/**
  \brief Event sent when function xEventGroupSync is called (API).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
  \param[in]  uxBitsToSet         bits to set in the Event Group before waiting.
  \param[in]  uxBitsToWaitFor     bits to wait for.
  \param[in]  xTicksToWait        amount of time to wait for.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupSync (EventGroupHandle_t xEventGroup,
                                                    const EventBits_t uxBitsToSet,
                                                    const EventBits_t uxBitsToWaitFor,
                                                    TickType_t xTicksToWait);

/**
  \brief Event sent before function xEventGroupSync returns (Op).
  \param[in]  uxReturn            the bits after wait or when block time expired.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupSync_Return (EventBits_t uxReturn);

/**
  \brief Event sent when function xEventGroupWaitBits is called (API).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
  \param[in]  uxBitsToWaitFor     the bits to wait for.
  \param[in]  xClearOnExit        specifies if the bits are cleared on exit.
  \param[in]  xWaitForAllBits     specified if function waits until all bits are set.
  \param[in]  xTicksToWait        amount of time to wait for.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupWaitBits (EventGroupHandle_t xEventGroup,
                                                        const EventBits_t uxBitsToWaitFor,
                                                        const BaseType_t xClearOnExit,
                                                        const BaseType_t xWaitForAllBits,
                                                        TickType_t xTicksToWait);

/**
  \brief Event sent before function xEventGroupWaitBits returns (Op).
  \param[in]  uxReturn            the bits after wait or when block time expired.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupWaitBits_Return (EventBits_t uxReturn);

/**
  \brief Event sent when function xEventGroupClearBits is called (API).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
  \param[in]  uxBitsToClear       the bits to clear.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupClearBits (EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear);

/**
  \brief Event sent before function xEventGroupClearBits returns (Op).
  \param[in]  uxReturn            the bits before clearing.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupClearBits_Return (EventBits_t uxReturn);

/**
  \brief Event sent when function xEventGroupClearBitsFromISR is called (API).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
  \param[in]  uxBitsToClear       the bits to clear.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupClearBitsFromISR (EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear);

/**
  \brief Event sent before function xEventGroupClearBitsFromISR returns (Op).
  \param[in]  xReturn             execution status.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupClearBitsFromISR_Return (BaseType_t xReturn);

/**
  \brief Event sent when function xEventGroupGetBitsFromISR is called (API).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupGetBitsFromISR (EventGroupHandle_t xEventGroup);

/**
  \brief Event sent before function xEventGroupGetBitsFromISR returns (Op).
  \param[in]  uxReturn            the set bits.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupGetBitsFromISR_Return (EventBits_t uxReturn);

/**
  \brief Event sent when function xEventGroupSetBits is called (API).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
  \param[in]  uxBitsToSet         the bits to set.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupSetBits (EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet);

/**
  \brief Event sent before function xEventGroupSetBits returns (Op).
  \param[in]  uxReturn            the bits set at the time function returns.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupSetBits_Return (EventBits_t uxReturn);

/**
  \brief Event sent when function vEventGroupDelete is called (API).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
*/
extern void EvrFreeRTOSEventGroups_vEventGroupDelete (EventGroupHandle_t xEventGroup);

/**
  \brief Event sent before function vEventGroupDelete returns (Op).
*/
extern void EvrFreeRTOSEventGroups_vEventGroupDelete_Return (void);

/**
  \brief Event sent when function xEventGroupGetStaticBuffer is called (API).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
  \param[in]  ppxEventGroupBuffer pointer to memory where pointer to Event Groups object data buffer is stored.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupGetStaticBuffer (EventGroupHandle_t xEventGroup, /*StaticEventGroup_t** */uint32_t ppxEventGroupBuffer);

/**
  \brief Event sent before function xEventGroupGetStaticBuffer returns (Op).
  \param[in]  xReturn             execution status.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupGetStaticBuffer_Return (BaseType_t xReturn);

/**
  \brief Event sent when function vEventGroupSetBitsCallback is called (API).
  \param[in]  pvEventGroup        pointer to Event Groups object handle.
  \param[in]  ulBitsToSet         bits to set.
*/
extern void EvrFreeRTOSEventGroups_vEventGroupSetBitsCallback (void *pvEventGroup, uint32_t ulBitsToSet);

/**
  \brief Event sent before function vEventGroupSetBitsCallback returns (Op).
*/
extern void EvrFreeRTOSEventGroups_vEventGroupSetBitsCallback_Return (void);

/**
  \brief Event sent when function vEventGroupClearBitsCallback is called (API).
  \param[in]  pvEventGroup        pointer to Event Groups object handle.
  \param[in]  ulBitsToClear       bits to clear.
*/
extern void EvrFreeRTOSEventGroups_vEventGroupClearBitsCallback (void *pvEventGroup, uint32_t ulBitsToClear);

/**
  \brief Event sent before function vEventGroupClearBitsCallback returns (Op).
*/
extern void EvrFreeRTOSEventGroups_vEventGroupClearBitsCallback_Return (void);

/**
  \brief Event sent when function xEventGroupSetBitsFromISR is called (API).
  \param[in]  xEventGroup               pointer to Event Groups object handle.
  \param[in]  uxBitsToSet               bits to set
  \param[in]  pxHigherPriorityTaskWoken pointer to variable where information if any task was woken will be stored.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupSetBitsFromISR (EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t * pxHigherPriorityTaskWoken);

/**
  \brief Event sent before function xEventGroupSetBitsFromISR returns (Op).
  \param[in]  xReturn             execution status.
*/
extern void EvrFreeRTOSEventGroups_xEventGroupSetBitsFromISR_Return (BaseType_t xReturn);

/**
  \brief Event sent when function uxEventGroupGetNumber is called (API).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
*/
extern void EvrFreeRTOSEventGroups_uxEventGroupGetNumber (void *xEventGroup);

/**
  \brief Event sent before function uxEventGroupGetNumber returns (Op).
  \param[in]  xReturn             Event Groups object number.
*/
extern void EvrFreeRTOSEventGroups_uxEventGroupGetNumber_Return (UBaseType_t xReturn);

/**
  \brief Event sent when function vEventGroupSetNumber is called (API).
  \param[in]  xEventGroup         pointer to Event Groups object handle.
  \param[in]  uxEventGroupNumber  number to assign to the Event Groups object.
*/
extern void EvrFreeRTOSEventGroups_vEventGroupSetNumber (void *xEventGroup, UBaseType_t uxEventGroupNumber);

/**
  \brief Event sent before function vEventGroupSetNumber returns (Op).
*/
extern void EvrFreeRTOSEventGroups_vEventGroupSetNumber_Return (void);


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

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceMOVED_TASK_TO_DELAYED_LIST_DISABLE))
  #define traceMOVED_TASK_TO_DELAYED_LIST()           EvrFreeRTOSTasks_MovedTaskToDelayedList(pxCurrentTCB)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceMOVED_TASK_TO_OVERFLOW_DELAYED_LIST_DISABLE))
  #define traceMOVED_TASK_TO_OVERFLOW_DELAYED_LIST()  EvrFreeRTOSTasks_MovedTaskToOverflowDelayedList(pxCurrentTCB)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateStatic_DISABLE))
  #define traceENTER_xTaskCreateStatic(pxTC,pcN,ulS,pvP,uxP,puxS,pxTB) \
                                                      EvrFreeRTOSTasks_xTaskCreateStatic((void*)pxTC,pcN,ulS,pvP,uxP,puxS,pxTB)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateStatic_DISABLE))
  #define traceRETURN_xTaskCreateStatic               EvrFreeRTOSTasks_xTaskCreateStatic_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateStaticAffinitySet_DISABLE))
  #define traceENTER_xTaskCreateStaticAffinitySet     EvrFreeRTOSTasks_xTaskCreateStaticAffinitySet
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateStaticAffinitySet_DISABLE))
  #define traceRETURN_xTaskCreateStaticAffinitySet    EvrFreeRTOSTasks_xTaskCreateStaticAffinitySet_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateRestrictedStatic_DISABLE))
  #define traceENTER_xTaskCreateRestrictedStatic(pxT,pxC) EvrFreeRTOSTasks_xTaskCreateRestrictedStatic(pxT, (void **)pxC)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateRestrictedStatic_DISABLE))
  #define traceRETURN_xTaskCreateRestrictedStatic     EvrFreeRTOSTasks_xTaskCreateRestrictedStatic_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateRestrictedStaticAffinitySet_DISABLE))
  #define traceENTER_xTaskCreateRestrictedStaticAffinitySet   EvrFreeRTOSTasks_xTaskCreateRestrictedStaticAffinitySet
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateRestrictedStaticAffinitySet_DISABLE))
  #define traceRETURN_xTaskCreateRestrictedStaticAffinitySet  EvrFreeRTOSTasks_xTaskCreateRestrictedStaticAffinitySet_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateRestricted_DISABLE))
  #define traceENTER_xTaskCreateRestricted(pxT,pxC)   EvrFreeRTOSTasks_xTaskCreateRestricted(pxT,(void **)pxC)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateRestricted_DISABLE))
  #define traceRETURN_xTaskCreateRestricted           EvrFreeRTOSTasks_xTaskCreateRestricted_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateRestrictedAffinitySet_DISABLE))
  #define traceENTER_xTaskCreateRestrictedAffinitySet   EvrFreeRTOSTasks_xTaskCreateRestrictedAffinitySet
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateRestrictedAffinitySet_DISABLE))
  #define traceRETURN_xTaskCreateRestrictedAffinitySet  EvrFreeRTOSTasks_xTaskCreateRestrictedAffinitySet_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreate_DISABLE))
  #define traceENTER_xTaskCreate(pxT,pcN,uxS,pvP,uxP,pxC) \
                                                      EvrFreeRTOSTasks_xTaskCreate((void*)pxT,pcN,uxS,pvP,uxP,pxC)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreate_DISABLE))
  #define traceRETURN_xTaskCreate                     EvrFreeRTOSTasks_xTaskCreate_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCreateAffinitySet_DISABLE))
  #define traceENTER_xTaskCreateAffinitySet           EvrFreeRTOSTasks_xTaskCreateAffinitySet
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCreateAffinitySet_DISABLE))
  #define traceRETURN_xTaskCreateAffinitySet          EvrFreeRTOSTasks_xTaskCreateAffinitySet_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskDelete_DISABLE))
  #define traceENTER_vTaskDelete                      EvrFreeRTOSTasks_vTaskDelete
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskDelete_DISABLE))
  #define traceRETURN_vTaskDelete                     EvrFreeRTOSTasks_vTaskDelete_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskDelayUntil_DISABLE))
  #define traceENTER_xTaskDelayUntil                  EvrFreeRTOSTasks_xTaskDelayUntil
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskDelayUntil_DISABLE))
  #define traceRETURN_xTaskDelayUntil                 EvrFreeRTOSTasks_xTaskDelayUntil_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskDelay_DISABLE))
  #define traceENTER_vTaskDelay                       EvrFreeRTOSTasks_vTaskDelay
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskDelay_DISABLE))
  #define traceRETURN_vTaskDelay                      EvrFreeRTOSTasks_vTaskDelay_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_eTaskGetState_DISABLE))
  #define traceENTER_eTaskGetState                    EvrFreeRTOSTasks_eTaskGetState
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_eTaskGetState_DISABLE))
  #define traceRETURN_eTaskGetState                   EvrFreeRTOSTasks_eTaskGetState_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskPriorityGet_DISABLE))
  #define traceENTER_uxTaskPriorityGet                EvrFreeRTOSTasks_uxTaskPriorityGet
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskPriorityGet_DISABLE))
  #define traceRETURN_uxTaskPriorityGet               EvrFreeRTOSTasks_uxTaskPriorityGet_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskPriorityGetFromISR_DISABLE))
  #define traceENTER_uxTaskPriorityGetFromISR         EvrFreeRTOSTasks_uxTaskPriorityGetFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskPriorityGetFromISR_DISABLE))
  #define traceRETURN_uxTaskPriorityGetFromISR        EvrFreeRTOSTasks_uxTaskPriorityGetFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskBasePriorityGet_DISABLE))
  #define traceENTER_uxTaskBasePriorityGet            EvrFreeRTOSTasks_uxTaskBasePriorityGet
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskBasePriorityGet_DISABLE))
  #define traceRETURN_uxTaskBasePriorityGet           EvrFreeRTOSTasks_uxTaskBasePriorityGet_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskBasePriorityGetFromISR_DISABLE))
  #define traceENTER_uxTaskBasePriorityGetFromISR     EvrFreeRTOSTasks_uxTaskBasePriorityGetFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskBasePriorityGetFromISR_DISABLE))
  #define traceRETURN_uxTaskBasePriorityGetFromISR    EvrFreeRTOSTasks_uxTaskBasePriorityGetFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPrioritySet_DISABLE))
  #define traceENTER_vTaskPrioritySet                 EvrFreeRTOSTasks_vTaskPrioritySet
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPrioritySet_DISABLE))
  #define traceRETURN_vTaskPrioritySet                EvrFreeRTOSTasks_vTaskPrioritySet_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskCoreAffinitySet_DISABLE))
  #define traceENTER_vTaskCoreAffinitySet             EvrFreeRTOSTasks_vTaskCoreAffinitySet
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskCoreAffinitySet_DISABLE))
  #define traceRETURN_vTaskCoreAffinitySet            EvrFreeRTOSTasks_vTaskCoreAffinitySet_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskCoreAffinityGet_DISABLE))
  #define traceENTER_vTaskCoreAffinityGet             EvrFreeRTOSTasks_vTaskCoreAffinityGet
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskCoreAffinityGet_DISABLE))
  #define traceRETURN_vTaskCoreAffinityGet            EvrFreeRTOSTasks_vTaskCoreAffinityGet_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPreemptionDisable_DISABLE))
  #define traceENTER_vTaskPreemptionDisable           EvrFreeRTOSTasks_vTaskPreemptionDisable
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPreemptionDisable_DISABLE))
  #define traceRETURN_vTaskPreemptionDisable          EvrFreeRTOSTasks_vTaskPreemptionDisable_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPreemptionEnable_DISABLE))
  #define traceENTER_vTaskPreemptionEnable            EvrFreeRTOSTasks_vTaskPreemptionEnable
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPreemptionEnable_DISABLE))
  #define traceRETURN_vTaskPreemptionEnable           EvrFreeRTOSTasks_vTaskPreemptionEnable_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSuspend_DISABLE))
  #define traceENTER_vTaskSuspend                     EvrFreeRTOSTasks_vTaskSuspend
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSuspend_DISABLE))
  #define traceRETURN_vTaskSuspend                    EvrFreeRTOSTasks_vTaskSuspend_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskResume_DISABLE))
  #define traceENTER_vTaskResume                      EvrFreeRTOSTasks_vTaskResume
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskResume_DISABLE))
  #define traceRETURN_vTaskResume                     EvrFreeRTOSTasks_vTaskResume_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskResumeFromISR_DISABLE))
  #define traceENTER_xTaskResumeFromISR               EvrFreeRTOSTasks_xTaskResumeFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskResumeFromISR_DISABLE))
  #define traceRETURN_xTaskResumeFromISR              EvrFreeRTOSTasks_xTaskResumeFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskStartScheduler_DISABLE))
  #define traceENTER_vTaskStartScheduler              EvrFreeRTOSTasks_vTaskStartScheduler
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskStartScheduler_DISABLE))
  #define traceRETURN_vTaskStartScheduler             EvrFreeRTOSTasks_vTaskStartScheduler_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskEndScheduler_DISABLE))
  #define traceENTER_vTaskEndScheduler                EvrFreeRTOSTasks_vTaskEndScheduler
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskEndScheduler_DISABLE))
  #define traceRETURN_vTaskEndScheduler               EvrFreeRTOSTasks_vTaskEndScheduler_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSuspendAll_DISABLE))
  #define traceENTER_vTaskSuspendAll                  EvrFreeRTOSTasks_vTaskSuspendAll
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSuspendAll_DISABLE))
  #define traceRETURN_vTaskSuspendAll                 EvrFreeRTOSTasks_vTaskSuspendAll_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskResumeAll_DISABLE))
  #define traceENTER_xTaskResumeAll                   EvrFreeRTOSTasks_xTaskResumeAll
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskResumeAll_DISABLE))
  #define traceRETURN_xTaskResumeAll                  EvrFreeRTOSTasks_xTaskResumeAll_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetTickCount_DISABLE))
  #define traceENTER_xTaskGetTickCount                EvrFreeRTOSTasks_xTaskGetTickCount
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetTickCount_DISABLE))
  #define traceRETURN_xTaskGetTickCount               EvrFreeRTOSTasks_xTaskGetTickCount_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetTickCountFromISR_DISABLE))
  #define traceENTER_xTaskGetTickCountFromISR         EvrFreeRTOSTasks_xTaskGetTickCountFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetTickCountFromISR_DISABLE))
  #define traceRETURN_xTaskGetTickCountFromISR        EvrFreeRTOSTasks_xTaskGetTickCountFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskGetNumberOfTasks_DISABLE))
  #define traceENTER_uxTaskGetNumberOfTasks           EvrFreeRTOSTasks_uxTaskGetNumberOfTasks
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskGetNumberOfTasks_DISABLE))
  #define traceRETURN_uxTaskGetNumberOfTasks          EvrFreeRTOSTasks_uxTaskGetNumberOfTasks_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_pcTaskGetName_DISABLE))
  #define traceENTER_pcTaskGetName                    EvrFreeRTOSTasks_pcTaskGetName
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_pcTaskGetName_DISABLE))
  #define traceRETURN_pcTaskGetName                   EvrFreeRTOSTasks_pcTaskGetName_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetHandle_DISABLE))
  #define traceENTER_xTaskGetHandle                   EvrFreeRTOSTasks_xTaskGetHandle
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetHandle_DISABLE))
  #define traceRETURN_xTaskGetHandle                  EvrFreeRTOSTasks_xTaskGetHandle_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetStaticBuffers_DISABLE))
  #define traceENTER_xTaskGetStaticBuffers(x,ppux,ppx) EvrFreeRTOSTasks_xTaskGetStaticBuffers(x,ppux,(uint32_t)ppx)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetStaticBuffers_DISABLE))
  #define traceRETURN_xTaskGetStaticBuffers           EvrFreeRTOSTasks_xTaskGetStaticBuffers_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskGetSystemState_DISABLE))
  #define traceENTER_uxTaskGetSystemState             EvrFreeRTOSTasks_uxTaskGetSystemState
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskGetSystemState_DISABLE))
  #define traceRETURN_uxTaskGetSystemState            EvrFreeRTOSTasks_uxTaskGetSystemState_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetIdleTaskHandle_DISABLE))
  #define traceENTER_xTaskGetIdleTaskHandle           EvrFreeRTOSTasks_xTaskGetIdleTaskHandle
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetIdleTaskHandle_DISABLE))
  #define traceRETURN_xTaskGetIdleTaskHandle          EvrFreeRTOSTasks_xTaskGetIdleTaskHandle_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetIdleTaskHandleForCore_DISABLE))
  #define traceENTER_xTaskGetIdleTaskHandleForCore    EvrFreeRTOSTasks_xTaskGetIdleTaskHandleForCore
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetIdleTaskHandleForCore_DISABLE))
  #define traceRETURN_xTaskGetIdleTaskHandleForCore   EvrFreeRTOSTasks_xTaskGetIdleTaskHandleForCore_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskStepTick_DISABLE))
  #define traceENTER_vTaskStepTick                    EvrFreeRTOSTasks_vTaskStepTick
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskStepTick_DISABLE))
  #define traceRETURN_vTaskStepTick                   EvrFreeRTOSTasks_vTaskStepTick_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCatchUpTicks_DISABLE))
  #define traceENTER_xTaskCatchUpTicks                EvrFreeRTOSTasks_xTaskCatchUpTicks
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCatchUpTicks_DISABLE))
  #define traceRETURN_xTaskCatchUpTicks               EvrFreeRTOSTasks_xTaskCatchUpTicks_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskAbortDelay_DISABLE))
  #define traceENTER_xTaskAbortDelay                  EvrFreeRTOSTasks_xTaskAbortDelay
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskAbortDelay_DISABLE))
  #define traceRETURN_xTaskAbortDelay                 EvrFreeRTOSTasks_xTaskAbortDelay_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskIncrementTick_DISABLE))
  #define traceENTER_xTaskIncrementTick               EvrFreeRTOSTasks_xTaskIncrementTick
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskIncrementTick_DISABLE))
  #define traceRETURN_xTaskIncrementTick              EvrFreeRTOSTasks_xTaskIncrementTick_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSetApplicationTaskTag_DISABLE))
  #define traceENTER_vTaskSetApplicationTaskTag       EvrFreeRTOSTasks_vTaskSetApplicationTaskTag
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSetApplicationTaskTag_DISABLE))
  #define traceRETURN_vTaskSetApplicationTaskTag      EvrFreeRTOSTasks_vTaskSetApplicationTaskTag_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetApplicationTaskTag_DISABLE))
  #define traceENTER_xTaskGetApplicationTaskTag       EvrFreeRTOSTasks_xTaskGetApplicationTaskTag
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetApplicationTaskTag_DISABLE))
  #define traceRETURN_xTaskGetApplicationTaskTag      EvrFreeRTOSTasks_xTaskGetApplicationTaskTag_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetApplicationTaskTagFromISR_DISABLE))
  #define traceENTER_xTaskGetApplicationTaskTagFromISR  EvrFreeRTOSTasks_xTaskGetApplicationTaskTagFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetApplicationTaskTagFromISR_DISABLE))
  #define traceRETURN_xTaskGetApplicationTaskTagFromISR EvrFreeRTOSTasks_xTaskGetApplicationTaskTagFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCallApplicationTaskHook_DISABLE))
  #define traceENTER_xTaskCallApplicationTaskHook     EvrFreeRTOSTasks_xTaskCallApplicationTaskHook
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCallApplicationTaskHook_DISABLE))
  #define traceRETURN_xTaskCallApplicationTaskHook    EvrFreeRTOSTasks_xTaskCallApplicationTaskHook_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSwitchContext_DISABLE))
  #define traceENTER_vTaskSwitchContext               EvrFreeRTOSTasks_vTaskSwitchContext
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSwitchContext_DISABLE))
  #define traceRETURN_vTaskSwitchContext              EvrFreeRTOSTasks_vTaskSwitchContext_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPlaceOnEventList_DISABLE))
  #define traceENTER_vTaskPlaceOnEventList            EvrFreeRTOSTasks_vTaskPlaceOnEventList
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPlaceOnEventList_DISABLE))
  #define traceRETURN_vTaskPlaceOnEventList           EvrFreeRTOSTasks_vTaskPlaceOnEventList_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPlaceOnUnorderedEventList_DISABLE))
  #define traceENTER_vTaskPlaceOnUnorderedEventList   EvrFreeRTOSTasks_vTaskPlaceOnUnorderedEventList
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPlaceOnUnorderedEventList_DISABLE))
  #define traceRETURN_vTaskPlaceOnUnorderedEventList  EvrFreeRTOSTasks_vTaskPlaceOnUnorderedEventList_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPlaceOnEventListRestricted_DISABLE))
  #define traceENTER_vTaskPlaceOnEventListRestricted  EvrFreeRTOSTasks_vTaskPlaceOnEventListRestricted
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPlaceOnEventListRestricted_DISABLE))
  #define traceRETURN_vTaskPlaceOnEventListRestricted EvrFreeRTOSTasks_vTaskPlaceOnEventListRestricted_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskRemoveFromEventList_DISABLE))
  #define traceENTER_xTaskRemoveFromEventList         EvrFreeRTOSTasks_xTaskRemoveFromEventList
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskRemoveFromEventList_DISABLE))
  #define traceRETURN_xTaskRemoveFromEventList        EvrFreeRTOSTasks_xTaskRemoveFromEventList_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskRemoveFromUnorderedEventList_DISABLE))
  #define traceENTER_vTaskRemoveFromUnorderedEventList  EvrFreeRTOSTasks_vTaskRemoveFromUnorderedEventList
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskRemoveFromUnorderedEventList_DISABLE))
  #define traceRETURN_vTaskRemoveFromUnorderedEventList EvrFreeRTOSTasks_vTaskRemoveFromUnorderedEventList_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSetTimeOutState_DISABLE))
  #define traceENTER_vTaskSetTimeOutState             EvrFreeRTOSTasks_vTaskSetTimeOutState
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSetTimeOutState_DISABLE))
  #define traceRETURN_vTaskSetTimeOutState            EvrFreeRTOSTasks_vTaskSetTimeOutState_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskInternalSetTimeOutState_DISABLE))
  #define traceENTER_vTaskInternalSetTimeOutState     EvrFreeRTOSTasks_vTaskInternalSetTimeOutState
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskInternalSetTimeOutState_DISABLE))
  #define traceRETURN_vTaskInternalSetTimeOutState    EvrFreeRTOSTasks_vTaskInternalSetTimeOutState_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskCheckForTimeOut_DISABLE))
  #define traceENTER_xTaskCheckForTimeOut             EvrFreeRTOSTasks_xTaskCheckForTimeOut
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskCheckForTimeOut_DISABLE))
  #define traceRETURN_xTaskCheckForTimeOut            EvrFreeRTOSTasks_xTaskCheckForTimeOut_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskMissedYield_DISABLE))
  #define traceENTER_vTaskMissedYield                 EvrFreeRTOSTasks_vTaskMissedYield
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskMissedYield_DISABLE))
  #define traceRETURN_vTaskMissedYield                EvrFreeRTOSTasks_vTaskMissedYield_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskGetTaskNumber_DISABLE))
  #define traceENTER_uxTaskGetTaskNumber              EvrFreeRTOSTasks_uxTaskGetTaskNumber
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskGetTaskNumber_DISABLE))
  #define traceRETURN_uxTaskGetTaskNumber             EvrFreeRTOSTasks_uxTaskGetTaskNumber_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSetTaskNumber_DISABLE))
  #define traceENTER_vTaskSetTaskNumber               EvrFreeRTOSTasks_vTaskSetTaskNumber
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSetTaskNumber_DISABLE))
  #define traceRETURN_vTaskSetTaskNumber              EvrFreeRTOSTasks_vTaskSetTaskNumber_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_eTaskConfirmSleepModeStatus_DISABLE))
  #define traceENTER_eTaskConfirmSleepModeStatus      EvrFreeRTOSTasks_eTaskConfirmSleepModeStatus
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_eTaskConfirmSleepModeStatus_DISABLE))
  #define traceRETURN_eTaskConfirmSleepModeStatus     EvrFreeRTOSTasks_eTaskConfirmSleepModeStatus_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskSetThreadLocalStoragePointer_DISABLE))
  #define traceENTER_vTaskSetThreadLocalStoragePointer  EvrFreeRTOSTasks_vTaskSetThreadLocalStoragePointer
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskSetThreadLocalStoragePointer_DISABLE))
  #define traceRETURN_vTaskSetThreadLocalStoragePointer EvrFreeRTOSTasks_vTaskSetThreadLocalStoragePointer_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_pvTaskGetThreadLocalStoragePointer_DISABLE))
  #define traceENTER_pvTaskGetThreadLocalStoragePointer EvrFreeRTOSTasks_pvTaskGetThreadLocalStoragePointer
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_pvTaskGetThreadLocalStoragePointer_DISABLE))
  #define traceRETURN_pvTaskGetThreadLocalStoragePointer EvrFreeRTOSTasks_pvTaskGetThreadLocalStoragePointer_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskAllocateMPURegions_DISABLE))
  #define traceENTER_vTaskAllocateMPURegions          EvrFreeRTOSTasks_vTaskAllocateMPURegions
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskAllocateMPURegions_DISABLE))
  #define traceRETURN_vTaskAllocateMPURegions         EvrFreeRTOSTasks_vTaskAllocateMPURegions_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskGetInfo_DISABLE))
  #define traceENTER_vTaskGetInfo                     EvrFreeRTOSTasks_vTaskGetInfo
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskGetInfo_DISABLE))
  #define traceRETURN_vTaskGetInfo                    EvrFreeRTOSTasks_vTaskGetInfo_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskGetStackHighWaterMark2_DISABLE))
  #define traceENTER_uxTaskGetStackHighWaterMark2     EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark2
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskGetStackHighWaterMark2_DISABLE))
  #define traceRETURN_uxTaskGetStackHighWaterMark2    EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark2_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskGetStackHighWaterMark_DISABLE))
  #define traceENTER_uxTaskGetStackHighWaterMark      EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskGetStackHighWaterMark_DISABLE))
  #define traceRETURN_uxTaskGetStackHighWaterMark     EvrFreeRTOSTasks_uxTaskGetStackHighWaterMark_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetCurrentTaskHandle_DISABLE))
  #define traceENTER_xTaskGetCurrentTaskHandle        EvrFreeRTOSTasks_xTaskGetCurrentTaskHandle
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetCurrentTaskHandle_DISABLE))
  #define traceRETURN_xTaskGetCurrentTaskHandle       EvrFreeRTOSTasks_xTaskGetCurrentTaskHandle_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetCurrentTaskHandleForCore_DISABLE))
  #define traceENTER_xTaskGetCurrentTaskHandleForCore EvrFreeRTOSTasks_xTaskGetCurrentTaskHandleForCore
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetCurrentTaskHandleForCore_DISABLE))
  #define traceRETURN_xTaskGetCurrentTaskHandleForCore  EvrFreeRTOSTasks_xTaskGetCurrentTaskHandleForCore_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetSchedulerState_DISABLE))
  #define traceENTER_xTaskGetSchedulerState           EvrFreeRTOSTasks_xTaskGetSchedulerState
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetSchedulerState_DISABLE))
  #define traceRETURN_xTaskGetSchedulerState          EvrFreeRTOSTasks_xTaskGetSchedulerState_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskPriorityInherit_DISABLE))
  #define traceENTER_xTaskPriorityInherit             EvrFreeRTOSTasks_xTaskPriorityInherit
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskPriorityInherit_DISABLE))
  #define traceRETURN_xTaskPriorityInherit            EvrFreeRTOSTasks_xTaskPriorityInherit_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskPriorityDisinherit_DISABLE))
  #define traceENTER_xTaskPriorityDisinherit          EvrFreeRTOSTasks_xTaskPriorityDisinherit
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskPriorityDisinherit_DISABLE))
  #define traceRETURN_xTaskPriorityDisinherit         EvrFreeRTOSTasks_xTaskPriorityDisinherit_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskPriorityDisinheritAfterTimeout_DISABLE))
  #define traceENTER_vTaskPriorityDisinheritAfterTimeout  EvrFreeRTOSTasks_vTaskPriorityDisinheritAfterTimeout
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskPriorityDisinheritAfterTimeout_DISABLE))
  #define traceRETURN_vTaskPriorityDisinheritAfterTimeout EvrFreeRTOSTasks_vTaskPriorityDisinheritAfterTimeout_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskYieldWithinAPI_DISABLE))
  #define traceENTER_vTaskYieldWithinAPI              EvrFreeRTOSTasks_vTaskYieldWithinAPI
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskYieldWithinAPI_DISABLE))
  #define traceRETURN_vTaskYieldWithinAPI             EvrFreeRTOSTasks_vTaskYieldWithinAPI_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskEnterCritical_DISABLE))
  #define traceENTER_vTaskEnterCritical               EvrFreeRTOSTasks_vTaskEnterCritical
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskEnterCritical_DISABLE))
  #define traceRETURN_vTaskEnterCritical              EvrFreeRTOSTasks_vTaskEnterCritical_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskEnterCriticalFromISR_DISABLE))
  #define traceENTER_vTaskEnterCriticalFromISR        EvrFreeRTOSTasks_vTaskEnterCriticalFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskEnterCriticalFromISR_DISABLE))
  #define traceRETURN_vTaskEnterCriticalFromISR       EvrFreeRTOSTasks_vTaskEnterCriticalFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskExitCritical_DISABLE))
  #define traceENTER_vTaskExitCritical                EvrFreeRTOSTasks_vTaskExitCritical
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskExitCritical_DISABLE))
  #define traceRETURN_vTaskExitCritical               EvrFreeRTOSTasks_vTaskExitCritical_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskExitCriticalFromISR_DISABLE))
  #define traceENTER_vTaskExitCriticalFromISR         EvrFreeRTOSTasks_vTaskExitCriticalFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskExitCriticalFromISR_DISABLE))
  #define traceRETURN_vTaskExitCriticalFromISR        EvrFreeRTOSTasks_vTaskExitCriticalFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskListTasks_DISABLE))
  #define traceENTER_vTaskListTasks                   EvrFreeRTOSTasks_vTaskListTasks
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskListTasks_DISABLE))
  #define traceRETURN_vTaskListTasks                  EvrFreeRTOSTasks_vTaskListTasks_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskGetRunTimeStatistics_DISABLE))
  #define traceENTER_vTaskGetRunTimeStatistics        EvrFreeRTOSTasks_vTaskGetRunTimeStatistics
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskGetRunTimeStatistics_DISABLE))
  #define traceRETURN_vTaskGetRunTimeStatistics       EvrFreeRTOSTasks_vTaskGetRunTimeStatistics_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTaskResetEventItemValue_DISABLE))
  #define traceENTER_uxTaskResetEventItemValue        EvrFreeRTOSTasks_uxTaskResetEventItemValue
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTaskResetEventItemValue_DISABLE))
  #define traceRETURN_uxTaskResetEventItemValue       EvrFreeRTOSTasks_uxTaskResetEventItemValue_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_pvTaskIncrementMutexHeldCount_DISABLE))
  #define traceENTER_pvTaskIncrementMutexHeldCount    EvrFreeRTOSTasks_pvTaskIncrementMutexHeldCount
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_pvTaskIncrementMutexHeldCount_DISABLE))
  #define traceRETURN_pvTaskIncrementMutexHeldCount   EvrFreeRTOSTasks_pvTaskIncrementMutexHeldCount_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGenericNotifyTake_DISABLE))
  #define traceENTER_ulTaskGenericNotifyTake          EvrFreeRTOSTasks_ulTaskGenericNotifyTake
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGenericNotifyTake_DISABLE))
  #define traceRETURN_ulTaskGenericNotifyTake         EvrFreeRTOSTasks_ulTaskGenericNotifyTake_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGenericNotifyWait_DISABLE))
  #define traceENTER_xTaskGenericNotifyWait           EvrFreeRTOSTasks_xTaskGenericNotifyWait
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGenericNotifyWait_DISABLE))
  #define traceRETURN_xTaskGenericNotifyWait          EvrFreeRTOSTasks_xTaskGenericNotifyWait_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGenericNotify_DISABLE))
  #define traceENTER_xTaskGenericNotify               EvrFreeRTOSTasks_xTaskGenericNotify
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGenericNotify_DISABLE))
  #define traceRETURN_xTaskGenericNotify              EvrFreeRTOSTasks_xTaskGenericNotify_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGenericNotifyFromISR_DISABLE))
  #define traceENTER_xTaskGenericNotifyFromISR        EvrFreeRTOSTasks_xTaskGenericNotifyFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGenericNotifyFromISR_DISABLE))
  #define traceRETURN_xTaskGenericNotifyFromISR       EvrFreeRTOSTasks_xTaskGenericNotifyFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTaskGenericNotifyGiveFromISR_DISABLE))
  #define traceENTER_vTaskGenericNotifyGiveFromISR    EvrFreeRTOSTasks_vTaskGenericNotifyGiveFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTaskGenericNotifyGiveFromISR_DISABLE))
  #define traceRETURN_vTaskGenericNotifyGiveFromISR   EvrFreeRTOSTasks_vTaskGenericNotifyGiveFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGenericNotifyStateClear_DISABLE))
  #define traceENTER_xTaskGenericNotifyStateClear     EvrFreeRTOSTasks_xTaskGenericNotifyStateClear
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGenericNotifyStateClear_DISABLE))
  #define traceRETURN_xTaskGenericNotifyStateClear    EvrFreeRTOSTasks_xTaskGenericNotifyStateClear_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGenericNotifyValueClear_DISABLE))
  #define traceENTER_ulTaskGenericNotifyValueClear    EvrFreeRTOSTasks_ulTaskGenericNotifyValueClear
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGenericNotifyValueClear_DISABLE))
  #define traceRETURN_ulTaskGenericNotifyValueClear   EvrFreeRTOSTasks_ulTaskGenericNotifyValueClear_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGetRunTimeCounter_DISABLE))
  #define traceENTER_ulTaskGetRunTimeCounter          EvrFreeRTOSTasks_ulTaskGetRunTimeCounter
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGetRunTimeCounter_DISABLE))
  #define traceRETURN_ulTaskGetRunTimeCounter         EvrFreeRTOSTasks_ulTaskGetRunTimeCounter_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGetRunTimePercent_DISABLE))
  #define traceENTER_ulTaskGetRunTimePercent          EvrFreeRTOSTasks_ulTaskGetRunTimePercent
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGetRunTimePercent_DISABLE))
  #define traceRETURN_ulTaskGetRunTimePercent         EvrFreeRTOSTasks_ulTaskGetRunTimePercent_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGetIdleRunTimeCounter_DISABLE))
  #define traceENTER_ulTaskGetIdleRunTimeCounter      EvrFreeRTOSTasks_ulTaskGetIdleRunTimeCounter
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGetIdleRunTimeCounter_DISABLE))
  #define traceRETURN_ulTaskGetIdleRunTimeCounter     EvrFreeRTOSTasks_ulTaskGetIdleRunTimeCounter_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_ulTaskGetIdleRunTimePercent_DISABLE))
  #define traceENTER_ulTaskGetIdleRunTimePercent      EvrFreeRTOSTasks_ulTaskGetIdleRunTimePercent
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_ulTaskGetIdleRunTimePercent_DISABLE))
  #define traceRETURN_ulTaskGetIdleRunTimePercent     EvrFreeRTOSTasks_ulTaskGetIdleRunTimePercent_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTaskGetMPUSettings_DISABLE))
  #define traceENTER_xTaskGetMPUSettings              EvrFreeRTOSTasks_xTaskGetMPUSettings
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTaskGetMPUSettings_DISABLE))
  #define traceRETURN_xTaskGetMPUSettings             EvrFreeRTOSTasks_xTaskGetMPUSettings_Return
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
  #define tracePEND_FUNC_CALL(xF,pvP,ulP,xR)          EvrFreeRTOSTimers_PendFuncCall((void*)xF,pvP,ulP,xR)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(tracePEND_FUNC_CALL_FROM_ISR_DISABLE))
  #define tracePEND_FUNC_CALL_FROM_ISR(xF,pvP,ulP,xR) EvrFreeRTOSTimers_PendFuncCallFromIsr((void*)xF,pvP,ulP,xR)
#endif


#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerCreateTimerTask_DISABLE))
  #define traceENTER_xTimerCreateTimerTask            EvrFreeRTOSTimers_xTimerCreateTimerTask
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerCreateTimerTask_DISABLE))
  #define traceRETURN_xTimerCreateTimerTask           EvrFreeRTOSTimers_xTimerCreateTimerTask_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerCreate_DISABLE))
  #define traceENTER_xTimerCreate(pcT,xT,xA,pvT,pxC)  EvrFreeRTOSTimers_xTimerCreate(pcT,xT,xA,pvT,(void*)pxC)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerCreate_DISABLE))
  #define traceRETURN_xTimerCreate                    EvrFreeRTOSTimers_xTimerCreate_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerCreateStatic_DISABLE))
  #define traceENTER_xTimerCreateStatic(pcT,xT,xA,pvT,pxC,pxT) \
                                                      EvrFreeRTOSTimers_xTimerCreateStatic(pcT,xT,xA,pvT,(void*)pxC,pxT)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerCreateStatic_DISABLE))
  #define traceRETURN_xTimerCreateStatic              EvrFreeRTOSTimers_xTimerCreateStatic_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGenericCommandFromTask_DISABLE))
  #define traceENTER_xTimerGenericCommandFromTask     EvrFreeRTOSTimers_xTimerGenericCommandFromTask
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGenericCommandFromTask_DISABLE))
  #define traceRETURN_xTimerGenericCommandFromTask    EvrFreeRTOSTimers_xTimerGenericCommandFromTask_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGenericCommandFromISR_DISABLE))
  #define traceENTER_xTimerGenericCommandFromISR      EvrFreeRTOSTimers_xTimerGenericCommandFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGenericCommandFromISR_DISABLE))
  #define traceRETURN_xTimerGenericCommandFromISR     EvrFreeRTOSTimers_xTimerGenericCommandFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGetTimerDaemonTaskHandle_DISABLE))
  #define traceENTER_xTimerGetTimerDaemonTaskHandle   EvrFreeRTOSTimers_xTimerGetTimerDaemonTaskHandle
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGetTimerDaemonTaskHandle_DISABLE))
  #define traceRETURN_xTimerGetTimerDaemonTaskHandle  EvrFreeRTOSTimers_xTimerGetTimerDaemonTaskHandle_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGetPeriod_DISABLE))
  #define traceENTER_xTimerGetPeriod                  EvrFreeRTOSTimers_xTimerGetPeriod
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGetPeriod_DISABLE))
  #define traceRETURN_xTimerGetPeriod                 EvrFreeRTOSTimers_xTimerGetPeriod_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTimerSetReloadMode_DISABLE))
  #define traceENTER_vTimerSetReloadMode              EvrFreeRTOSTimers_vTimerSetReloadMode
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTimerSetReloadMode_DISABLE))
  #define traceRETURN_vTimerSetReloadMode             EvrFreeRTOSTimers_vTimerSetReloadMode_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGetReloadMode_DISABLE))
  #define traceENTER_xTimerGetReloadMode              EvrFreeRTOSTimers_xTimerGetReloadMode
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGetReloadMode_DISABLE))
  #define traceRETURN_xTimerGetReloadMode             EvrFreeRTOSTimers_xTimerGetReloadMode_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTimerGetReloadMode_DISABLE))
  #define traceENTER_uxTimerGetReloadMode             EvrFreeRTOSTimers_uxTimerGetReloadMode
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTimerGetReloadMode_DISABLE))
  #define traceRETURN_uxTimerGetReloadMode            EvrFreeRTOSTimers_uxTimerGetReloadMode_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGetExpiryTime_DISABLE))
  #define traceENTER_xTimerGetExpiryTime              EvrFreeRTOSTimers_xTimerGetExpiryTime
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGetExpiryTime_DISABLE))
  #define traceRETURN_xTimerGetExpiryTime             EvrFreeRTOSTimers_xTimerGetExpiryTime_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerGetStaticBuffer_DISABLE))
  #define traceENTER_xTimerGetStaticBuffer(x,ppx)     EvrFreeRTOSTimers_xTimerGetStaticBuffer(x,(uint32_t)ppx)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerGetStaticBuffer_DISABLE))
  #define traceRETURN_xTimerGetStaticBuffer           EvrFreeRTOSTimers_xTimerGetStaticBuffer_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_pcTimerGetName_DISABLE))
  #define traceENTER_pcTimerGetName                   EvrFreeRTOSTimers_pcTimerGetName
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_pcTimerGetName_DISABLE))
  #define traceRETURN_pcTimerGetName                  EvrFreeRTOSTimers_pcTimerGetName_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerIsTimerActive_DISABLE))
  #define traceENTER_xTimerIsTimerActive              EvrFreeRTOSTimers_xTimerIsTimerActive
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerIsTimerActive_DISABLE))
  #define traceRETURN_xTimerIsTimerActive             EvrFreeRTOSTimers_xTimerIsTimerActive_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_pvTimerGetTimerID_DISABLE))
  #define traceENTER_pvTimerGetTimerID                EvrFreeRTOSTimers_pvTimerGetTimerID
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_pvTimerGetTimerID_DISABLE))
  #define traceRETURN_pvTimerGetTimerID               EvrFreeRTOSTimers_pvTimerGetTimerID_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTimerSetTimerID_DISABLE))
  #define traceENTER_vTimerSetTimerID                 EvrFreeRTOSTimers_vTimerSetTimerID
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTimerSetTimerID_DISABLE))
  #define traceRETURN_vTimerSetTimerID                EvrFreeRTOSTimers_vTimerSetTimerID_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerPendFunctionCallFromISR_DISABLE))
  #define traceENTER_xTimerPendFunctionCallFromISR(xF,pvP,ulP,pxH) \
                                                      EvrFreeRTOSTimers_xTimerPendFunctionCallFromISR((void*)xF,pvP,ulP,pxH)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerPendFunctionCallFromISR_DISABLE))
  #define traceRETURN_xTimerPendFunctionCallFromISR   EvrFreeRTOSTimers_xTimerPendFunctionCallFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xTimerPendFunctionCall_DISABLE))
  #define traceENTER_xTimerPendFunctionCall(xF,pvP,ulP,xT)  \
                                                      EvrFreeRTOSTimers_xTimerPendFunctionCall((void*)xF,pvP,ulP,xT)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xTimerPendFunctionCall_DISABLE))
  #define traceRETURN_xTimerPendFunctionCall          EvrFreeRTOSTimers_xTimerPendFunctionCall_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxTimerGetTimerNumber_DISABLE))
  #define traceENTER_uxTimerGetTimerNumber            EvrFreeRTOSTimers_uxTimerGetTimerNumber
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxTimerGetTimerNumber_DISABLE))
  #define traceRETURN_uxTimerGetTimerNumber           EvrFreeRTOSTimers_uxTimerGetTimerNumber_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vTimerSetTimerNumber_DISABLE))
  #define traceENTER_vTimerSetTimerNumber             EvrFreeRTOSTimers_vTimerSetTimerNumber
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vTimerSetTimerNumber_DISABLE))
  #define traceRETURN_vTimerSetTimerNumber            EvrFreeRTOSTimers_vTimerSetTimerNumber_Return
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


#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupCreateStatic_DISABLE))
  #define traceENTER_xEventGroupCreateStatic          EvrFreeRTOSEventGroups_xEventGroupCreateStatic
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupCreateStatic_DISABLE))
  #define traceRETURN_xEventGroupCreateStatic         EvrFreeRTOSEventGroups_xEventGroupCreateStatic_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupCreate_DISABLE))
  #define traceENTER_xEventGroupCreate                EvrFreeRTOSEventGroups_xEventGroupCreate
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupCreate_DISABLE))
  #define traceRETURN_xEventGroupCreate               EvrFreeRTOSEventGroups_xEventGroupCreate_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupSync_DISABLE))
  #define traceENTER_xEventGroupSync                  EvrFreeRTOSEventGroups_xEventGroupSync
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupSync_DISABLE))
  #define traceRETURN_xEventGroupSync                 EvrFreeRTOSEventGroups_xEventGroupSync_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupWaitBits_DISABLE))
  #define traceENTER_xEventGroupWaitBits              EvrFreeRTOSEventGroups_xEventGroupWaitBits
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupWaitBits_DISABLE))
  #define traceRETURN_xEventGroupWaitBits             EvrFreeRTOSEventGroups_xEventGroupWaitBits_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupClearBits_DISABLE))
  #define traceENTER_xEventGroupClearBits             EvrFreeRTOSEventGroups_xEventGroupClearBits
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupClearBits_DISABLE))
  #define traceRETURN_xEventGroupClearBits            EvrFreeRTOSEventGroups_xEventGroupClearBits_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupClearBitsFromISR_DISABLE))
  #define traceENTER_xEventGroupClearBitsFromISR      EvrFreeRTOSEventGroups_xEventGroupClearBitsFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupClearBitsFromISR_DISABLE))
  #define traceRETURN_xEventGroupClearBitsFromISR     EvrFreeRTOSEventGroups_xEventGroupClearBitsFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupGetBitsFromISR_DISABLE))
  #define traceENTER_xEventGroupGetBitsFromISR        EvrFreeRTOSEventGroups_xEventGroupGetBitsFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupGetBitsFromISR_DISABLE))
  #define traceRETURN_xEventGroupGetBitsFromISR       EvrFreeRTOSEventGroups_xEventGroupGetBitsFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupSetBits_DISABLE))
  #define traceENTER_xEventGroupSetBits               EvrFreeRTOSEventGroups_xEventGroupSetBits
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupSetBits_DISABLE))
  #define traceRETURN_xEventGroupSetBits              EvrFreeRTOSEventGroups_xEventGroupSetBits_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vEventGroupDelete_DISABLE))
  #define traceENTER_vEventGroupDelete                EvrFreeRTOSEventGroups_vEventGroupDelete
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vEventGroupDelete_DISABLE))
  #define traceRETURN_vEventGroupDelete               EvrFreeRTOSEventGroups_vEventGroupDelete_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupGetStaticBuffer_DISABLE))
  #define traceENTER_xEventGroupGetStaticBuffer(x,ppx) EvrFreeRTOSEventGroups_xEventGroupGetStaticBuffer(x,(uint32_t)ppx)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupGetStaticBuffer_DISABLE))
  #define traceRETURN_xEventGroupGetStaticBuffer      EvrFreeRTOSEventGroups_xEventGroupGetStaticBuffer_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vEventGroupSetBitsCallback_DISABLE))
  #define traceENTER_vEventGroupSetBitsCallback       EvrFreeRTOSEventGroups_vEventGroupSetBitsCallback
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vEventGroupSetBitsCallback_DISABLE))
  #define traceRETURN_vEventGroupSetBitsCallback      EvrFreeRTOSEventGroups_vEventGroupSetBitsCallback_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vEventGroupClearBitsCallback_DISABLE))
  #define traceENTER_vEventGroupClearBitsCallback     EvrFreeRTOSEventGroups_vEventGroupClearBitsCallback
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vEventGroupClearBitsCallback_DISABLE))
  #define traceRETURN_vEventGroupClearBitsCallback    EvrFreeRTOSEventGroups_vEventGroupClearBitsCallback_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_xEventGroupSetBitsFromISR_DISABLE))
  #define traceENTER_xEventGroupSetBitsFromISR        EvrFreeRTOSEventGroups_xEventGroupSetBitsFromISR
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_xEventGroupSetBitsFromISR_DISABLE))
  #define traceRETURN_xEventGroupSetBitsFromISR       EvrFreeRTOSEventGroups_xEventGroupSetBitsFromISR_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_uxEventGroupGetNumber_DISABLE))
  #define traceENTER_uxEventGroupGetNumber            EvrFreeRTOSEventGroups_uxEventGroupGetNumber
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_uxEventGroupGetNumber_DISABLE))
  #define traceRETURN_uxEventGroupGetNumber           EvrFreeRTOSEventGroups_uxEventGroupGetNumber_Return
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceENTER_vEventGroupSetNumber_DISABLE))
  #define traceENTER_vEventGroupSetNumber             EvrFreeRTOSEventGroups_vEventGroupSetNumber
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceRETURN_vEventGroupSetNumber_DISABLE))
  #define traceRETURN_vEventGroupSetNumber            EvrFreeRTOSEventGroups_vEventGroupSetNumber_Return
#endif


/* Stream Buffer */

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_FAILED_DISABLE))
  #define traceSTREAM_BUFFER_CREATE_FAILED(ux)        EvrFreeRTOSStreamBuf_StreamBufferCreateFailed(ux)
#endif

#if (!defined(EVR_FREERTOS_DISABLE) && !defined(traceSTREAM_BUFFER_CREATE_STATIC_FAILED_DISABLE))
  #define traceSTREAM_BUFFER_CREATE_STATIC_FAILED(x,ux)  EvrFreeRTOSStreamBuf_StreamBufferCreateStaticFailed(NULL,ux)
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

#undef TaskHandle_t
#undef TimerHandle_t
#undef ConstTaskHandle_t
#undef EventGroupHandle_t
#undef TaskHookFunction_t
#undef TimerCallbackFunction_t

#undef StaticTask_t
#undef StaticTimer_t
#undef StaticEventGroup_t
#undef TaskParameters_t
#undef TaskStatus_t
#undef List_t
#undef ListItem_t
#undef TimeOut_t
#undef MemoryRegion_t

#undef BaseType_t
#undef UBaseType_t
#undef eTaskState
#undef StackType_t
#undef TickType_t
#undef EventBits_t
#undef eSleepModeStatus
#undef eNotifyAction

#endif /* FREERTOS_EVR_H_ */
