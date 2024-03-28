/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2019 Arm Limited. All rights reserved.
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
 *      Name:    library_nsc.c
 *      Purpose: Example function callable from the non-secure domain
 *
 *---------------------------------------------------------------------------*/
 
#include <arm_cmse.h>
#include "secure_port_macros.h"         // ARM.FreeRTOS::RTOS:TrustZone

#include "library_nsc.h"                // Non-secure callable function definition

/* Non-secure function pointer type */
typedef void (*NS_Func_t) (void) __attribute__((cmse_nonsecure_call));


/* Secure counter */
static uint32_t Count_S = 0U;

/* Function that can be called from the non-secure application */
secureportNON_SECURE_CALLABLE uint32_t Func_NSC (Callback_t callback) {
  NS_Func_t ns_callback;

  /* Create function pointer to call back non-secure domain */
  ns_callback = (NS_Func_t)cmse_nsfptr_create(callback);

  /* Execute callback function */
  ns_callback();

  Count_S += 1U;

  /* Return secure counter value */
  return Count_S;
}
