 /*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2015-2024 Cadence Design Systems, Inc.
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

/*
 * Configuration-specific information for Xtensa build. This file must be
 * included in FreeRTOSConfig.h to properly set up the config-dependent
 * parameters correctly.
 *
 * NOTE: To enable thread-safe C library support, XT_USE_THREAD_SAFE_CLIB must
 * be defined to be > 0 somewhere above or on the command line.
 */

#ifndef XTENSA_CONFIG_H
#define XTENSA_CONFIG_H

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include <xtensa/config/system.h>   /* required for XSHAL_CLIB */

#include "xtensa_context.h"


/*-----------------------------------------------------------------------------
*                                 STACK REQUIREMENTS
*
* This section defines the minimum stack size, and the extra space required to
* be allocated for saving coprocessor state on the stack when needed. The sizes
* are in bytes.
*
* Stack sizes for individual tasks should be derived from these minima based on
* the maximum call depth of the task. A minimum stack size is defined by the
* XT_STACK_MIN_SIZE value. This minimum is based on the requirement for a task
* that calls nothing else but can be interrupted.
*
* If the Xtensa processor configuration includes coprocessors, then space is
* allocated to save the coprocessor state on the stack.
*
* If thread safety is enabled for the C runtime library, (XT_USE_THREAD_SAFE_CLIB
* is defined) then space is allocated to save the C library context in the TCB.
* 
* Allocating insufficient stack space is a common source of hard-to-find errors.
* During development, it is best to enable the FreeRTOS stack checking features.
*
* Usage:
* 
* XT_USE_THREAD_SAFE_CLIB -- Define this to a nonzero value to enable thread-safe
*                            use of the C library.
* 
* NOTE: The Xtensa toolchain supports multiple C libraries and not all of them
* support thread safety. Check your core configuration to see which C library
* was chosen for your system.
* 
* XT_STACK_MIN_SIZE       -- The minimum stack size for any task. It is recommended
*                            that you do not use a stack smaller than this for any
*                            task. In case you want to use stacks smaller than this
*                            size, you must verify that the smaller size(s) will work
*                            under all operating conditions.
*
* XT_STACK_EXTRA          -- The amount of extra stack space to be allocated for the
*                            system overhead (coprocessor state, exception frame etc.).
*                            Add this to the amount of stack space required by the
*                            task itself.
*
* XT_SYSTEM_STACK_SIZE    -- The size of the system interrupt stack. You will need to
*                            size this according to your system requirements.
-----------------------------------------------------------------------------*/

/* Extra space required for interrupt/exception hooks. */
#ifdef XT_INTEXC_HOOKS
  #ifdef __XTENSA_CALL0_ABI__
    #define STK_INTEXC_EXTRA        0x200
  #else
    #define STK_INTEXC_EXTRA        0x180
  #endif
#else
  #define STK_INTEXC_EXTRA          0
#endif

/* Check C library thread safety support and compute size of C library save area.
   For the supported libraries, we enable thread safety by default, and this can
   be overridden from the compiler/make command line. */
#if (XSHAL_CLIB == XTHAL_CLIB_NEWLIB) || (XSHAL_CLIB == XTHAL_CLIB_XCLIB)
  #ifndef XT_USE_THREAD_SAFE_CLIB
    #define XT_USE_THREAD_SAFE_CLIB         1
  #endif
#else
  #define XT_USE_THREAD_SAFE_CLIB           0
#endif

#if XT_USE_THREAD_SAFE_CLIB > 0u
  #if XSHAL_CLIB == XTHAL_CLIB_XCLIB
    #define XT_HAVE_THREAD_SAFE_CLIB        1
    #if !defined __ASSEMBLER__
      #include <sys/reent.h>
      #define XT_CLIB_CONTEXT_AREA_SIZE     ((sizeof(struct _reent) + 15) + (-16))
      #define XT_CLIB_GLOBAL_PTR            _reent_ptr
      #define _REENT_INIT_PTR               _init_reent
      #define _impure_ptr                   _reent_ptr

      void _reclaim_reent(struct _reent * ptr);
    #endif  // !__ASSEMBLER__
  #elif XSHAL_CLIB == XTHAL_CLIB_NEWLIB
    #define XT_HAVE_THREAD_SAFE_CLIB        1
    #if !defined __ASSEMBLER__
      #include <sys/reent.h>
      #define XT_CLIB_CONTEXT_AREA_SIZE     ((sizeof(struct _reent) + 15) + (-16))
      #define XT_CLIB_GLOBAL_PTR            _impure_ptr

      void _reclaim_reent(struct _reent * ptr);
    #endif  // !__ASSEMBLER__
  #else     // XTHAL_CLIB_XCLIB || XTHAL_CLIB_NEWLIB
    #define XT_HAVE_THREAD_SAFE_CLIB        0
    #error The selected C runtime library is not thread safe.
  #endif    // XTHAL_CLIB_XCLIB || XTHAL_CLIB_NEWLIB
  #if (defined __DYNAMIC_REENT__)
    // For xclib/newlib with support for custom reent_ptr_() we keep
    // XT_CLIB_GLOBAL_PTR within interrupt data struct
    #if (configNUMBER_OF_CORES > 1)
    #define configSET_TLS_BLOCK(xTLSBlock)  ( _XT_INTDATA(portGET_CORE_ID()).xt_reent_p = \
                                                &( xTLSBlock ) )
    #else
    #define configSET_TLS_BLOCK(xTLSBlock)  ( _xt_intdata.xt_reent_p = &( xTLSBlock ) )
    #endif
  #endif // __DYNAMIC_REENT__
#else
  #define XT_CLIB_CONTEXT_AREA_SIZE         0
#endif      // XT_USE_THREAD_SAFE_CLIB

/*------------------------------------------------------------------------------
  Extra size -- interrupt frame plus coprocessor save area plus hook space.
  NOTE: Make sure XT_INTEXC_HOOKS is undefined unless you really need the hooks.
------------------------------------------------------------------------------*/
#ifdef __XTENSA_CALL0_ABI__
  #define XT_XTRA_SIZE            (XT_STK_FRMSZ + STK_INTEXC_EXTRA + 0x10 + XT_CP_SIZE)
#else
  #define XT_XTRA_SIZE            (XT_STK_FRMSZ + STK_INTEXC_EXTRA + 0x20 + XT_CP_SIZE)
#endif

/*------------------------------------------------------------------------------
  Space allocated for user code -- function calls and local variables.
  NOTE: This number can be adjusted to suit your needs. You must verify that the
  amount of space you reserve is adequate for the worst-case conditions in your
  application.
  NOTE: The windowed ABI requires more stack, since space has to be reserved
  for spilling register windows.
------------------------------------------------------------------------------*/
#ifdef __XTENSA_CALL0_ABI__
  #define XT_USER_SIZE            0x200
#else
  #define XT_USER_SIZE            0x400
#endif

/* Minimum recommended stack size. */
#define XT_STACK_MIN_SIZE         ((XT_XTRA_SIZE + XT_USER_SIZE) / sizeof(unsigned char))

/* OS overhead. */
#define XT_STACK_EXTRA            (XT_XTRA_SIZE)

/* Default system (interrupt) stack size */
#define XT_SYSTEM_STACK_SIZE      0x400

/**
 * XT_USE_L2RAM is defined in xtensa_config.h and can be enabled to improve
 * performance for SMP configurations.  When set, all "PRIVILEGED_DATA" 
 * structures are moved to L2RAM instead of L2-cached sysram.  Both locations
 * are cached per coherence protocol in each core's L1 data cache.
 *
 * It's worth noting that the default FreeRTOS heap is privileged and is moved
 * along with these structures, so sufficient L2RAM space must be provisioned.
 */
#if (configNUMBER_OF_CORES > 1)
    /* Not usable if L2 is configured as all-cache */
    #if XCHAL_L2CACHE_ONLY
    #undef  XT_USE_L2RAM
    #define XT_USE_L2RAM          0
    #endif
    /* Default is to not use L2RAM for shared data structures;
     * enabling this can improve context switching performance.
     */
    #if !(defined XT_USE_L2RAM)
    #define XT_USE_L2RAM          0
    #endif
#else
    #undef  XT_USE_L2RAM
    #define XT_USE_L2RAM          0
#endif

#if XT_USE_L2RAM
    /**
     * The 50/50 L2CACHE/L2RAM split defined here may not work for all systems.
     * These defines must be absolute numerical values, not expressions.
     */
    #define XT_L2RAM_SIXTEENTHS                         8
    #define XT_L2CACHE_SIXTEENTHS                       8

    #define PRIVILEGED_DATA        __attribute__( ( section( ".l2ram.bss" ) ) )
#endif

/**
 * XT_USE_DATARAM is defined in xtensa_config.h and can be enabled to improve
 * performance for SMP configurations.  When set, the _xt_intdata per-core
 * structures are moved to dataram on each core.  This reduces overhead of
 * indexing the shared structure and provides fast access to RTOS data.
 *
 * TODO: may also improve performance for single-core configs.
 */
#if ((configNUMBER_OF_CORES > 1) && (XCHAL_NUM_DATARAM > 0))
    #if !(defined XT_USE_DATARAM)
    #define XT_USE_DATARAM        0
    #endif
#else
    #undef  XT_USE_DATARAM
    #define XT_USE_DATARAM        0
#endif

#if XT_USE_DATARAM
    #define XT_DATARAM_ATTR       __attribute__ ((section(".dram0.data")))
#endif

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* XTENSA_CONFIG_H */
