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
 *      Name:    FreeRTOS_Tick_Config.h
 *      Purpose: FreeRTOS kernel tick timer definitions
 *
 *---------------------------------------------------------------------------*/

#ifndef FREERTOS_TICK_CONFIG_H
#define FREERTOS_TICK_CONFIG_H

#include "RTE_Components.h"             // Component selection
#include CMSIS_device_header

#if defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
  #define TICK_TIMER_SYSTICK
#elif defined (__ARM_ARCH_7A__)
  #define TICK_TIMER_PTIM
#else
  #error "FreeRTOS tick timer configuration is unknown."
#endif


void vConfigureTickInterrupt (void);
void vClearTickInterrupt     (void);

__STATIC_INLINE uint32_t TickTimer_GetPeriod   (void) __attribute__((always_inline));
__STATIC_INLINE uint32_t TickTimer_GetValue    (void) __attribute__((always_inline));
__STATIC_INLINE uint32_t TickTimer_GetOverflow (void) __attribute__((always_inline));


#if defined(TICK_TIMER_SYSTICK) /* Use SysTick Timer */
/*
  Get tick timer period
*/
uint32_t TickTimer_GetPeriod (void) {
  return (SysTick->LOAD + 1U);
}

/*
  Get tick timer current count value
*/
uint32_t TickTimer_GetValue (void) {
  uint32_t load = SysTick->LOAD;
  return  (load - SysTick->VAL);
}

/*
  Get tick timer overflow status
*/
uint32_t TickTimer_GetOverflow (void) {
  return ((SysTick->CTRL >> 16) & 1U);
}

#elif defined(TICK_TIMER_PTIM) /* Use Private Timer */
/*
  Get tick timer period
*/
uint32_t TickTimer_GetPeriod (void) {
  return (PTIM->LOAD + 1U);
}

/*
  Get tick timer current count value
*/
uint32_t TickTimer_GetValue (void) {
  uint32_t load = PTIM->LOAD;
  return  (load - PTIM->COUNTER);
}

/*
  Get tick timer overflow status
*/
uint32_t TickTimer_GetOverflow (void) {
  return (PTIM->ISR & 1);
}
#endif

#endif /* FREERTOS_TICK_CONFIG_H */
