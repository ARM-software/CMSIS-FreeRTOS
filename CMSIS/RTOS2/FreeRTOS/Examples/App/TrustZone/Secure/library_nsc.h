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
 *      Name:    library_nsc.h
 *      Purpose: Example function callable from the non-secure domain
 *
 *---------------------------------------------------------------------------*/

#ifndef LIBRARY_NSC_H__
#define LIBRARY_NSC_H__

#include <stdint.h>

/* Callback function pointer type */
typedef void (*Callback_t) (void);


/*
  Secure function that executes:
  - callback to the non-secure application
  - increments secure counter and return its value
*/
uint32_t Func_NSC (Callback_t callback);

#endif /* LIBRARY_NSC */
