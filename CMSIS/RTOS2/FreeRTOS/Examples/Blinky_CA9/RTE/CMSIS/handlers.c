/******************************************************************************
 * @file     irq_handler.c
 * @brief    CMSIS-FreeRTOS Interrupt Handler
 * @version  9.1.0
 * @date     11 Aug 2017
 *
 * @note
 *
 ******************************************************************************/
/*
 * Copyright (c) 2017 Arm Limited. All rights reserved.
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

#include "RTE_Components.h"
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
