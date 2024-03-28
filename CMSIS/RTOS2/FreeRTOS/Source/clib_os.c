/* -------------------------------------------------------------------------- 
 * Copyright (c) 2024 Arm Limited. All rights reserved.
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
 *      Name:    clib_os.c
 *      Purpose: C library startup hooks (unless handled by CMSIS-Compiler)
 *
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include <stddef.h>

#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

/* Event Recorder initialization before entering function "main" */
#if (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) && !defined(__MICROLIB))
extern void _platform_post_stackheap_init (void);
__WEAK void _platform_post_stackheap_init (void) {
  /* Initialize Event Recorder and apply initial filter configuration */
  #if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
    EvrFreeRTOSSetup(0);
  #endif
}
#elif defined(__GNUC__)
extern void software_init_hook (void);
__WEAK void software_init_hook (void) {
  /* Initialize Event Recorder and apply initial filter configuration */
  #if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
    EvrFreeRTOSSetup(0);
  #endif
}

#elif defined(__ICCARM__)
extern void $Super$$__iar_data_init3 (void);
void $Sub$$__iar_data_init3 (void) {
  $Super$$__iar_data_init3();
  /* Initialize Event Recorder and apply initial filter configuration */
  #if defined(RTE_Compiler_EventRecorder) || defined(RTE_CMSIS_View_EventRecorder)
    EvrFreeRTOSSetup(0);
  #endif
}

#endif

#if ((defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) && !defined(__MICROLIB))
/* Arm C/C++ Standard Library Interfaces */

#if (!defined(CLIB_NO_FP_INIT) && \
     !defined(__ARM_ARCH_7A__) && \
     (defined(__FPU_PRESENT) && (__FPU_PRESENT == 1U)) && \
     (defined(__FPU_USED   ) && (__FPU_USED    == 1U)))

/* Floating-point Initialization */
extern void $Super$$_fp_init (void);

void $Sub$$_fp_init (void);
void $Sub$$_fp_init (void) {
  $Super$$_fp_init();
  FPU->FPDSCR = __get_FPSCR();
}

#endif

#if (!defined(RTE_CMSIS_Compiler_OS_Interface_RTOS2_LIBSPACE) && \
     !defined(RTE_CMSIS_Compiler_OS_Interface_RTOS2_LOCKS))

/* OS Kernel state checking */
static uint32_t os_kernel_is_active (void) {
  if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
    return 1U;
  } else {
    return 0U;
  }
}

#endif

#ifndef RTE_CMSIS_Compiler_OS_Interface_RTOS2_LIBSPACE

/* Define the number of Threads which use standard C/C++ library libspace */
#ifndef OS_THREAD_LIBSPACE_NUM
  #define OS_THREAD_LIBSPACE_NUM      4
#endif

#define LIBSPACE_SIZE 96

/* Libspace memory pool */
static uint32_t os_libspace[OS_THREAD_LIBSPACE_NUM+1][LIBSPACE_SIZE/sizeof(uint32_t)];

/* Array of Threads (IDs) using libspace */
static TaskHandle_t os_libspace_id[OS_THREAD_LIBSPACE_NUM];

/* Provide libspace for current thread */
void *__user_perthread_libspace (void);
void *__user_perthread_libspace (void) {
  TaskHandle_t id;
  uint32_t     n;

  if (!os_kernel_is_active()) {
    return (void *)&os_libspace[OS_THREAD_LIBSPACE_NUM][0];
  }

  id = xTaskGetCurrentTaskHandle();

  for (n = 0U; n < OS_THREAD_LIBSPACE_NUM; n++) {
    if (os_libspace_id[n] == NULL) {

      os_libspace_id[n] = id;

      return (void *)&os_libspace[n][0];
    }
    if (os_libspace_id[n] == id) {
      return (void *)&os_libspace[n][0];
    }
  }

  return (void *)&os_libspace[n][0];
}

#endif /* RTE_CMSIS_Compiler_OS_Interface_RTOS2_LIBSPACE */

#ifndef RTE_CMSIS_Compiler_OS_Interface_RTOS2_LOCKS

#if (__ARM_ARCH_7A__ == 1U)
/* CPSR mode bitmasks */
#define CPSR_MODE_USER            0x10U
#define CPSR_MODE_SYSTEM          0x1FU

#define IS_IRQ_MODE()             ((__get_mode() != CPSR_MODE_USER) && (__get_mode() != CPSR_MODE_SYSTEM))
#else
#define IS_IRQ_MODE()             (__get_IPSR() != 0U)
#endif

/* Define the number of Mutexes used by standard C/C++ library for stream protection */
#ifndef OS_MUTEX_CLIB_NUM
  #define OS_MUTEX_CLIB_NUM           5
#endif

#if (OS_MUTEX_CLIB_NUM > 0)
static StaticSemaphore_t clib_mutex_cb[OS_MUTEX_CLIB_NUM];
static SemaphoreHandle_t clib_mutex_id[OS_MUTEX_CLIB_NUM];
#endif

/* Define mutex object and function prototypes */
typedef void *mutex;

__USED int  _mutex_initialize(mutex *m);
__USED void _mutex_acquire   (mutex *m);
__USED void _mutex_release   (mutex *m);
__USED void _mutex_free      (mutex *m);

/* Check if processor is in Thread or Handler mode */
static uint32_t is_thread_mode (void) {
  if (IS_IRQ_MODE() != 0) {
    return 1U; /* Thread mode  */
  } else {
    return 0U; /* Handler mode */
  }
}

/* Initialize mutex */
int _mutex_initialize(mutex *m) {
#if (OS_MUTEX_CLIB_NUM > 0)
  uint32_t i;
#endif

  *m = NULL;

#if (OS_MUTEX_CLIB_NUM > 0)
  for (i = 0U; i < OS_MUTEX_CLIB_NUM; i++) {
    if (clib_mutex_id[i] == NULL) {
      /* Create mutex using static memory */
      clib_mutex_id[i] = xSemaphoreCreateMutexStatic(&clib_mutex_cb[i]);

      /* Return mutex id */
      *m = clib_mutex_id[i];
      break;
    }
  }
#endif
  if ((*m == NULL) && (os_kernel_is_active())) {
    /* Create mutex using dynamic memory */
    *m = xSemaphoreCreateMutex();
  }

  /* FreeRTOS disables interrupts when its API is called before the kernel is started. */
  /* This is pre-main context and since interrupts shall not happen before reaching    */
  /* main we can re-enable interrupts and have consistent state when main gets called. */
  portENABLE_INTERRUPTS();

  if (*m == NULL) {
    return 0;
  }
  return 1;
}

/* Acquire mutex */
void _mutex_acquire(mutex *m) {
  /* Don't allow mutex operations when the kernel is not switching tasks and also   */
  /* block protection when in interrupt. Using stdio streams in interrupt is bad    */
  /* practice, but some applications call printf as last resort for debug purposes. */
  if (os_kernel_is_active() && is_thread_mode()) {
    xSemaphoreTake(*m, portMAX_DELAY);
  }
}

/* Release mutex */
void _mutex_release(mutex *m) {

  if (os_kernel_is_active() && is_thread_mode()) {
    xSemaphoreGive(*m);
  }
}

/* Free mutex */
void _mutex_free(mutex *m) {
#if (OS_MUTEX_CLIB_NUM > 0)
  uint32_t i;
#endif

  vSemaphoreDelete(*m);

#if (OS_MUTEX_CLIB_NUM > 0)
  /* Check if mutex was using static memory */
  for (i = 0U; i < OS_MUTEX_CLIB_NUM; i++) {
    if (*m == clib_mutex_id[i]) {
      clib_mutex_id[i] = NULL;
      break;
    }
  }
#endif
}

#endif /* RTE_CMSIS_Compiler_OS_Interface_RTOS2_LOCKS */
#endif
