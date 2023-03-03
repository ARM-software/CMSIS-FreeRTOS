/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2022 Arm Limited. All rights reserved.
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
 *      Name:    main_s.c
 *      Purpose: TrustZone Secure Domain example program
 *
 *---------------------------------------------------------------------------*/
 
#include <arm_cmse.h>
#include "secure_port_macros.h"         // ARM.FreeRTOS::RTOS:TrustZone


/* Start address of non-secure application */
#ifndef NS_APP_START_ADDRESS
#define NS_APP_START_ADDRESS (0x00008000)
#endif

/* Non-secure function pointer type */
typedef void (*NS_Func_t) (void) __attribute__((cmse_nonsecure_call));

/*
  Fist entry in the non-secure vector table is the Main Stack Pointer:
  *((uint32_t *)(NS_APP_START_ADDRESS)) == MSP_NS

  Second entry in the non-secure vector table is the Reset Handler:
  *((uint32_t *)(NS_APP_START_ADDRESS + 4U)) == Reset_Handler
*/


int main (void) {
  NS_Func_t ResetHandler_NS;

  /* Set Main Stack Pointer for the non-secure side (MSP_NS) */
  secureportSET_MSP_NS (*((uint32_t *)(NS_APP_START_ADDRESS)));

  /* Set address of the non-secure Reset Handler */
  ResetHandler_NS = (NS_Func_t)(*((uint32_t *)(NS_APP_START_ADDRESS + 4U)));
  
  /* Call non-secure Reset Handler and start executing non-secure application */
  ResetHandler_NS();

  /* Non-secure application does not return */
  for (;;){;}
}
