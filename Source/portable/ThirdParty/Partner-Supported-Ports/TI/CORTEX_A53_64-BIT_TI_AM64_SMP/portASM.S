/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
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
 *  Copyright (C) 2018-2022 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

	.text

	/* Variables and functions. */
	.extern vTaskSwitchContext
	.extern HwiP_intrHandler
	.extern ullPortInterruptNesting
	.extern ullPortTaskHasFPUContext
	.extern ullPortYieldRequired

	.global HwiP_IRQ_Handler
    .global HwiP_SVC_Handler
	.global vPortRestoreTaskContext
    .global HwiP_defaultExcHandler
	.global HwiP_abortHandler

.macro PUSH_ALL_CPU_REGS stackPtr
        stp     x0, x1, [\stackPtr, #-16]!
        stp     x2, x3, [\stackPtr, #-16]!
        stp     x4, x5, [\stackPtr, #-16]!
        stp     x6, x7, [\stackPtr, #-16]!
        stp     x8, x9, [\stackPtr, #-16]!
        stp     x10, x11, [\stackPtr, #-16]!
        stp     x12, x13, [\stackPtr, #-16]!
        stp     x14, x15, [\stackPtr, #-16]!
        stp     x16, x17, [\stackPtr, #-16]!
        stp     x18, x19, [\stackPtr, #-16]!
        stp     x20, x21, [\stackPtr, #-16]!
        stp     x22, x23, [\stackPtr, #-16]!
        stp     x24, x25, [\stackPtr, #-16]!
        stp     x26, x27, [\stackPtr, #-16]!
        stp     x28, x29, [\stackPtr, #-16]!
        stp     x30, xzr, [\stackPtr, #-16]!
.endm

.macro POP_ALL_CPU_REGS stackPtr
        LDP 	X30, XZR, [\stackPtr], #0x10
        LDP 	X28, X29, [\stackPtr], #0x10
        LDP 	X26, X27, [\stackPtr], #0x10
        LDP 	X24, X25, [\stackPtr], #0x10
        LDP 	X22, X23, [\stackPtr], #0x10
        LDP 	X20, X21, [\stackPtr], #0x10
        LDP 	X18, X19, [\stackPtr], #0x10
        LDP 	X16, X17, [\stackPtr], #0x10
        LDP 	X14, X15, [\stackPtr], #0x10
        LDP 	X12, X13, [\stackPtr], #0x10
        LDP 	X10, X11, [\stackPtr], #0x10
        LDP 	X8, X9, [\stackPtr], #0x10
        LDP 	X6, X7, [\stackPtr], #0x10
        LDP 	X4, X5, [\stackPtr], #0x10
        LDP 	X2, X3, [\stackPtr], #0x10
        LDP 	X0, X1, [\stackPtr], #0x10
.endm

.macro PUSH_CALLER_SAVE_CPU_REGS stackPtr
        stp     x0, x1, [\stackPtr, #-16]!
        stp     x2, x3, [\stackPtr, #-16]!
        stp     x4, x5, [\stackPtr, #-16]!
        stp     x6, x7, [\stackPtr, #-16]!
        stp     x8, x9, [\stackPtr, #-16]!
        stp     x10, x11, [\stackPtr, #-16]!
        stp     x12, x13, [\stackPtr, #-16]!
        stp     x14, x15, [\stackPtr, #-16]!
        stp     x16, x17, [\stackPtr, #-16]!
        stp     x18, x19, [\stackPtr, #-16]!
        stp     x29, x30, [\stackPtr, #-16]!
.endm

.macro POP_CALLER_SAVE_CPU_REGS stackPtr
        ldp     x29, x30, [\stackPtr], #16
        ldp     x18, x19, [\stackPtr], #16
        ldp     x16, x17, [\stackPtr], #16
        ldp     x14, x15, [\stackPtr], #16
        ldp     x12, x13, [\stackPtr], #16
        ldp     x10, x11, [\stackPtr], #16
        ldp     x8, x9, [\stackPtr], #16
        ldp     x6, x7, [\stackPtr], #16
        ldp     x4, x5, [\stackPtr], #16
        ldp     x2, x3, [\stackPtr], #16
        ldp     x0, x1, [\stackPtr], #16
.endm

.macro PUSH_CALLER_SAVE_FPU_REGS stackPtr
        stp     q0, q1, [\stackPtr, #-32]!
        stp     q2, q3, [\stackPtr, #-32]!
        stp     q4, q5, [\stackPtr, #-32]!
        stp     q6, q7, [\stackPtr, #-32]!
        stp     q8, q9, [\stackPtr, #-32]!
        stp     q10, q11, [\stackPtr, #-32]!
        stp     q12, q13, [\stackPtr, #-32]!
        stp     q14, q15, [\stackPtr, #-32]!
        stp     q16, q17, [\stackPtr, #-32]!
        stp     q18, q19, [\stackPtr, #-32]!
        stp     q20, q21, [\stackPtr, #-32]!
        stp     q22, q23, [\stackPtr, #-32]!
        stp     q24, q25, [\stackPtr, #-32]!
        stp     q26, q27, [\stackPtr, #-32]!
        stp     q28, q29, [\stackPtr, #-32]!
        stp     q30, q31, [\stackPtr, #-32]!
.endm

.macro POP_CALLER_SAVE_FPU_REGS stackPtr
        ldp     q30, q31, [\stackPtr], #32
        ldp     q28, q29, [\stackPtr], #32
        ldp     q26, q27, [\stackPtr], #32
        ldp     q24, q25, [\stackPtr], #32
        ldp     q22, q23, [\stackPtr], #32
        ldp     q20, q21, [\stackPtr], #32
        ldp     q18, q19, [\stackPtr], #32
        ldp     q16, q17, [\stackPtr], #32
        ldp     q14, q15, [\stackPtr], #32
        ldp     q12, q13, [\stackPtr], #32
        ldp     q10, q11, [\stackPtr], #32
        ldp     q8, q9, [\stackPtr], #32
        ldp     q6, q7, [\stackPtr], #32
        ldp     q4, q5, [\stackPtr], #32
        ldp     q2, q3, [\stackPtr], #32
        ldp     q0, q1, [\stackPtr], #32
.endm


.macro portSAVE_CONTEXT

	/* Switch to use the EL0 stack pointer. */
	MSR 	SPSEL, #0

	/* Save the entire context. */
    PUSH_ALL_CPU_REGS SP

	/* Save the SPSR. */
	MRS		X3, SPSR_EL1
	MRS		X2, ELR_EL1

	STP 	X2, X3, [SP, #-0x10]!

    /* Get coreId to X1 */
    MRS     X1, MPIDR_EL1
    AND     X1, X1, #0xff
    LDR		X0, ullPortTaskHasFPUContextConst
    CMP     X1, #0
    BEQ     3f
    ADD     X0, X0, #8
3:

	/* Save the FPU context indicator. */
	LDR		X2, [X0]

	/* Save the FPU context, if any (32 128-bit registers). */
	CMP		X2, #0
	B.EQ	1f
    PUSH_CALLER_SAVE_FPU_REGS SP

1:
	/* Store the FPU context indicator. */
	STR 	x2, [SP, #-0x10]!

	LDR 	X0, pxCurrentTCBConst

    /* Get coreId and choose the corresponding index for core in pxCurrentTCB */
    MRS     X1, MPIDR_EL1
    AND     X1, X1, #0xff
    CMP     X1, #0
    B.EQ    2f
    ADD     X0, X0, #8

2:
	LDR 	X1, [X0]
	MOV 	X0, SP   /* Move SP into X0 for saving. */
	STR 	X0, [X1]

	/* Switch to use the ELx stack pointer. */
	MSR 	SPSEL, #1

.endm

; /**********************************************************************/

.macro portRESTORE_CONTEXT

	/* Switch to use the EL0 stack pointer. */
	MSR 	SPSEL, #0

	/* Set the SP to point to the stack of the task being restored. */
	LDR		X0, pxCurrentTCBConst

    /* Get coreId and choose the corresponding index for core in pxCurrentTCB */
    MRS     X1, MPIDR_EL1
    AND     X1, X1, #0xff
    CMP     X1, #0
    B.EQ    2f
    ADD     X0, X0, #8

2:
	LDR		X1, [X0]
	LDR		X0, [X1]
	MOV		SP, X0

	LDR 	X2, [SP], #0x10  /* FPU context. */

	/* Set the PMR register to be correct for the current critical nesting
	depth. */
	MOV		X1, #255					/* X1 holds the unmask value. */

	MSR		s3_0_c4_c6_0, X1 			/* Write the mask value to ICCPMR. s3_0_c4_c6_0 is ICC_PMR_EL1. */
	DSB 	SY							/* _RB_Barriers probably not required here. */
	ISB 	SY

    /* Get coreId */
    MRS     X1, MPIDR_EL1
    AND     X1, X1, #0xff
    LDR		X0, ullPortTaskHasFPUContextConst
    BEQ     3f
    ADD     X0, X0, #8
3:

	/* Restore the FPU context indicator. */
	STR		X2, [X0]

	/* Restore the FPU context, if any. */
	CMP		X2, #0
	B.EQ	1f
    POP_CALLER_SAVE_FPU_REGS SP
1:
	LDP 	X2, X3, [SP], #0x10  /* SPSR and ELR. */

	/* Restore the SPSR. */
	MSR		SPSR_EL1, X3
	/* Restore the ELR. */
	MSR		ELR_EL1, X2

    POP_ALL_CPU_REGS SP

	/* Switch to use the ELx stack pointer.  _RB_ Might not be required. */
	MSR 	SPSEL, #1

	ERET

.endm

.macro VECTOR_ENTRY name
        .align  7
\name:
.endm

    .global HwiP_gicv3Vectors
    .section .vecs, "ax"
    .align  11
HwiP_gicv3Vectors:

/*
 *************************************************************************
 * Exception from currentEL, using SP0
 *************************************************************************
 */
VECTOR_ENTRY el1SyncSP0
    B HwiP_SVC_Handler

VECTOR_ENTRY el1IrqSP0
    B HwiP_IRQ_Handler

VECTOR_ENTRY el1FiqSP0
    b       el1FiqSP0

VECTOR_ENTRY el1SErrorSP0
    PUSH_ALL_CPU_REGS sp        /* save all CPU regs */
    mrs     x0, sp_el0
    mrs     x1, elr_el1
    mrs     x2, spsr_el1
    mrs     x3, esr_el1
    stp     x0, x1, [sp, #-16]! /* save sp & elr */
    stp     x2, x3, [sp, #-16]! /* save spsr and esr */
    mov     x0, sp
    mov     x1, #1
    ldr     x2, =HwiP_defaultExcHandler
    br      x2

/*
 *************************************************************************
 * Exception from currentEL, using SPx
 *************************************************************************
 */
VECTOR_ENTRY el1SyncSPx
    B HwiP_SVC_Handler

VECTOR_ENTRY el1IrqSPx
    B HwiP_IRQ_Handler

VECTOR_ENTRY el1FiqSPx
    b    el1FiqSPx

VECTOR_ENTRY el1SErrorSPx
    PUSH_ALL_CPU_REGS sp        /* save all CPU regs */
    add     x0, sp, #256        /* compute original SP */
    mrs     x1, elr_el1
    mrs     x2, spsr_el1
    mrs     x3, esr_el1
    stp     x0, x1, [sp, #-16]! /* save sp & elr */
    stp     x2, x3, [sp, #-16]! /* save spsr and esr */
    mov     x0, sp
    mov     x1, #1
    ldr     x2, =HwiP_defaultExcHandler
    br      x2

/*
 *************************************************************************
 * Exception from lowerEL, all lowerEL using Aarch64
 *************************************************************************
 */
VECTOR_ENTRY el0SyncAarch64
    b    el0SyncAarch64

VECTOR_ENTRY el0IrqAarch64
    b    el0IrqAarch64

VECTOR_ENTRY el0FiqAarch64
    b    el0FiqAarch64

VECTOR_ENTRY el0SErrorAarch64
    b    el0SErrorAarch64

/*
 *************************************************************************
 * Exception from lowerEL, all lowerEL using Aarch32
 *************************************************************************
 */
VECTOR_ENTRY el0SyncAarch32
    b    el0SyncAarch32

VECTOR_ENTRY el0IrqAarch32
    b    el0IrqAarch32

VECTOR_ENTRY el0FiqAarch32
    b    el0FiqAarch32

VECTOR_ENTRY el0SErrorAarch32
    b    el0SErrorAarch32


/******************************************************************************
 * vPortRestoreTaskContext is used to start the scheduler.
 *****************************************************************************/
.align 8
.type vPortRestoreTaskContext, %function
vPortRestoreTaskContext:
	DSB		SY
	ISB		SY
	/* Start the first task. */
	portRESTORE_CONTEXT

/******************************************************************************
 * handles SVC entry and exit.
 *****************************************************************************/
.align 8
.type HwiP_SVC_Handler, %function
HwiP_SVC_Handler:
    /* Save the context of the current task and select a new task to run. */
	portSAVE_CONTEXT
	MRS		X0, ESR_EL1
    MRS     X2, ELR_EL1
	LSR		X1, X0, #26
	CMP		X1, #0x15 	/* 0x15 = SVC instruction. */
	B.NE	HwiP_SVC_Abort
    MRS     x0, mpidr_el1       /* Get CoreID */
    and     x0, x0, #0xff
	BL 		vTaskSwitchContext
	portRESTORE_CONTEXT
HwiP_SVC_Abort:
	/* Full ESR is in X0, exception class code is in X1. */
	B		.

/******************************************************************************
 * handles IRQ entry and exit.
 *****************************************************************************/
.align 8
.type HwiP_IRQ_Handler, %function
HwiP_IRQ_Handler:
    /* save cpu scratch regs */
    PUSH_CALLER_SAVE_CPU_REGS SP

	/* Save the SPSR and ELR. */
	MRS		X3, SPSR_EL1
	MRS		X2, ELR_EL1
	STP 	X2, X3, [SP, #-0x10]!

	/* Increment the interrupt nesting counter. */
	LDR		X5, ullPortInterruptNestingConst

    /* Get coreId and choose the corresponding index for core in ullPortInterruptNesting */
    MRS     X1, MPIDR_EL1
    AND     X1, X1, #0xff
    CMP     X1, #0
    B.EQ    2f
    ADD     X5, X5, #8
2:
	LDR		X1, [X5]	/* Old nesting count in X1. */
	ADD		X6, X1, #1
	STR		X6, [X5]	/* Address of nesting count variable in X5. */

	/* Maintain the interrupt nesting information across the function call. */
	STP		X1, X5, [SP, #-0x10]!

	/* Call the C handler. */
	BL HwiP_intrHandler

	/* Disable interrupts. */
	MSR 	DAIFSET, #2
	DSB		SY
	ISB		SY

	/* Restore the critical nesting count. */
	LDP		X1, X5, [SP], #0x10
	STR		X1, [X5]

	/* Has interrupt nesting unwound? */
	CMP		X1, #0
	B.NE	Exit_IRQ_No_Context_Switch

	/* Is a context switch required? */
	LDR		X0, ullPortYieldRequiredConst

    /* Get coreId and choose the corresponding index for core in ullPortYieldRequired */
    MRS     X1, MPIDR_EL1
    AND     X1, X1, #0xff
    CMP     X1, #0
    B.EQ    2f
    ADD     X0, X0, #8
2:
	LDR		X1, [X0]
	CMP		X1, #0
	B.EQ	Exit_IRQ_No_Context_Switch

	/* Reset ullPortYieldRequired to 0. */
	MOV		X2, #0
	STR		X2, [X0]

	/* Restore volatile registers. */
	LDP 	X4, X5, [SP], #0x10  /* SPSR and ELR. */
	MSR		SPSR_EL1, X5
	MSR		ELR_EL1, X4
	DSB		SY
	ISB		SY

    POP_CALLER_SAVE_CPU_REGS SP

	/* Save the context of the current task and select a new task to run. */
	portSAVE_CONTEXT
    MRS     x0, mpidr_el1       /* Get CoreID */
    and     x0, x0, #0xff
	BL vTaskSwitchContext
	portRESTORE_CONTEXT

Exit_IRQ_No_Context_Switch:
	/* Restore volatile registers. */
	LDP 	X4, X5, [SP], #0x10  /* SPSR and ELR. */
	MSR		SPSR_EL1, X5
	MSR		ELR_EL1, X4
	DSB		SY
	ISB		SY

    POP_CALLER_SAVE_CPU_REGS SP

	ERET

/*
 *  Void HwiP_defaultExcHandler(uint32_t usingEL0Stack);
 */
        .section .text.HwiP_defaultExcHandler, "ax"
        .func HwiP_defaultExcHandler
HwiP_defaultExcHandler:

        mov     x19, x0              /* copy 'usingEL0Stack' argument */
        mrs     x0, spsr_el1
        mrs     x1, elr_el1
        stp     x0, x1, [sp, #-16]!  /* save spsr_el1 and elr_el1 */
#if defined(__ARM_FP)
        mrs     x0, fpcr
        mrs     x1, fpsr
        stp     x0, x1, [sp, #-16]!  /* save fpcr and fpsr */
#endif

        /* call dispatch default handler */
        mrs     x0, elr_el1          /* pass IRP to dispatchIRQC() */
        ldr     x1, =HwiP_defaultHandler
        blr     x1

                                    /* returns with interrupts disabled */
#if defined(__ARM_FP)
        ldp     x0, x1, [sp], #16
        msr     fpcr, x0
        msr     fpsr, x1
#endif
        ldp     x0, x1, [sp], #16
        msr     spsr_el1, x0
        msr     elr_el1, x1
#if defined(__ARM_FP)
        POP_CALLER_SAVE_FPU_REGS sp  /* restore vfp scratch regs */
#endif
        POP_CALLER_SAVE_CPU_REGS sp  /* restore cpu scratch regs */
        eret

.align 8

/*
 *  int32_t GateSmp_tryLock(uintptr_t gateWord);
 */
        .global GateSmp_tryLock
        .type GateSmp_tryLock  , %function
GateSmp_tryLock:
        ldxr    w1, [x0]
        cmp     w1, #1  /* is locked already? */
        beq     1f      /* if so, leave with fail */

        mov     w2, #1  /* locked = 1 */
        stxr    w1, w2, [x0] /* if so attempt to grab it */
        cmp     w1, #0       /* did we get it? zero is yes */
        /* if not, loop while in contention */
        bne     GateSmp_tryLock
1:
        mov     w0, w1
        dmb     sy
        ret

/*
 *  void GateSmp_unlock(uintptr_t gateWord);
 */
        .global GateSmp_unlock
        .type GateSmp_unlock  , %function

GateSmp_unlock:
        dmb     sy

        mov     w1, #0
        str     w1, [x0]

        dsb     sy
        sev         /* let everyone know */
        ret

pxCurrentTCBConst: .dword pxCurrentTCBs
ullPortTaskHasFPUContextConst: .dword ullPortTaskHasFPUContext
ullPortInterruptNestingConst: .dword ullPortInterruptNesting
ullPortYieldRequiredConst: .dword ullPortYieldRequired

.end
