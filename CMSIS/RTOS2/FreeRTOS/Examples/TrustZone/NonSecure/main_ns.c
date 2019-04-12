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
 *      Name:    main_ns.c
 *      Purpose: TrustZone Non-Secure Domain example program
 *
 *---------------------------------------------------------------------------*/

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core

/* Secure functions callable from the non-secure application */
#include "library_nsc.h"


/* Non-secure counters */
uint32_t Count_NS;
uint32_t Count_S;

/* Callback function called from the secure domain */
static void Callback_NS (void) {
  Count_NS += 1U;
}

/* Application main thread (non-secure) */
static void app_main (void *arg) {
  uint32_t count_s;

  /* Allocate secure contex for this task (because it calls secure side) */
  portALLOCATE_SECURE_CONTEXT (configMINIMAL_SECURE_STACK_SIZE);

  /* Initialize counters */
  Count_NS = 0U;
  Count_S  = 0U;

  for (;;) {

    /* Call secure function */
    count_s = Func_NSC(Callback_NS);

    osDelay(1000);

    /* Store returned (secure) counter value */
    Count_S = count_s;
  }
}


int main (void) {

  // System Initialization
  SystemCoreClockUpdate();

  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
  if (osKernelGetState() == osKernelReady) {
    osKernelStart();                    // Start thread execution
  }

  for(;;) {}
}