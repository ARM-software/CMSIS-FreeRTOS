/*
 * Copyright 2017-2018, 2026 Arm Limited and/or its affiliates.
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

#include <stddef.h>

#if defined(_RTE_)
#include "RTE_Components.h"
#elif !defined(CMSIS_device_header)
#error "CMSIS_device_header must be defined to point to CMSIS device header"
#endif

#include CMSIS_device_header
#include "irq_ctrl.h"

/* The function called by the RTOS port layer after it has managed interrupt
entry. */
void vApplicationIRQHandler( uint32_t ulICCIAR )
{
  uint32_t ulInterruptID;
  IRQHandler_t h;

  /* Re-enable interrupts. */
  __enable_irq();

  /* The ID of the interrupt can be obtained by bitwise anding the ICCIAR value
  with 0x3FF. */
  ulInterruptID = ulICCIAR & 0x3FFUL;

  /* Call the function installed in the array of installed handler functions. */
  h = IRQ_GetHandler (ulInterruptID);

  /* Call handler function */
  if (h != NULL) {
    h();
  }
}
