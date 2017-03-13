/* --------------------------------------------------------------------------
 * Copyright (c) 2013-2017 ARM Limited. All rights reserved.
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
 *      Name:    cmsis_os2.c
 *      Purpose: CMSIS RTOS2 wrapper for FreeRTOS
 *
 *---------------------------------------------------------------------------*/

#include <string.h>

#include "RTE_Components.h"             // Component selection

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "cmsis_compiler.h"

#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

/*---------------------------------------------------------------------------*/
#ifndef __ARM_ARCH_6M__
  #define __ARM_ARCH_6M__         0
#endif
#ifndef __ARM_ARCH_7M__
  #define __ARM_ARCH_7M__         0
#endif
#ifndef __ARM_ARCH_7EM__
  #define __ARM_ARCH_7EM__        0
#endif
#ifndef __ARM_ARCH_8M_MAIN__
  #define __ARM_ARCH_8M_MAIN__    0
#endif
#ifndef __ARM_ARCH_7A__
  #define __ARM_ARCH_7A__         0
#endif

#if   ((__ARM_ARCH_7M__      == 1U) || \
       (__ARM_ARCH_7EM__     == 1U) || \
       (__ARM_ARCH_8M_MAIN__ == 1U))
#define IS_IRQ_MASKED()           ((__get_PRIMASK() != 0U) || ((KernelState == osKernelRunning) && (__get_BASEPRI() != 0U)))
#elif  (__ARM_ARCH_6M__      == 1U)
#define IS_IRQ_MASKED()           ((__get_PRIMASK() != 0U) &&  (KernelState == osKernelRunning))
#else
#define IS_IRQ_MASKED()            (__get_PRIMASK() != 0U)
#endif

#if    (__ARM_ARCH_7A__      == 1U)
#define IS_IRQ()                  (__get_mode() == 0x12U)
#else
#define IS_IRQ_MODE()             (__get_IPSR() != 0U)
#define IS_IRQ()                  (IS_IRQ_MODE() || IS_IRQ_MASKED())
#endif

/* Limits */
#define MAX_BITS_TASK_NOTIFY      31U
#define MAX_BITS_EVENT_GROUPS     24U

#define THREAD_FLAGS_INVALID_BITS (~((1UL << MAX_BITS_TASK_NOTIFY)  - 1U))
#define EVENT_FLAGS_INVALID_BITS  (~((1UL << MAX_BITS_EVENT_GROUPS) - 1U))

/* Kernel version and identification string definition */
#define KERNEL_VERSION            (((uint32_t)tskKERNEL_VERSION_MAJOR * 10000000UL) | \
                                   ((uint32_t)tskKERNEL_VERSION_MINOR *    10000UL) | \
                                   ((uint32_t)tskKERNEL_VERSION_BUILD *        1UL))

#define KERNEL_ID                 "FreeRTOS V9.0.0"

/* Timer callback information structure definition */
typedef struct {
  osTimerFunc_t func;
  void         *arg;
} TimerCallback_t;

/* Kernel initialization state */
static osKernelState_t KernelState;

/* Heap region definition used by heap_5 variant */
#if defined(RTE_RTOS_FreeRTOS_HEAP_5)
#if (configAPPLICATION_ALLOCATED_HEAP == 1)
/*
  The application writer has already defined the array used for the RTOS
  heap - probably so it can be placed in a special segment or address.
*/
  extern uint8_t ucHeap[configTOTAL_HEAP_SIZE];
#else
  static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
#endif /* configAPPLICATION_ALLOCATED_HEAP */

static HeapRegion_t xHeapRegions[] = {
  { ucHeap, configTOTAL_HEAP_SIZE },
  { NULL,   0                     }
};
#endif /* RTE_RTOS_FreeRTOS_HEAP_5 */

/*---------------------------------------------------------------------------*/

osStatus_t osKernelInitialize (void) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else {
    if (KernelState == osKernelInactive) {
      #if defined(RTE_RTOS_FreeRTOS_HEAP_5)
        vPortDefineHeapRegions (xHeapRegions);
      #endif
      KernelState = osKernelReady;
      stat = osOK;
    } else {
      stat = osError;
    }
  }

  return (stat);
}

osStatus_t osKernelGetInfo (osVersion_t *version, char *id_buf, uint32_t id_size) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else {
    if (version != NULL) {
      version->api    = KERNEL_VERSION;
      version->kernel = KERNEL_VERSION;
    }

    if ((id_buf != NULL) && (id_size != 0U)) {
      if (id_size > sizeof(KERNEL_ID)) {
        id_size = sizeof(KERNEL_ID);
      }
      memcpy(id_buf, KERNEL_ID, id_size);
    }
    stat = osOK;
  }

  return (stat);
}

osKernelState_t osKernelGetState (void) {
  osKernelState_t state;

  if (IS_IRQ()) {
    state = osKernelError;
  }
  else {
    switch (xTaskGetSchedulerState()) {
      case taskSCHEDULER_RUNNING:
        state = osKernelRunning;
        break;

      case taskSCHEDULER_SUSPENDED:
        state = osKernelLocked;
        break;

      case taskSCHEDULER_NOT_STARTED:
      default:
        if (KernelState == osKernelReady) {
          state = osKernelReady;
        } else {
          state = osKernelInactive;
        }
        break;
    }
  }
  return (state);
}

osStatus_t osKernelStart (void) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else {
    if (KernelState == osKernelReady) {
      KernelState = osKernelRunning;
      vTaskStartScheduler();
      stat = osOK;
    } else {
      stat = osError;
    }
  }

  return (stat);
}

int32_t osKernelLock (void) {
  int32_t lock;

  if (IS_IRQ()) {
    lock = (int32_t)osErrorISR;
  }
  else {
    switch (xTaskGetSchedulerState()) {
      case taskSCHEDULER_SUSPENDED:
        lock = 1;
        break;

      case taskSCHEDULER_RUNNING:
        vTaskSuspendAll();
        lock = 0;
        break;

      case taskSCHEDULER_NOT_STARTED:
      default:
        lock = (int32_t)osError;
        break;
    }
  }

  return (lock);
}

int32_t osKernelUnlock (void) {
  int32_t lock;

  if (IS_IRQ()) {
    lock = (int32_t)osErrorISR;
  }
  else {
    switch (xTaskGetSchedulerState()) {
      case taskSCHEDULER_SUSPENDED:
        lock = 1;

        if (xTaskResumeAll() != pdTRUE) {
          if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) {
            lock = (int32_t)osError;
          }
        }
        break;

      case taskSCHEDULER_RUNNING:
        lock = 0;
        break;

      case taskSCHEDULER_NOT_STARTED:
      default:
        lock = (int32_t)osError;
        break;
    }
  }

  return (lock);
}

int32_t osKernelRestoreLock (int32_t lock) {

  if (IS_IRQ()) {
    lock = (int32_t)osErrorISR;
  }
  else {
    switch (xTaskGetSchedulerState()) {
      case taskSCHEDULER_SUSPENDED:
      case taskSCHEDULER_RUNNING:
        if (lock == 1) {
          vTaskSuspendAll();
        }
        else {
          if (lock != 0) {
            lock = (int32_t)osError;
          }
          else {
            if (xTaskResumeAll() != pdTRUE) {
              if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
                lock = (int32_t)osError;
              }
            }
          }
        }
        break;

      case taskSCHEDULER_NOT_STARTED:
      default:
        lock = (int32_t)osError;
        break;
    }
  }

  return (lock);
}

uint64_t osKernelGetTickCount (void) {
  TickType_t ticks;

  if (IS_IRQ()) {
    ticks = 0U;
  } else {
    ticks = xTaskGetTickCount();
  }

  return ((uint64_t)ticks);
}

uint32_t osKernelGetTickFreq (void) {
  uint32_t freq;

  if (IS_IRQ()) {
    freq = 0U;
  } else {
    freq = configTICK_RATE_HZ;
  }

  return (freq);
}

uint32_t osKernelGetSysTimerCount (void) {
  TickType_t ticks;
  uint32_t val;

  portDISABLE_INTERRUPTS();

  ticks = xTaskGetTickCount();

  val = TickTimer_GetPeriod() - TickTimer_GetValue();

  if (TickTimer_GetOverflow() != 0U) {
    val = TickTimer_GetPeriod() - TickTimer_GetValue();
    ticks++;
  }
  val += ticks * (TickTimer_GetPeriod() + 1U);

  portENABLE_INTERRUPTS();

  return (val);
}

uint32_t osKernelGetSysTimerFreq (void) {
  return (configCPU_CLOCK_HZ);
}

/*---------------------------------------------------------------------------*/

osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr) {
  char empty;
  const char *name;
  uint32_t stack;
  TaskHandle_t h;
  UBaseType_t prio;
  int32_t mem;

  h = NULL;

  if (!IS_IRQ() && (func != NULL)) {
    stack = configMINIMAL_STACK_SIZE;
    prio  = (UBaseType_t)osPriorityNormal;

    empty = '\0';
    name  = &empty;
    mem   = -1;

    if (attr != NULL) {
      if (attr->name != NULL) {
        name = attr->name;
      }
      if (attr->priority != osPriorityNone) {
        prio = (UBaseType_t)attr->priority;
      }

      if ((prio < osPriorityIdle) || (prio > osPriorityISR) || ((attr->attr_bits & osThreadJoinable) == osThreadJoinable)) {
        return (NULL);
      }

      if (attr->stack_size > 0U) {
        stack = attr->stack_size;
      }

      if ((attr->cb_mem    != NULL) && (attr->cb_size    >= sizeof(StaticTask_t)) &&
          (attr->stack_mem != NULL) && (attr->stack_size >  0U)) {
        mem = 1;
      }
      else {
        if ((attr->cb_mem == NULL) && (attr->cb_size == 0U) && (attr->stack_mem == NULL)) {
          mem = 0;
        }
      }
    }
    else {
      mem = 0;
    }

    if (mem == 1) {
      h = xTaskCreateStatic ((TaskFunction_t)func, name, stack, argument, prio, (StackType_t  *)attr->stack_mem,
                                                                                (StaticTask_t *)attr->cb_mem);
    }
    else {
      if (mem == 0) {
        if (xTaskCreate ((TaskFunction_t)func, name, (uint16_t)stack, argument, prio, &h) != pdPASS) {
          h = NULL;
        }
      }
    }
  }

  return ((osThreadId_t)h);
}

const char *osThreadGetName (osThreadId_t thread_id) {
  const char *name;

  if (IS_IRQ() || (thread_id == NULL)) {
    name = NULL;
  } else {
    name = pcTaskGetName ((TaskHandle_t)thread_id);
  }

  return (name);
}

osThreadId_t osThreadGetId (void) {
  osThreadId_t id;

  if (IS_IRQ()) {
    id = NULL;
  } else {
    id = (osThreadId_t)xTaskGetCurrentTaskHandle();
  }

  return (id);
}

osThreadState_t osThreadGetState (osThreadId_t thread_id) {
  osThreadState_t state;

  if (IS_IRQ() || (thread_id == NULL)) {
    state = osThreadError;
  }
  else {
    switch (eTaskGetState ((TaskHandle_t)thread_id)) {
      case eRunning:   state = osThreadRunning;    break;
      case eReady:     state = osThreadReady;      break;
      case eBlocked:
      case eSuspended: state = osThreadBlocked;    break;
      case eDeleted:   state = osThreadTerminated; break;
      case eInvalid:
      default:         state = osThreadError;      break;
    }
  }

  return (state);
}

uint32_t osThreadGetStackSpace (osThreadId_t thread_id) {
  uint32_t sz;

  if (IS_IRQ() || (thread_id == NULL)) {
    sz = 0U;
  } else {
    sz = (uint32_t)uxTaskGetStackHighWaterMark ((TaskHandle_t)thread_id);
  }

  return (sz);
}

osStatus_t osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if ((thread_id == NULL) || (priority < osPriorityIdle) || (priority > osPriorityISR)) {
    stat = osErrorParameter;
  }
  else {
    stat = osOK;
    vTaskPrioritySet ((TaskHandle_t)thread_id, (UBaseType_t)priority);
  }

  return (stat);
}

osPriority_t osThreadGetPriority (osThreadId_t thread_id) {
  osPriority_t prio;

  if (IS_IRQ() || (thread_id == NULL)) {
    prio = osPriorityError;
  } else {
    prio = (osPriority_t)uxTaskPriorityGet ((TaskHandle_t)thread_id);
  }

  return (prio);
}

osStatus_t osThreadYield (void) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  } else {
    stat = osOK;
    taskYIELD();
  }

  return (stat);
}

osStatus_t osThreadSuspend (osThreadId_t thread_id) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (thread_id == NULL) {
    stat = osErrorParameter;
  }
  else {
    stat = osOK;
    vTaskSuspend ((TaskHandle_t)thread_id);
  }

  return (stat);
}

osStatus_t osThreadResume (osThreadId_t thread_id) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (thread_id == NULL) {
    stat = osErrorParameter;
  }
  else {
    stat = osOK;
    vTaskResume ((TaskHandle_t)thread_id);
  }

  return (stat);
}

__NO_RETURN void osThreadExit (void) {
#ifndef RTE_RTOS_FreeRTOS_HEAP_1
  vTaskDelete (NULL);
#endif
  for (;;);
}

osStatus_t osThreadTerminate (osThreadId_t thread_id) {
  osStatus_t stat;
#ifndef RTE_RTOS_FreeRTOS_HEAP_1
  eTaskState tstate;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (thread_id == NULL) {
    stat = osErrorParameter;
  }
  else {
    tstate = eTaskGetState ((TaskHandle_t)thread_id);

    if (tstate != eDeleted) {
      stat = osOK;
      vTaskDelete ((TaskHandle_t)thread_id);
    } else {
      stat = osErrorResource;
    }
  }
#else
  stat = osError;
#endif

  return (stat);
}

uint32_t osThreadGetCount (void) {
  uint32_t count;

  if (IS_IRQ()) {
    count = 0U;
  } else {
    count = uxTaskGetNumberOfTasks();
  }

  return (count);
}

uint32_t osThreadEnumerate (osThreadId_t *thread_array, uint32_t array_items) {
  uint32_t i, count;
  TaskStatus_t *task;

  if (IS_IRQ() || (thread_array == NULL) || (array_items == 0U)) {
    count = 0U;
  } else {
    vTaskSuspendAll();

    count = uxTaskGetNumberOfTasks();
    task  = pvPortMalloc (count * sizeof(TaskStatus_t));

    if (task != NULL) {
      count = uxTaskGetSystemState (task, count, NULL);

      for (i = 0U; (i < count) && (i < array_items); i++) {
        thread_array[i] = (osThreadId_t)task[i].xHandle;
      }
      count = i;
    }
    (void)xTaskResumeAll();

    vPortFree (task);
  }

  return (count);
}

uint32_t osThreadFlagsSet (osThreadId_t thread_id, uint32_t flags) {
  TaskHandle_t thread = (TaskHandle_t)thread_id;

  if ((thread == NULL) || ((flags & THREAD_FLAGS_INVALID_BITS) != 0U)) {
    flags = (uint32_t)osErrorParameter;
  }
  else if (IS_IRQ()) {
    if (xTaskNotifyFromISR (thread, flags, eSetBits, NULL) != pdPASS) {
      flags = (uint32_t)osError;
    }
  }
  else {
    if (xTaskNotify (thread, flags, eSetBits) != pdPASS) {
      flags = (uint32_t)osError;
    }
  }
  /* Return flags after setting */
  return (flags);
}

uint32_t osThreadFlagsClear (uint32_t flags) {
  TaskHandle_t thread;
  uint32_t rflags, cflags;

  if (IS_IRQ()) {
    rflags = (uint32_t)osErrorISR;
  }
  else if ((flags & THREAD_FLAGS_INVALID_BITS) != 0U) {
    rflags = (uint32_t)osErrorParameter;
  }
  else {
    thread = xTaskGetCurrentTaskHandle();

    if (xTaskNotifyAndQuery (thread, 0, eNoAction, &cflags) == pdPASS) {
      rflags = cflags;
      cflags &= ~flags;

      if (xTaskNotify (thread, cflags, eSetValueWithOverwrite) != pdPASS) {
        rflags = (uint32_t)osError;
      }
    }
    else {
      rflags = (uint32_t)osError;
    }
  }

  /* Return flags before clearing */
  return (rflags);
}

uint32_t osThreadFlagsGet (void) {
  TaskHandle_t thread;
  uint32_t rflags;

  if (IS_IRQ()) {
    rflags = (uint32_t)osErrorISR;
  }
  else {
    thread = xTaskGetCurrentTaskHandle();

    if (xTaskNotifyAndQuery (thread, 0, eNoAction, &rflags) != pdPASS) {
      rflags = (uint32_t)osError;
    }
  }

  return (rflags);
}

uint32_t osThreadFlagsWait (uint32_t flags, uint32_t options, uint32_t timeout) {
  uint32_t rflags, nval;
  uint32_t clear;
  TickType_t t0;

  if (IS_IRQ()) {
    rflags = (uint32_t)osErrorISR;
  }
  else if ((flags & THREAD_FLAGS_INVALID_BITS) != 0U) {
    rflags = (uint32_t)osErrorParameter;
  }
  else {
    if ((options & osFlagsNoClear) == osFlagsNoClear) {
      clear = 0U;
    } else {
      clear = flags;
    }

    rflags = 0U;

    t0 = xTaskGetTickCount();
    do {
      if (xTaskNotifyWait (0, clear, &nval, timeout) == pdPASS) {
        rflags |= nval;

        if ((options & osFlagsWaitAll) == osFlagsWaitAll) {
          if ((flags & rflags) == flags) {
            break;
          } else {
            if (timeout == 0U) {
              rflags = (uint32_t)osErrorResource;
            }
          }
        }
        else {
          if ((flags & rflags) != 0) {
            break;
          } else {
            if (timeout == 0U) {
              rflags = (uint32_t)osErrorResource;
            }
          }
        }
      }
      else {
        if (timeout == 0) {
          rflags = (uint32_t)osErrorResource;
        } else {
          rflags = (uint32_t)osErrorTimeout;
        }
        break;
      }
    }
    while ((xTaskGetTickCount() - t0) < timeout);
  }

  /* Return flags before clearing */
  return (rflags);
}

osStatus_t osDelay (uint32_t ticks) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else {
    stat = osOK;

    if (ticks != 0U) {
      vTaskDelay(ticks);
    }
  }

  return (stat);
}

osStatus_t osDelayUntil (uint64_t ticks) {
  TickType_t tcnt;
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else {
    stat = osOK;
    tcnt = xTaskGetTickCount();

    vTaskDelayUntil (&tcnt, (TickType_t)ticks);
  }

  return (stat);
}

/*---------------------------------------------------------------------------*/

static void TimerCallback (TimerHandle_t hTimer) {
  TimerCallback_t *callb;

  callb = (TimerCallback_t *)pvTimerGetTimerID (hTimer);

  if (callb != NULL) {
    callb->func (callb->arg);
  }
}

osTimerId_t osTimerNew (osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr) {
  const char *name;
  TimerHandle_t h;
  TimerCallback_t *callb;
  UBaseType_t reload;
  int32_t mem;

  h = NULL;

  if (!IS_IRQ() && (func != NULL)) {
    /* Allocate memory to store callback function and argument */
    callb = pvPortMalloc (sizeof(TimerCallback_t));

    if (callb != NULL) {
      callb->func = func;
      callb->arg  = argument;

      if (type == osTimerOnce) {
        reload = pdFALSE;
      } else {
        reload = pdTRUE;
      }

      mem  = -1;
      name = NULL;

      if (attr != NULL) {
        if (attr->name != NULL) {
          name = attr->name;
        }

        if ((attr->cb_mem != NULL) && (attr->cb_size >= sizeof(StaticTimer_t))) {
          mem = 1;
        }
        else {
          if ((attr->cb_mem == NULL) && (attr->cb_size == 0U)) {
            mem = 0;
          }
        }
      }
      else {
        mem = 0;
      }

      if (mem == 1) {
        h = xTimerCreateStatic (name, 1, reload, callb, TimerCallback, (StaticTimer_t *)attr->cb_mem);
      }
      else {
        if (mem == 0) {
          h = xTimerCreate (name, 1, reload, callb, TimerCallback);
        }
      }
    }
  }

  return ((osTimerId_t)h);
}

const char *osTimerGetName (osTimerId_t timer_id) {
  const char *p;

  if (IS_IRQ() || (timer_id == NULL)) {
    p = NULL;
  } else {
    p = pcTimerGetName ((TimerHandle_t)timer_id);
  }

  return (p);
}

osStatus_t osTimerStart (osTimerId_t timer_id, uint32_t ticks) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (timer_id == NULL) {
    stat = osErrorParameter;
  }
  else {
    if (xTimerChangePeriod ((TimerHandle_t)timer_id, ticks, 0) == pdPASS) {
      stat = osOK;
    } else {
      stat = osErrorResource;
    }
  }

  return (stat);
}

osStatus_t osTimerStop (osTimerId_t timer_id) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (timer_id == NULL) {
    stat = osErrorParameter;
  }
  else {
    if (xTimerIsTimerActive ((TimerHandle_t)timer_id) == pdFALSE) {
      stat = osErrorResource;
    }
    else {
      if (xTimerStop ((TimerHandle_t)timer_id, 0) == pdPASS) {
        stat = osOK;
      } else {
        stat = osError;
      }
    }
  }

  return (stat);
}

uint32_t osTimerIsRunning (osTimerId_t timer_id) {
  uint32_t running;

  if (IS_IRQ() || (timer_id == NULL)) {
    running = 0U;
  } else {
    running = (uint32_t)xTimerIsTimerActive ((TimerHandle_t)timer_id);
  }

  return (running);
}

osStatus_t osTimerDelete (osTimerId_t timer_id) {
  osStatus_t stat;
#ifndef RTE_RTOS_FreeRTOS_HEAP_1
  TimerCallback_t *callb;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (timer_id == NULL) {
    stat = osErrorParameter;
  }
  else {
    callb = (TimerCallback_t *)pvTimerGetTimerID ((TimerHandle_t)timer_id);

    if (xTimerDelete ((TimerHandle_t)timer_id, 0) == pdPASS) {
      vPortFree (callb);
      stat = osOK;
    } else {
      stat = osErrorResource;
    }
  }
#else
  stat = osError;
#endif

  return (stat);
}

/*---------------------------------------------------------------------------*/

osEventFlagsId_t osEventFlagsNew (const osEventFlagsAttr_t *attr) {
  EventGroupHandle_t h;
  int32_t mem;

  h = NULL;

  if (!IS_IRQ()) {
    mem = -1;

    if (attr != NULL) {
      if ((attr->cb_mem != NULL) && (attr->cb_size >= sizeof(StaticEventGroup_t))) {
        mem = 1;
      }
      else {
        if ((attr->cb_mem == NULL) && (attr->cb_size == 0U)) {
          mem = 0;
        }
      }
    }
    else {
      mem = 0;
    }

    if (mem == 1) {
      h = xEventGroupCreateStatic (attr->cb_mem);
    }
    else {
      if (mem == 0) {
        h = xEventGroupCreate();
      }
    }
  }

  return ((osEventFlagsId_t)h);
}

uint32_t osEventFlagsSet (osEventFlagsId_t ef_id, uint32_t flags) {
  uint32_t rflags;

  if ((ef_id == NULL) || ((flags & EVENT_FLAGS_INVALID_BITS) != 0U)) {
    rflags = (uint32_t)osErrorParameter;
  }
  else if (IS_IRQ()) {
    if (xEventGroupSetBitsFromISR ((EventGroupHandle_t)ef_id, (EventBits_t)flags, NULL) == pdPASS) {
      rflags = flags;
    } else {
      rflags = (uint32_t)osErrorResource;
    }
  }
  else {
    rflags = xEventGroupSetBits ((EventGroupHandle_t)ef_id, (EventBits_t)flags);
  }

  return (rflags);
}

uint32_t osEventFlagsClear (osEventFlagsId_t ef_id, uint32_t flags) {
  uint32_t rflags;

  if ((ef_id == NULL) || ((flags & EVENT_FLAGS_INVALID_BITS) != 0U)) {
    rflags = (uint32_t)osErrorParameter;
  }
  else if (IS_IRQ()) {
    rflags = xEventGroupGetBitsFromISR ((EventGroupHandle_t)ef_id);

    if (xEventGroupClearBitsFromISR ((EventGroupHandle_t)ef_id, (EventBits_t)flags) == pdFAIL) {
      rflags = (uint32_t)osErrorResource;
    }
  }
  else {
    rflags = xEventGroupClearBits ((EventGroupHandle_t)ef_id, (EventBits_t)flags);
  }

  return (rflags);
}

uint32_t osEventFlagsGet (osEventFlagsId_t ef_id) {
  uint32_t rflags;

  if (ef_id == NULL) {
    rflags = 0U;
  }
  else if (IS_IRQ()) {
    rflags = xEventGroupGetBitsFromISR ((EventGroupHandle_t)ef_id);
  }
  else {
    rflags = xEventGroupGetBits ((EventGroupHandle_t)ef_id);
  }

  return (rflags);
}

uint32_t osEventFlagsWait (osEventFlagsId_t ef_id, uint32_t flags, uint32_t options, uint32_t timeout) {
  BaseType_t wait_all;
  BaseType_t exit_clr;
  uint32_t rflags;

  if ((ef_id == NULL) || ((flags & EVENT_FLAGS_INVALID_BITS) != 0U)) {
    rflags = (uint32_t)osErrorParameter;
  }
  else if (IS_IRQ()) {
    rflags = (uint32_t)osErrorISR;
  }
  else {
    if (options & osFlagsWaitAll) {
      wait_all = pdTRUE;
    } else {
      wait_all = pdFAIL;
    }

    if (options & osFlagsNoClear) {
      exit_clr = pdFAIL;
    } else {
      exit_clr = pdTRUE;
    }

    rflags = xEventGroupWaitBits ((EventGroupHandle_t)ef_id, (EventBits_t)flags, exit_clr,
                                                                                 wait_all,
                                                                                 (TickType_t)timeout);
    if (options & osFlagsWaitAll) {
      if (flags != rflags) {
        if (timeout > 0U) {
          rflags = (uint32_t)osErrorTimeout;
        } else {
          rflags = (uint32_t)osErrorResource;
        }
      }
    }
    else {
      if ((flags & rflags) == 0U) {
        if (timeout > 0U) {
          rflags = (uint32_t)osErrorTimeout;
        } else {
          rflags = (uint32_t)osErrorResource;
        }
      }
    }
  }

  return (rflags);
}

osStatus_t osEventFlagsDelete (osEventFlagsId_t ef_id) {
  osStatus_t stat;

#ifndef RTE_RTOS_FreeRTOS_HEAP_1
  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (ef_id == NULL) {
    stat = osErrorParameter;
  }
  else {
    stat = osOK;
    vEventGroupDelete ((EventGroupHandle_t)ef_id);
  }
#else
  stat = osError;
#endif

  return (stat);
}

/*---------------------------------------------------------------------------*/

osMutexId_t osMutexNew (const osMutexAttr_t *attr) {
  SemaphoreHandle_t mutex;
  uint32_t type;
  uint32_t rmtx;
  int32_t  mem;

  mutex = NULL;

  if (!IS_IRQ()) {
    if (attr != NULL) {
      type = attr->attr_bits;
    } else {
      type = 0U;
    }

    if ((type & osMutexRecursive) == osMutexRecursive) {
      rmtx = 1U;
    } else {
      rmtx = 0U;
    }

    if ((type & osMutexRobust) != osMutexRobust) {
      mem = -1;

      if (attr != NULL) {
        if ((attr->cb_mem != NULL) && (attr->cb_size >= sizeof(StaticSemaphore_t))) {
          mem = 1;
        }
        else {
          if ((attr->cb_mem == NULL) && (attr->cb_size == 0U)) {
            mem = 0;
          }
        }
      }
      else {
        mem = 0;
      }

      if (mem == 1) {
        if (rmtx != 0U) {
          mutex = xSemaphoreCreateRecursiveMutexStatic (attr->cb_mem);
        }
        else {
          mutex = xSemaphoreCreateMutexStatic (attr->cb_mem);
        }
      }
      else {
        if (mem == 0) {
          if (rmtx != 0U) {
            mutex = xSemaphoreCreateRecursiveMutex ();
          } else {
            mutex = xSemaphoreCreateMutex ();
          }
        }
      }

      if ((mutex != NULL) && (rmtx != 0U)) {
        mutex = (SemaphoreHandle_t)((uint32_t)mutex | 1U);
      }
    }
  }

  return ((osMutexId_t)mutex);
}

osStatus_t osMutexAcquire (osMutexId_t mutex_id, uint32_t timeout) {
  SemaphoreHandle_t mutex;
  osStatus_t stat;
  uint32_t rmtx;

  mutex = (SemaphoreHandle_t)((uint32_t)mutex_id & ~1U);

  rmtx = (uint32_t)mutex_id & 1U;

  stat = osOK;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (mutex == NULL) {
    stat = osErrorParameter;
  }
  else {
    if (rmtx != 0U) {
      if (xSemaphoreTakeRecursive (mutex, timeout) != pdPASS) {
        if (timeout != 0U) {
          stat = osErrorTimeout;
        } else {
          stat = osErrorResource;
        }
      }
    }
    else {
      if (xSemaphoreTake (mutex, timeout) != pdPASS) {
        if (timeout != 0U) {
          stat = osErrorTimeout;
        } else {
          stat = osErrorResource;
        }
      }
    }
  }

  return (stat);
}

osStatus_t osMutexRelease (osMutexId_t mutex_id) {
  SemaphoreHandle_t mutex;
  osStatus_t stat;
  uint32_t rmtx;

  mutex = (SemaphoreHandle_t)((uint32_t)mutex_id & ~1U);

  rmtx = (uint32_t)mutex_id & 1U;

  stat = osOK;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (mutex == NULL) {
    stat = osErrorParameter;
  }
  else {
    if (rmtx != 0U) {
      if (xSemaphoreGiveRecursive (mutex) != pdPASS) {
        stat = osErrorResource;
      }
    }
    else {
      if (xSemaphoreGive (mutex) != pdPASS) {
        stat = osErrorResource;
      }
    }
  }

  return (stat);
}

osThreadId_t osMutexGetOwner (osMutexId_t mutex_id) {
  SemaphoreHandle_t mutex;
  osThreadId_t owner;

  mutex = (SemaphoreHandle_t)((uint32_t)mutex_id & ~1U);

  if (IS_IRQ() || (mutex == NULL)) {
    owner = NULL;
  } else {
    owner = (osThreadId_t)xSemaphoreGetMutexHolder (mutex);
  }

  return (owner);
}

osStatus_t osMutexDelete (osMutexId_t mutex_id) {
  osStatus_t stat;
#ifndef RTE_RTOS_FreeRTOS_HEAP_1
  SemaphoreHandle_t mutex;

  mutex = (SemaphoreHandle_t)((uint32_t)mutex_id & ~1U);

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (mutex == NULL) {
    stat = osErrorParameter;
  }
  else {
    stat = osOK;
    vSemaphoreDelete (mutex);
  }
#else
  stat = osError;
#endif

  return (stat);
}

/*---------------------------------------------------------------------------*/

osSemaphoreId_t osSemaphoreNew (uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t *attr) {
  SemaphoreHandle_t h;
  int32_t mem;

  h = NULL;

  if (!IS_IRQ() && (max_count > 0U) && (initial_count <= max_count)) {
    mem = -1;

    if (attr != NULL) {
      if ((attr->cb_mem != NULL) && (attr->cb_size >= sizeof(StaticSemaphore_t))) {
        mem = 1;
      }
      else {
        if ((attr->cb_mem == NULL) && (attr->cb_size == 0U)) {
          mem = 0;
        }
      }
    }
    else {
      mem = 0;
    }

    if (mem != -1) {
      if (max_count == 1U) {
        if (mem == 1) {
          h = xSemaphoreCreateBinaryStatic ((StaticSemaphore_t *)attr->cb_mem);
        }
        else {
          h = xSemaphoreCreateBinary();
        }

        if ((h != NULL) && (initial_count != 0U)) {
          if (xSemaphoreGive (h) != pdPASS) {
            vSemaphoreDelete (h);
            h = NULL;
          }
        }
      }
      else {
        if (mem == 1) {
          h = xSemaphoreCreateCountingStatic (max_count, initial_count, (StaticSemaphore_t *)attr->cb_mem);
        }
        else {
          h = xSemaphoreCreateCounting (max_count, initial_count);
        }
      }
    }
  }

  return (h);
}

osStatus_t osSemaphoreAcquire (osSemaphoreId_t semaphore_id, uint32_t timeout) {
  osStatus_t stat;

  stat = osOK;

  if (semaphore_id == NULL) {
    stat = osErrorParameter;
  }
  else if (IS_IRQ()) {
    if (timeout != 0U) {
      stat = osErrorParameter;
    }
    else {
      if (xSemaphoreTakeFromISR ((SemaphoreHandle_t)semaphore_id, NULL) != pdPASS) {
        stat = osErrorResource;
      }
    }
  }
  else {
    if (xSemaphoreTake ((SemaphoreHandle_t)semaphore_id, (TickType_t)timeout) != pdPASS) {
      if (timeout != 0U) {
        stat = osErrorTimeout;
      } else {
        stat = osErrorResource;
      }
    }
  }

  return (stat);
}

osStatus_t osSemaphoreRelease (osSemaphoreId_t semaphore_id) {
  osStatus_t stat;

  stat = osOK;

  if (semaphore_id == NULL) {
    stat = osErrorParameter;
  }
  else if (IS_IRQ()) {
    if (xSemaphoreGiveFromISR ((SemaphoreHandle_t)semaphore_id, NULL) != pdTRUE) {
      stat = osErrorResource;
    }
  }
  else {
    if (xSemaphoreGive ((SemaphoreHandle_t)semaphore_id) != pdPASS) {
      stat = osErrorResource;
    }
  }

  return (stat);
}

uint32_t osSemaphoreGetCount (osSemaphoreId_t semaphore_id) {
  uint32_t count;

  if (semaphore_id == NULL) {
    count = 0U;
  }
  else if (IS_IRQ()) {
    count = uxQueueMessagesWaitingFromISR ((QueueHandle_t)semaphore_id);
  } else {
    count = (uint32_t)uxSemaphoreGetCount ((SemaphoreHandle_t)semaphore_id);
  }

  return (count);
}

osStatus_t osSemaphoreDelete (osSemaphoreId_t semaphore_id) {
  osStatus_t stat;

#ifndef RTE_RTOS_FreeRTOS_HEAP_1
  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (semaphore_id == NULL) {
    stat = osErrorParameter;
  }
  else {
    stat = osOK;
    vSemaphoreDelete ((SemaphoreHandle_t)semaphore_id);
  }
#else
  stat = osError;
#endif

  return (stat);
}

/*---------------------------------------------------------------------------*/

osMessageQueueId_t osMessageQueueNew (uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr) {
  QueueHandle_t h;
  int32_t mem;

  h = NULL;

  if (!IS_IRQ() && (msg_count > 0U) && (msg_size > 0U)) {
    mem = -1;

    if (attr != NULL) {
      if ((attr->cb_mem != NULL) && (attr->cb_size >= sizeof(StaticQueue_t)) &&
          (attr->mq_mem != NULL) && (attr->mq_size >= (msg_count * msg_size))) {
        mem = 1;
      }
      else {
        if ((attr->cb_mem == NULL) && (attr->cb_size == 0U) &&
            (attr->mq_mem == NULL) && (attr->mq_size == 0U)) {
          mem = 0;
        }
      }
    }
    else {
      mem = 0;
    }

    if (mem == 1) {
      h = xQueueCreateStatic (msg_count, msg_size, attr->mq_mem, attr->cb_mem);
    }
    else {
      if (mem == 0) {
        h = xQueueCreate (msg_count, msg_size);
      }
    }
  }

  return ((osMessageQueueId_t)h);
}

osStatus_t osMessageQueuePut (osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout) {
  osStatus_t stat;

  (void)msg_prio; /* Message priority is ignored */

  stat = osOK;

  if (IS_IRQ()) {
    if ((mq_id == NULL) || (msg_ptr == NULL) || (timeout != 0U)) {
      stat = osErrorParameter;
    }
    else {
      if (xQueueSendToBackFromISR ((QueueHandle_t)mq_id, msg_ptr, NULL) != pdTRUE) {
        stat = osErrorResource;
      }
    }
  }
  else {
    if ((mq_id == NULL) || (msg_ptr == NULL)) {
      stat = osErrorParameter;
    }
    else {
      if (xQueueSendToBack ((QueueHandle_t)mq_id, msg_ptr, (TickType_t)timeout) != pdPASS) {
        if (timeout != 0U) {
          stat = osErrorTimeout;
        } else {
          stat = osErrorResource;
        }
      }
    }
  }

  return (stat);
}

osStatus_t osMessageQueueGet (osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout) {
  osStatus_t stat;

  (void)msg_prio; /* Message priority is ignored */

  stat = osOK;

  if (IS_IRQ()) {
    if ((mq_id == NULL) || (msg_ptr == NULL) || (timeout != 0U)) {
      stat = osErrorParameter;
    }
    else {
      if (xQueueReceiveFromISR ((QueueHandle_t)mq_id, msg_ptr, NULL) != pdPASS) {
        stat = osErrorResource;
      }
    }
  }
  else {
    if ((mq_id == NULL) || (msg_ptr == NULL)) {
      stat = osErrorParameter;
    }
    else {
      if (xQueueReceive ((QueueHandle_t)mq_id, msg_ptr, (TickType_t)timeout) != pdPASS) {
        if (timeout != 0U) {
          stat = osErrorTimeout;
        } else {
          stat = osErrorResource;
        }
      }
    }
  }

  return (stat);
}

uint32_t osMessageQueueGetCapacity (osMessageQueueId_t mq_id) {
  StaticQueue_t *mq = (StaticQueue_t *)mq_id;
  uint32_t capacity;

  if (mq == NULL) {
    capacity = 0U;
  } else {
    /* capacity = pxQueue->uxLength */
    capacity = mq->uxDummy4[1];
  }

  return (capacity);
}

uint32_t osMessageQueueGetMsgSize (osMessageQueueId_t mq_id) {
  StaticQueue_t *mq = (StaticQueue_t *)mq_id;
  uint32_t size;

  if (mq == NULL) {
    size = 0U;
  } else {
    /* size = pxQueue->uxItemSize */
    size = mq->uxDummy4[2];
  }

  return (size);
}

uint32_t osMessageQueueGetCount (osMessageQueueId_t mq_id) {
  UBaseType_t count;

  if (mq_id == NULL) {
    count = 0U;
  }
  else if (IS_IRQ()) {
    count = uxQueueMessagesWaitingFromISR ((QueueHandle_t)mq_id);
  }
  else {
    count = uxQueueMessagesWaiting ((QueueHandle_t)mq_id);
  }

  return ((uint32_t)count);
}

uint32_t osMessageQueueGetSpace (osMessageQueueId_t mq_id) {
  StaticQueue_t *mq = (StaticQueue_t *)mq_id;
  uint32_t space;
  uint32_t isrm;

  if (mq == NULL) {
    space = 0U;
  }
  else if (IS_IRQ()) {
    isrm = taskENTER_CRITICAL_FROM_ISR();

    /* space = pxQueue->uxLength - pxQueue->uxMessagesWaiting; */
    space = mq->uxDummy4[1] - mq->uxDummy4[0];

    taskEXIT_CRITICAL_FROM_ISR(isrm);
  }
  else {
    space = (uint32_t)uxQueueSpacesAvailable ((QueueHandle_t)mq);
  }

  return (space);
}

osStatus_t osMessageQueueReset (osMessageQueueId_t mq_id) {
  osStatus_t stat;

  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (mq_id == NULL) {
    stat = osErrorParameter;
  }
  else {
    stat = osOK;
    (void)xQueueReset ((QueueHandle_t)mq_id);
  }

  return (stat);
}

osStatus_t osMessageQueueDelete (osMessageQueueId_t mq_id) {
  osStatus_t stat;

#ifndef RTE_RTOS_FreeRTOS_HEAP_1
  if (IS_IRQ()) {
    stat = osErrorISR;
  }
  else if (mq_id == NULL) {
    stat = osErrorParameter;
  }
  else {
    stat = osOK;
    vQueueDelete ((QueueHandle_t)mq_id);
  }
#else
  stat = osError;
#endif

  return (stat);
}

/*---------------------------------------------------------------------------*/

/* Callback function prototypes */
extern void vApplicationIdleHook (void);
extern void vApplicationTickHook (void);
extern void vApplicationMallocFailedHook (void);
extern void vApplicationDaemonTaskStartupHook (void);
extern void vApplicationStackOverflowHook (TaskHandle_t xTask, signed char *pcTaskName);

/**
  Dummy implementation of the callback function vApplicationIdleHook().
*/
#if (configUSE_IDLE_HOOK == 1)
__WEAK void vApplicationIdleHook (void){}
#endif

/**
  Dummy implementation of the callback function vApplicationTickHook().
*/
#if (configUSE_TICK_HOOK == 1)
 __WEAK void vApplicationTickHook (void){}
#endif

/**
  Dummy implementation of the callback function vApplicationMallocFailedHook().
*/
#if (configUSE_MALLOC_FAILED_HOOK == 1)
__WEAK void vApplicationMallocFailedHook (void){}
#endif

/**
  Dummy implementation of the callback function vApplicationDaemonTaskStartupHook().
*/
#if (configUSE_DAEMON_TASK_STARTUP_HOOK == 1)
__WEAK void vApplicationDaemonTaskStartupHook (void){}
#endif

/**
  Dummy implementation of the callback function vApplicationStackOverflowHook().
*/
#if (configCHECK_FOR_STACK_OVERFLOW > 0)
__WEAK void vApplicationStackOverflowHook (TaskHandle_t xTask, signed char *pcTaskName) {
  (void)xTask;
  (void)pcTaskName;
}
#endif

/*---------------------------------------------------------------------------*/

/* External Idle and Timer task static memory allocation functions */
extern void vApplicationGetIdleTaskMemory  (StaticTask_t **ppxIdleTaskTCBBuffer,  StackType_t **ppxIdleTaskStackBuffer,  uint32_t *pulIdleTaskStackSize);
extern void vApplicationGetTimerTaskMemory (StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);

/* Idle task control block and stack */
static StaticTask_t Idle_TCB;
static StackType_t  Idle_Stack[configMINIMAL_STACK_SIZE];

/* Timer task control block and stack */
static StaticTask_t Timer_TCB;
static StackType_t  Timer_Stack[configTIMER_TASK_STACK_DEPTH];

/*
  vApplicationGetIdleTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
void vApplicationGetIdleTaskMemory (StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer   = &Idle_TCB;
  *ppxIdleTaskStackBuffer = &Idle_Stack[0];
  *pulIdleTaskStackSize   = (uint32_t)configMINIMAL_STACK_SIZE;
}

/*
  vApplicationGetTimerTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
void vApplicationGetTimerTaskMemory (StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer   = &Timer_TCB;
  *ppxTimerTaskStackBuffer = &Timer_Stack[0];
  *pulTimerTaskStackSize   = (uint32_t)configTIMER_TASK_STACK_DEPTH;
}
