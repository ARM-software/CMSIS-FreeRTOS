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
 * --------------------------------------------------------------------------
 *
 * $Revision:   V10.2.0
 *
 * Project:     CMSIS-FreeRTOS
 * Title:       FreeRTOS Secure Contex configuration definitions
 *
 * --------------------------------------------------------------------------*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//  <q>Use Memory Protection Unit
//  <i> Using Memory Protection Unit (MPU) affects secure context API.
//  <i> Enable MPU only when it is also enabled by the FreeRTOS Non-Secure Domain application.
//  <i> Default: 0
#define configENABLE_MPU                          0

//  <q>Use Floating Point Unit
//  <i> Using Floating Point Unit (FPU) affects secure context handling.
//  <i> Enable FPU when Secure Domain application uses floating point operations.
//  <i> Default: 0
#define configENABLE_FPU                          0

#endif /* FREERTOS_CONFIG_H */
