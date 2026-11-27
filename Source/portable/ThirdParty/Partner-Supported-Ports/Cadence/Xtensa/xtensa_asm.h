/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2015-2025 Cadence Design Systems, Inc.
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef XTENSA_ASM_H
#define XTENSA_ASM_H

#include <FreeRTOSConfig.h>

#if (defined configNUMBER_OF_CORES) && ( configNUMBER_OF_CORES > 1 )
#define XT_SMP_MACROS		1
#else
#define XT_SMP_MACROS		0
#endif

#if XT_SMP_MACROS

/*
*******************************************************************************
* Macro to return core ID into regieter r.
* Useful for assembly implementations of xthal_get_coreid().
*******************************************************************************
*/
    .macro  coreid  r
    rsr.prid    \r
#if XCHAL_SUBSYS_CORE_ID_BITS
    extui   \r,  \r, 0, XCHAL_SUBSYS_CORE_ID_BITS
#endif
    .endm

#endif	/* XT_SMP_MACROS */

/*
*******************************************************************************
* Macro to load a pointer to the current core's current TCB into register r.
* NOTE: Trashes register t and places coreid in it on SMP configurations.
*******************************************************************************
*/
#if XT_SMP_MACROS
    .extern pxCurrentTCBs
#else
    .extern pxCurrentTCB
#endif

    .macro  pxctcb  r, t
#if XT_SMP_MACROS
    coreid  \t
    movi    \r,  pxCurrentTCBs
    addx4   \r,  \t, \r
#else
    movi    \r,  pxCurrentTCB
#endif
    .endm

/*
*******************************************************************************
* Macro to load a pointer to the current core's xt_internal_data_t block,
* which contains the port_interruptNesting (first field) and port_switch_flag
* variables, along with other data on SMP configurations.
* NOTE: Trashes register t on SMP configurations.
*******************************************************************************
*/
#if XT_SMP_MACROS
    // ( XT_USE_DATARAM == 0 ) :
    //   Defined in C as: xt_internal_data_t _xt_intdata[ configNUMBER_OF_CORES ];
    //   where sizeof(xt_internal_data_t ) == XCHAL_DCACHE_LINESIZE
    // ( XT_USE_DATARAM == 1 ) :
    //   Defined in C as: xt_internal_data_t _xt_intdata __attribute__ ((section(".dram0.data")));
    .extern _xt_intdata
#else
    // Defined in C as: xt_internal_data_t _xt_intdata;
    .extern _xt_intdata
#endif

    .macro  pintdata    r, t
#if XT_SMP_MACROS && ( XT_USE_DATARAM == 0 )
    coreid  \t
    movi    \r,  _xt_intdata
    slli    \t,  \t, XCHAL_DCACHE_LINEWIDTH
    add     \r,  \r, \t
#else
    movi    \r,  _xt_intdata
#endif
    .endm

#endif /* XTENSA_ASM_H */
