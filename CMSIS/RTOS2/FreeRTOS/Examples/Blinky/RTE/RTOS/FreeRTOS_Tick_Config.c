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
 *      Name:    FreeRTOS_Tick_Config.c
 *      Purpose: FreeRTOS kernel tick timer configuration
 *
 *---------------------------------------------------------------------------*/

#include <string.h>

#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core

#if defined (TICK_TIMER_SYSTICK)
/* SysTick handler prototype */
extern void SysTick_Handler (void);

/* FreeRTOS tick timer interrupt handler prototype */
extern void xPortSysTickHandler (void);

/*
  SysTick handler implementation that also clears overflow flag.
*/
void SysTick_Handler (void) {
  /* Clear overflow flag */
  SysTick->CTRL;

  /* Call tick handler */
  xPortSysTickHandler();
}
#elif defined (TICK_TIMER_PTIM)
/*
  Configure Private Timer as FreeRTOS tick timer and install function
  FreeRTOS_Tick_Handler as tick timer interrupt handler.
*/
void vConfigureTickInterrupt (void) {

  PTIM_SetLoadValue ((SystemCoreClock/configTICK_RATE_HZ) - 1U);
  PTIM_SetControl (PTIM_GetControl() | 7U);

  /* Next assume Install_Interrupt() installs the function passed as its second
  parameter as the handler for the peripheral passed as its first parameter. */
  InterruptHandlerRegister (PrivTimer_IRQn, FreeRTOS_Tick_Handler);

  /* Determine number of implemented priority bits */
  GIC_SetPriority (PrivTimer_IRQn, 0xFFU);

  /* Set lowest priority -1 */
  GIC_SetPriority (PrivTimer_IRQn, GIC_GetPriority(PrivTimer_IRQn)-1);

  /* Enable IRQ */
  GIC_EnableIRQ (PrivTimer_IRQn);
}

/*
  Clear Private Timer interrupt flag.
*/
void vClearTickInterrupt (void) {
  PTIM_ClearEventFlag();
}
#endif
