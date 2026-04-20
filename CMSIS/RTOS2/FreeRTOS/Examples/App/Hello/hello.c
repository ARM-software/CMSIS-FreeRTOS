/*
 * Copyright 2024, 2026 Arm Limited and/or its affiliates.
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
 */

#include <stdio.h>

#include "cmsis_os2.h"
#include "main.h"

/* Thread attributes */
const osThreadAttr_t thread_attr_main  = {
  .name = "app_main",
  .stack_size = 1024U,
};

/* Thread IDs */
osThreadId_t thread_id_main;

/*
  Application main thread.
*/
__NO_RETURN void app_main_thread (void *argument) {
  (void)argument;

  for(int count = 0; count < 10; count++) {
    printf("Hello World %d\r\n", count);
    osDelay(1000U);
  }
  osDelay(osWaitForever);
  for(;;);
}

/*
  Application initialization.
*/
int app_main (void) {

  /* Initialize CMSIS-RTOS2 */
  osKernelInitialize();

  /* Create application main thread */
  thread_id_main = osThreadNew(app_main_thread, NULL, &thread_attr_main);

  /* Start thread execution */
  osKernelStart();

  return 0;
}
