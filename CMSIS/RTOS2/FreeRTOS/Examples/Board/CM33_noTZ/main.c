/*
 * Copyright 2026 Arm Limited and/or its affiliates.
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

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_vio.h"
#include "main.h"

/* stdio via semihosting is initialized by the C runtime startup */
int stdio_init (void) {
  return 0;
}

int main (void) {

  /* Update System Core Clock info */
  SystemCoreClockUpdate();

  /* Initialize stdio */
  stdio_init();

  /* Initialize Virtual I/O */
  vioInit();


  return (app_main());
}
