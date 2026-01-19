/*---------------------------------------------------------------------------
 * Copyright (c) 2026 Arm Limited (or its affiliates).
 * All rights reserved.
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
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include "main.h"
#include "cmsis_os2.h"
#include "cmsis_vio.h"

/* Thread attributes for the app_main thread */
static const osThreadAttr_t thread_attr_main   = {.name = "app_main"};

/* Thread attributes for the LED thread */
static const osThreadAttr_t thread_attr_LED    = {.name = "LED"};

/* Thread attributes for the Button thread */
static const osThreadAttr_t thread_attr_Button = {.name = "Button"};

/* Thread ID for the LED thread */
static osThreadId_t tid_LED;

/* Thread ID for the Button thread */
static osThreadId_t tid_Button;

/*
  Thread that blinks LED.
*/
static __NO_RETURN void thread_LED(void *argument) {
  uint32_t active_flag = 0U;

  (void) argument;

  for (;;) {
    if (osThreadFlagsWait(1U, osFlagsWaitAny, 0U) == 1U) {
      /* Toggle active flag on button press */
      active_flag ^= 1U;
    }

    if (active_flag == 1U) {
      /* Blink LED0 and LED1 alternately */
      vioSetSignal(vioLED0, vioLEDoff);
      vioSetSignal(vioLED1, vioLEDon);
      osDelay(100U);
      vioSetSignal(vioLED0, vioLEDon);
      vioSetSignal(vioLED1, vioLEDoff);
      osDelay(100U);
    } else {
      /* Blink LED0 */
      vioSetSignal(vioLED0, vioLEDon);
      osDelay(500U);
      vioSetSignal(vioLED0, vioLEDoff);
      osDelay(500U);
    }
  }
}

/*
  Thread that checks Button state.
*/
static __NO_RETURN void thread_Button(void *argument) {
  uint32_t last = 0U;
  uint32_t state;

  (void)argument;

  for (;;) {
    /* Get button state */
    state = vioGetSignal(vioBUTTON0);
    if (state != last) {
      if (state == 1U) {
        /* Button pressed, notify LED thread */
        osThreadFlagsSet(tid_LED, 1U);
      }
      last = state;
    }
    osDelay(100U);
  }
}

/*
  Application main thread.
*/
__NO_RETURN void app_main_thread(void *argument) {
  (void) argument;

  printf("Blinky example\n");

  /* Create LED and Button threads */
  tid_LED    = osThreadNew(thread_LED, NULL, &thread_attr_LED);
  tid_Button = osThreadNew(thread_Button, NULL, &thread_attr_Button);

  for (;;) {
    /* Delay indefinitely */
    osDelay(osWaitForever);
  }
}

/*
  Application initialization.
*/
int app_main(void) {

  /* Initialize CMSIS-RTOS2, create app main thread and start RTOS kernel */
  osKernelInitialize();
  osThreadNew(app_main_thread, NULL, &thread_attr_main);
  osKernelStart();

  return 0;
}
