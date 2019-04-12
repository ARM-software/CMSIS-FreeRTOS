/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
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
 *      Name:    Blinky.c
 *      Purpose: Example program
 *
 *---------------------------------------------------------------------------*/

#include <stdio.h>

#include "RTE_Components.h"             // Component selection
#include CMSIS_device_header

#include "FreeRTOS.h"                   // Keil::RTOS:FreeRTOS:Core
#include "task.h"                       // Keil::RTOS:FreeRTOS:Core

TaskHandle_t tid_phaseA;                /* Thread id of thread: phase_a      */
TaskHandle_t tid_phaseB;                /* Thread id of thread: phase_b      */
TaskHandle_t tid_phaseC;                /* Thread id of thread: phase_c      */
TaskHandle_t tid_phaseD;                /* Thread id of thread: phase_d      */
TaskHandle_t tid_clock;                 /* Thread id of thread: clock        */

struct phases_t {
  int_fast8_t phaseA;
  int_fast8_t phaseB;
  int_fast8_t phaseC;
  int_fast8_t phaseD;
} g_phases;


/*----------------------------------------------------------------------------
 *      Function 'signal_func' called from multiple threads
 *---------------------------------------------------------------------------*/
void signal_func (TaskHandle_t tid)  {
  xTaskNotify (tid_clock, 0x0100, eSetBits);/* set signal to clock thread    */
  vTaskDelay (500);                         /* delay 500ms                   */
  xTaskNotify (tid_clock, 0x0100, eSetBits);/* set signal to clock thread    */
  vTaskDelay (500);                         /* delay 500ms                   */
  xTaskNotify (tid, 0x0001, eSetBits);      /* set signal to thread 'thread' */
  vTaskDelay (500);                         /* delay 500ms                   */
}

/*----------------------------------------------------------------------------
 *      Thread 1 'phaseA': Phase A output
 *---------------------------------------------------------------------------*/
void phaseA (void *argument) {
  for (;;) {
    xTaskNotifyWait (0, 0x0001, NULL, portMAX_DELAY); /* wait for an thread flag */
    g_phases.phaseA = 1;
    signal_func(tid_phaseB);                          /* call signal function    */
    g_phases.phaseA = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 2 'phaseB': Phase B output
 *---------------------------------------------------------------------------*/
void phaseB (void *argument) {
  for (;;) {
    xTaskNotifyWait (0, 0x0001, NULL, portMAX_DELAY); /* wait for an thread flag */
    g_phases.phaseB = 1;
    signal_func(tid_phaseC);                          /* call signal function    */
    g_phases.phaseB = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 3 'phaseC': Phase C output
 *---------------------------------------------------------------------------*/
void phaseC (void *argument) {
  for (;;) {
    xTaskNotifyWait (0, 0x0001, NULL, portMAX_DELAY); /* wait for an thread flag */
    g_phases.phaseC = 1;
    signal_func(tid_phaseD);                          /* call signal function    */
    g_phases.phaseC = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 4 'phaseD': Phase D output
 *---------------------------------------------------------------------------*/
void phaseD (void *argument) {
  for (;;) {
    xTaskNotifyWait (0, 0x0001, NULL, portMAX_DELAY); /* wait for an thread flag */
    g_phases.phaseD = 1;
    signal_func(tid_phaseA);                          /* call signal function    */
    g_phases.phaseD = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 5 'clock': Signal Clock
 *---------------------------------------------------------------------------*/
void clock (void *argument) {
  for (;;) {
    xTaskNotifyWait (0, 0x0100, NULL, portMAX_DELAY); /* wait for an thread flag */
    vTaskDelay(80);                                   /* delay 80ms              */
  }
}

/*----------------------------------------------------------------------------
 *      Initialize and start threads
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {

  xTaskCreate (phaseA, "PhaseA", 64, NULL, tskIDLE_PRIORITY+1, &tid_phaseA);
  xTaskCreate (phaseB, "PhaseB", 64, NULL, tskIDLE_PRIORITY+1, &tid_phaseB);
  xTaskCreate (phaseC, "PhaseC", 64, NULL, tskIDLE_PRIORITY+1, &tid_phaseC);
  xTaskCreate (phaseD, "PhaseD", 64, NULL, tskIDLE_PRIORITY+1, &tid_phaseD);
  xTaskCreate (clock,  "Clock",  64, NULL, tskIDLE_PRIORITY+1, &tid_clock);

  xTaskNotify (tid_phaseA, 0x0001, eSetBits); /* set signal to phaseA thread  */

  vTaskDelay (portMAX_DELAY);
  while(1);
}

int main (void) {
  SystemCoreClockUpdate();

  EvrFreeRTOSSetup(0);

  xTaskCreate (app_main, "app_main", 64, NULL, tskIDLE_PRIORITY+1, NULL);

  vTaskStartScheduler();

  while(1);
}
