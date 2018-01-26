/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2017 Arm Limited. All rights reserved.
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
 *      Name:    clib_arm.c
 *      Purpose: ARM C library startup and initialization support
 *
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include <stddef.h>

#include "cmsis_os2.h"
#include "cmsis_compiler.h"

/* Define the number of Threads which use standard C/C++ library libspace */
#ifndef OS_THREAD_LIBSPACE_NUM
  #define OS_THREAD_LIBSPACE_NUM      4
#endif

/*----------------------------------------------------------------------------*/

/* Initialize OS */
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

#ifndef __MICROLIB
__WEAK
void _platform_post_stackheap_init (void);
void _platform_post_stackheap_init (void) {
  osKernelInitialize();
}
#endif /* __MICROLIB */

#elif defined(__GNUC__)
__WEAK
void software_init_hook (void);
void software_init_hook (void) {
  osKernelInitialize();
}

#endif

/*----------------------------------------------------------------------------*/

/* C/C++ Standard Library Multithreading Interface */
#if (( defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))) && !defined(__MICROLIB))

#define LIBSPACE_SIZE 96

/* Libspace memory pool */
static uint32_t os_libspace[OS_THREAD_LIBSPACE_NUM+1][LIBSPACE_SIZE/sizeof(uint32_t)];

/* Array of Threads (IDs) using libspace */
static osThreadId_t os_libspace_id[OS_THREAD_LIBSPACE_NUM];

/* OS Kernel state checking */
static uint32_t os_kernel_is_active (void) {
  static uint8_t os_kernel_active = 0U;

  if (os_kernel_active == 0U) {
    if (osKernelGetState() > osKernelReady) {
      os_kernel_active = 1U;
      return 1U;
    }
    return 0U;
  } else {
    return 1U;
  }
}

/* Provide libspace for current thread */
void *__user_perthread_libspace (void) {
  osThreadId_t id;
  uint32_t     n;

  if (!os_kernel_is_active()) {
    return (void *)&os_libspace[OS_THREAD_LIBSPACE_NUM][0];
  }

  id = osThreadGetId();
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

/*----------------------------------------------------------------------------*/

/* Define mutex object and function prototypes */
typedef void *mutex;

__USED int  _mutex_initialize(mutex *m);
__USED void _mutex_acquire   (mutex *m);
__USED void _mutex_release   (mutex *m);
__USED void _mutex_free      (mutex *m);


/* Initialize mutex */
int _mutex_initialize(mutex *m) {
  *m = osMutexNew(NULL);
  if (*m == NULL) {
    return 0;
  }
  return 1;
}

/* Acquire mutex */
void _mutex_acquire(mutex *m) {
  if (os_kernel_is_active()) {
    osMutexAcquire(*m, osWaitForever);
  }
}

/* Release mutex */
void _mutex_release(mutex *m) {
  if (os_kernel_is_active()) {
    osMutexRelease(*m);
  }
}

/* // Free mutex */
void _mutex_free(mutex *m) {
  osMutexDelete(*m);
}

#endif
