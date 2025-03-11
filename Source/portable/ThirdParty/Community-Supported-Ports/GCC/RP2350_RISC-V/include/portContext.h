/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef PORTCONTEXT_H
#define PORTCONTEXT_H

#include "pico.h"
#include "hardware/regs/rvcsr.h"
#include "hardware/regs/intctrl.h"

/*
 * todo this is currently disabled; as it certainly breaks
 * the RP2 on_core_zero & on_core_one interrupt tests in FreeRTOS
 */
#ifndef portcontextALLOW_NESTED_IRQs
#define portcontextALLOW_NESTED_IRQs 0
#endif

#if __riscv_xlen == 64
    #define portWORD_SIZE    8
    #define store_x          sd
    #define load_x           ld
#elif __riscv_xlen == 32
    #define store_x          sw
    #define load_x           lw
    #define portWORD_SIZE    4
#else
    #error Assembler did not define __riscv_xlen
#endif

#include "freertos_risc_v_chip_specific_extensions.h"

/* Only the standard core registers are stored by default.  Any additional
 * registers must be saved by the portasmSAVE_ADDITIONAL_REGISTERS and
 * portasmRESTORE_ADDITIONAL_REGISTERS macros - which can be defined in a chip
 * specific version of freertos_risc_v_chip_specific_extensions.h.  See the
 * notes at the top of portASM.S file. */
#ifdef __riscv_32e
    #define portCONTEXT_SIZE               ( 15 * portWORD_SIZE )
    #define portCRITICAL_NESTING_OFFSET    13
    #define portMSTATUS_OFFSET             14
#else
    #define portCONTEXT_SIZE               ( 31 * portWORD_SIZE )
    #define portCRITICAL_NESTING_OFFSET    29
    #define portMSTATUS_OFFSET             30
#endif

/*-----------------------------------------------------------*/

/*
 * OK this is ugly; we cannot use configNUMBER_OF_CORES because FreeRTOSConfig.h is not required to be ASM includable;
 * So we have a global pointer that is either to a value 0 for configNUMBER_OF_CORES == 1, or points to the SIO
 * reg on configNUMBER_OF_CORES == 1
 */
.extern pxCurrentCoreID
/*
 * Note also we must use our own symbol, because pxCurrentTCB is defined by tasks.c for configNUMBER_OF_CORES == 1
 * vs pCurrentTCBs for configNUMBER_OF_CORES != 1
 */
.extern pxCurrentTCBArray
.extern xCriticalNestingArray

.extern xISRStackTop
/*-----------------------------------------------------------*/

.macro portcontextGET_CORE_ID out_reg
    load_x \out_reg, pxCurrentCoreID
    load_x \out_reg, 0 ( \out_reg )
.endm

.macro portcontextGET_CURRENT_TCB_FOR_CORE core_reg, out_reg
    /* todo wasteful indirections here because we don't have configNUMBER_OF_CORES */
    la \out_reg, pxCurrentTCBArray
    load_x \out_reg, 0 ( \out_reg )
    sh2add \out_reg, \core_reg, \out_reg
    load_x \out_reg, 0 ( \out_reg )
.endm

.macro portcontextGET_CRITICAL_NESTING_PTR_FOR_CORE core_reg, out_reg
    load_x \out_reg, xCriticalNestingArray
    sh2add \out_reg, \core_reg, \out_reg
.endm

.macro portcontextSWITCH_TO_ISR_STACK reg_pISRStackTop, reg_ISRStackTop
    la \reg_pISRStackTop, SIO_BASE
    load_x \reg_pISRStackTop, 0 ( \reg_pISRStackTop )
    la \reg_ISRStackTop, xISRStackTops
    sh2add \reg_pISRStackTop, \reg_pISRStackTop, \reg_ISRStackTop
    load_x \reg_ISRStackTop, 0 ( \reg_pISRStackTop )
    beqz \reg_ISRStackTop, no_isr_stack\@
    mv sp, \reg_ISRStackTop
no_isr_stack\@:
.endm
/*-----------------------------------------------------------*/

.macro portcontextSAVE_CONTEXT_INTERNAL
addi sp, sp, -portCONTEXT_SIZE
store_x x1, 1 * portWORD_SIZE( sp )
store_x x5, 2 * portWORD_SIZE( sp )
store_x x6, 3 * portWORD_SIZE( sp )
store_x x7, 4 * portWORD_SIZE( sp )
store_x x8, 5 * portWORD_SIZE( sp )
store_x x9, 6 * portWORD_SIZE( sp )
store_x x10, 7 * portWORD_SIZE( sp )
store_x x11, 8 * portWORD_SIZE( sp )
store_x x12, 9 * portWORD_SIZE( sp )
store_x x13, 10 * portWORD_SIZE( sp )
store_x x14, 11 * portWORD_SIZE( sp )
store_x x15, 12 * portWORD_SIZE( sp )
#ifndef __riscv_32e
    store_x x16, 13 * portWORD_SIZE( sp )
    store_x x17, 14 * portWORD_SIZE( sp )
    store_x x18, 15 * portWORD_SIZE( sp )
    store_x x19, 16 * portWORD_SIZE( sp )
    store_x x20, 17 * portWORD_SIZE( sp )
    store_x x21, 18 * portWORD_SIZE( sp )
    store_x x22, 19 * portWORD_SIZE( sp )
    store_x x23, 20 * portWORD_SIZE( sp )
    store_x x24, 21 * portWORD_SIZE( sp )
    store_x x25, 22 * portWORD_SIZE( sp )
    store_x x26, 23 * portWORD_SIZE( sp )
    store_x x27, 24 * portWORD_SIZE( sp )
    store_x x28, 25 * portWORD_SIZE( sp )
    store_x x29, 26 * portWORD_SIZE( sp )
    store_x x30, 27 * portWORD_SIZE( sp )
    store_x x31, 28 * portWORD_SIZE( sp )
#endif /* ifndef __riscv_32e */

portcontextGET_CORE_ID t1
portcontextGET_CRITICAL_NESTING_PTR_FOR_CORE t1, t0
load_x t0, 0 ( t0 )
store_x t0, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp ) /* Store the critical nesting value to the stack. */


csrr t0, mstatus /* Required for MPIE bit. */
store_x t0, portMSTATUS_OFFSET * portWORD_SIZE( sp )


portasmSAVE_ADDITIONAL_REGISTERS /* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

    portcontextGET_CORE_ID t1
    portcontextGET_CURRENT_TCB_FOR_CORE t1, t0
    store_x sp, 0 ( t0 )             /* Write sp to first TCB member. */

   .endm
/*-----------------------------------------------------------*/

   .macro portcontextSAVE_EXCEPTION_CONTEXT
portcontextSAVE_CONTEXT_INTERNAL
csrr a0, mcause
csrr a1, mepc
addi a1, a1, 4          /* Synchronous so update exception return address to the instruction after the instruction that generated the exception. */
store_x a1, 0 ( sp )    /* Save updated exception return address. */
portcontextSWITCH_TO_ISR_STACK t0, t1
   .endm
/*-----------------------------------------------------------*/

   .macro portcontextSAVE_INTERRUPT_CONTEXT
portcontextSAVE_CONTEXT_INTERNAL
csrr a0, mcause
csrr a1, mepc
store_x a1, 0 ( sp )    /* Asynchronous interrupt so save unmodified exception return address. */
portcontextSWITCH_TO_ISR_STACK s10, s11
/*
 * s10 is now pISRStackTop for the current core
 * s11 is the old value of ISRStackTop
 * we expect them to remain saved, and we overwrite the
 * pISRStackTop with zero, so we don't overwrite again if nesting is enabled
 */
    sw zero, 0 (s10)
   .endm
/*-----------------------------------------------------------*/

.macro portcontextRESTORE_CONTEXT
portcontextGET_CORE_ID t0
portcontextGET_CURRENT_TCB_FOR_CORE t0, t1
load_x sp, 0 ( t1 )     /* Read sp from first TCB member. */

/* Load mepc with the address of the instruction in the task to run next. */
load_x t0, 0 ( sp )
csrw mepc, t0

/* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */
portasmRESTORE_ADDITIONAL_REGISTERS

/* Load mstatus with the interrupt enable bits used by the task. */
load_x t0, portMSTATUS_OFFSET * portWORD_SIZE( sp )
csrw mstatus, t0                                             /* Required for MPIE bit. */

/* Load the address of xCriticalNesting into t1. */
portcontextGET_CORE_ID t0
portcontextGET_CRITICAL_NESTING_PTR_FOR_CORE t0, t1

load_x t0, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp ) /* Obtain xCriticalNesting value for this task from task's stack. */
store_x t0, 0 ( t1 )                                         /* Restore the critical nesting value for this task. */

load_x x1, 1 * portWORD_SIZE( sp )
load_x x5, 2 * portWORD_SIZE( sp )
load_x x6, 3 * portWORD_SIZE( sp )
load_x x7, 4 * portWORD_SIZE( sp )
load_x x8, 5 * portWORD_SIZE( sp )
load_x x9, 6 * portWORD_SIZE( sp )
load_x x10, 7 * portWORD_SIZE( sp )
load_x x11, 8 * portWORD_SIZE( sp )
load_x x12, 9 * portWORD_SIZE( sp )
load_x x13, 10 * portWORD_SIZE( sp )
load_x x14, 11 * portWORD_SIZE( sp )
load_x x15, 12 * portWORD_SIZE( sp )
#ifndef __riscv_32e
    load_x x16, 13 * portWORD_SIZE( sp )
    load_x x17, 14 * portWORD_SIZE( sp )
    load_x x18, 15 * portWORD_SIZE( sp )
    load_x x19, 16 * portWORD_SIZE( sp )
    load_x x20, 17 * portWORD_SIZE( sp )
    load_x x21, 18 * portWORD_SIZE( sp )
    load_x x22, 19 * portWORD_SIZE( sp )
    load_x x23, 20 * portWORD_SIZE( sp )
    load_x x24, 21 * portWORD_SIZE( sp )
    load_x x25, 22 * portWORD_SIZE( sp )
    load_x x26, 23 * portWORD_SIZE( sp )
    load_x x27, 24 * portWORD_SIZE( sp )
    load_x x28, 25 * portWORD_SIZE( sp )
    load_x x29, 26 * portWORD_SIZE( sp )
    load_x x30, 27 * portWORD_SIZE( sp )
    load_x x31, 28 * portWORD_SIZE( sp )
#endif /* ifndef __riscv_32e */
addi sp, sp, portCONTEXT_SIZE

mret
   .endm
/*-----------------------------------------------------------*/

.macro portcontextRESTORE_INTERUUPT_CONTEXT
    /* restore old ISRStackTop */
    sw s11, 0 (s10)
    portcontextRESTORE_CONTEXT
.endm

#endif /* PORTCONTEXT_H */
