/******************************************************************************
 * @file     system_ARMCA9.c
 * @brief    CMSIS Device System Source File for ARM Cortex-A9 Device Series
 * @version  V1.00
 * @date     22 Feb 2017
 *
 * @note
 *
 ******************************************************************************/
/*
 * Copyright (c) 2009-2017 ARM Limited. All rights reserved.
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

#include <ARMCA9.h>
#include "irq_ctrl.h"

#define  SYSTEM_CLOCK  12000000U

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;

/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)
{
  SystemCoreClock = SYSTEM_CLOCK;
}

/*----------------------------------------------------------------------------
  IRQ Handler Register/Unregister
 *----------------------------------------------------------------------------*/
IRQHandler IRQTable[40U] = { 0U };

uint32_t IRQCount = sizeof IRQTable / 4U;

uint32_t InterruptHandlerRegister (IRQn_Type irq, IRQHandler handler)
{
  if (irq < IRQCount) {
    IRQTable[irq] = handler;
    return 0U;
  }
  else {
    return 1U;
  }
}

uint32_t InterruptHandlerUnregister (IRQn_Type irq)
{
  if (irq < IRQCount) {
    IRQTable[irq] = 0U;
    return 0U;
  }
  else {
    return 1U;
  }
}

/*----------------------------------------------------------------------------
  System Initialization
 *----------------------------------------------------------------------------*/
void SystemInit (void)
{
/* do not use global variables because this function is called before
   reaching pre-main. RW section may be overwritten afterwards.          */

  // Invalidate entire Unified TLB
  __set_TLBIALL(0);

  // Invalidate entire branch predictor array
  __set_BPIALL(0);
  __DSB();
  __ISB();

  //  Invalidate instruction cache and flush branch target cache
  __set_ICIALLU(0);
  __DSB();
  __ISB();

  //  Invalidate data cache
  L1C_InvalidateDCacheAll();

  // Create Translation Table
  MMU_CreateTranslationTable();

  // Enable MMU
  MMU_Enable();

  // Enable Caches
  L1C_EnableCaches();
  L1C_EnableBTAC();

#if (__L2C_PRESENT == 1) 
  // Enable GIC
  L2C_Enable();
#endif

#if (__GIC_PRESENT == 1) 
  // Enable GIC
  GIC_Enable();
#endif

#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
  // Enable FPU
  __FPU_Enable();
#endif
}

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
  h();
}
