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
 * XTENSA CONTEXT FRAMES AND MACROS FOR RTOS ASSEMBLER SOURCES
 *
 * This header contains definitions and macros for use primarily by Xtensa
 * RTOS assembly source files. It includes and uses the Xtensa hardware
 * abstraction layer (HAL) to deal with config specifics. It may also be
 * included in C source files.
 *
 * NOTE: The Xtensa architecture requires stack pointer alignment to 16 bytes.
 */

#ifndef XTENSA_CONTEXT_H
#define XTENSA_CONTEXT_H

#ifdef __ASSEMBLER__
#include    <xtensa/coreasm.h>
#endif

#include    <xtensa/config/tie.h>
#include    <xtensa/corebits.h>
#include    <xtensa/config/system.h>


/* Align a value up to nearest n-byte boundary, where n is a power of 2. */
#ifdef __ASSEMBLER__
#define ALIGNUP(n, val) (((val) + (n)-1) & -(n))
#else
#define ALIGNUP(n, val) (((val) + (n)-1) & (unsigned)-(n))
#endif


/* SYSCALL definitions for privilige support */
#define SYSCALL_raise_priv  10
#define SYSCALL_is_priv     12


/*
-------------------------------------------------------------------------------
  Macros that help define structures for both C and assembler.
-------------------------------------------------------------------------------
*/
#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)

#define XSTRUCT_BEGIN            .pushsection .text; .struct 0
#define XSTRUCT_FIELD(ctype,size,asname,name)    asname: .space  size
#define XSTRUCT_AFIELD(ctype,size,asname,name,n) asname: .space  (size)*(n)
#define XSTRUCT_END(sname)       sname##Size:; .popsection

#else

#define XSTRUCT_BEGIN            typedef struct {
#define XSTRUCT_FIELD(ctype,size,asname,name)    ctype   name;
#define XSTRUCT_AFIELD(ctype,size,asname,name,n) ctype   name[n];
#define XSTRUCT_END(sname)       } sname;

#endif //_ASMLANGUAGE || __ASSEMBLER__


/*
-------------------------------------------------------------------------------
  INTERRUPT/EXCEPTION STACK FRAME FOR A THREAD OR NESTED INTERRUPT

  A stack frame of this structure is allocated for any interrupt or exception.
  It goes on the current stack. If the RTOS has a system stack for handling
  interrupts, every thread stack must allow space for just one interrupt stack
  frame, then nested interrupt stack frames go on the system stack.

  The frame includes basic registers (explicit) and "extra" registers introduced
  by user TIE or the use of the MAC16 option in the user's Xtensa config.
  The frame size is minimized by omitting regs not applicable to user's config.

  For Windowed ABI, this stack frame includes the interruptee's base save area,
  another base save area to manage gcc nested functions, and a little temporary
  space to help manage the spilling of the register windows.
-------------------------------------------------------------------------------
*/

#if XCHAL_HAVE_XEA2

XSTRUCT_BEGIN
XSTRUCT_FIELD (long, 4, XT_STK_EXIT,     exit) /* exit point for dispatch */
XSTRUCT_FIELD (long, 4, XT_STK_PC,       pc)   /* return PC */
XSTRUCT_FIELD (long, 4, XT_STK_PS,       ps)   /* return PS */
XSTRUCT_FIELD (long, 4, XT_STK_A0,       a0)
XSTRUCT_FIELD (long, 4, XT_STK_A1,       a1)   /* stack pointer before interrupt */
XSTRUCT_FIELD (long, 4, XT_STK_A2,       a2)
XSTRUCT_FIELD (long, 4, XT_STK_A3,       a3)
XSTRUCT_FIELD (long, 4, XT_STK_A4,       a4)
XSTRUCT_FIELD (long, 4, XT_STK_A5,       a5)
XSTRUCT_FIELD (long, 4, XT_STK_A6,       a6)
XSTRUCT_FIELD (long, 4, XT_STK_A7,       a7)
XSTRUCT_FIELD (long, 4, XT_STK_A8,       a8)
XSTRUCT_FIELD (long, 4, XT_STK_A9,       a9)
XSTRUCT_FIELD (long, 4, XT_STK_A10,      a10)
XSTRUCT_FIELD (long, 4, XT_STK_A11,      a11)
XSTRUCT_FIELD (long, 4, XT_STK_A12,      a12)
XSTRUCT_FIELD (long, 4, XT_STK_A13,      a13)
XSTRUCT_FIELD (long, 4, XT_STK_A14,      a14)
XSTRUCT_FIELD (long, 4, XT_STK_A15,      a15)
XSTRUCT_FIELD (long, 4, XT_STK_SAR,      sar)
XSTRUCT_FIELD (long, 4, XT_STK_EXCCAUSE, exccause)
XSTRUCT_FIELD (long, 4, XT_STK_EXCVADDR, excvaddr)
#if XCHAL_HAVE_LOOPS
XSTRUCT_FIELD (long, 4, XT_STK_LBEG,     lbeg)
XSTRUCT_FIELD (long, 4, XT_STK_LEND,     lend)
XSTRUCT_FIELD (long, 4, XT_STK_LCOUNT,   lcount)
#endif
#if XCHAL_HAVE_EXCLUSIVE
XSTRUCT_FIELD (long, 4, XT_STK_ATOMCTL,  atomctl)
#endif
#ifndef __XTENSA_CALL0_ABI__
/* Temporary space for saving stuff during window spill */
XSTRUCT_FIELD (long, 4, XT_STK_TMP0,     tmp0)
XSTRUCT_FIELD (long, 4, XT_STK_TMP1,     tmp1)
XSTRUCT_FIELD (long, 4, XT_STK_TMP2,     tmp2)
#endif
#ifdef XT_USE_SWPRI
/* Storage for virtual priority mask */
XSTRUCT_FIELD (long, 4, XT_STK_VPRI,     vpri)
#endif
#ifdef XT_USE_OVLY
/* Storage for overlay state */
XSTRUCT_FIELD (long, 4, XT_STK_OVLY,         ovly)
XSTRUCT_FIELD (long, 4, XT_STK_OVLY_SCRATCH, ovly_scratch)
#endif
XSTRUCT_END(XtExcFrame)

#endif /* XCHAL_HAVE_XEA2 */

#if XCHAL_HAVE_XEA3

XSTRUCT_BEGIN
#if defined(__XTENSA_WINDOWED_ABI__) && portUSING_MPU_WRAPPERS
/* Temporary space for syscall argument/return value */
XSTRUCT_FIELD (long, 4, XT_STK_A2,       a2)
#endif
XSTRUCT_FIELD (long, 4, XT_STK_ATOMCTL,  atomctl)
XSTRUCT_FIELD (long, 4, XT_STK_LCOUNT,   lcount)
XSTRUCT_FIELD (long, 4, XT_STK_LEND,     lend)
XSTRUCT_FIELD (long, 4, XT_STK_LBEG,     lbeg)
XSTRUCT_FIELD (long, 4, XT_STK_PC,       pc)
XSTRUCT_FIELD (long, 4, XT_STK_PS,       ps)
XSTRUCT_FIELD (long, 4, XT_STK_EXCCAUSE, exccause)
XSTRUCT_FIELD (long, 4, XT_STK_EXCVADDR, excvaddr)
#ifdef __XTENSA_CALL0_ABI__
XSTRUCT_FIELD (long, 4, XT_STK_A0,     a0)
XSTRUCT_FIELD (long, 4, XT_STK_A1,     a1)
XSTRUCT_FIELD (long, 4, XT_STK_A2,     a2)
XSTRUCT_FIELD (long, 4, XT_STK_A3,     a3)
XSTRUCT_FIELD (long, 4, XT_STK_A4,     a4)
XSTRUCT_FIELD (long, 4, XT_STK_A5,     a5)
XSTRUCT_FIELD (long, 4, XT_STK_A6,     a6)
XSTRUCT_FIELD (long, 4, XT_STK_A7,     a7)
#endif
XSTRUCT_FIELD (long, 4, XT_STK_A8,     a8)
XSTRUCT_FIELD (long, 4, XT_STK_A9,     a9)
XSTRUCT_FIELD (long, 4, XT_STK_A10,    a10)
XSTRUCT_FIELD (long, 4, XT_STK_A11,    a11)
XSTRUCT_FIELD (long, 4, XT_STK_A12,    a12)
XSTRUCT_FIELD (long, 4, XT_STK_A13,    a13)
XSTRUCT_FIELD (long, 4, XT_STK_A14,    a14)
XSTRUCT_FIELD (long, 4, XT_STK_A15,    a15)
#ifdef __XTENSA_WINDOWED_ABI__
XSTRUCT_AFIELD(long, 4, XT_STK_SPILL,  spill, 8)
#endif
XSTRUCT_END(XtExcFrame)

#endif /* XCHAL_HAVE_XEA3 */

#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)
#define XT_STK_NEXT1      XtExcFrameSize
#else
#define XT_STK_NEXT1      sizeof(XtExcFrame)
#endif


/* Allocate extra storage if needed for non-CP TIE state. Allow for alignment
   padding as needed.
 */
#if XCHAL_EXTRA_SA_SIZE != 0

#if XCHAL_EXTRA_SA_ALIGN <= 16
#define XT_STK_EXTRA            ALIGNUP(XCHAL_EXTRA_SA_ALIGN, XT_STK_NEXT1)
#else
/* If need more alignment than stack, add space for dynamic alignment */
#define XT_STK_EXTRA            (ALIGNUP(XCHAL_EXTRA_SA_ALIGN, XT_STK_NEXT1) + XCHAL_EXTRA_SA_ALIGN)
#endif
#define XT_STK_NEXT2            (XT_STK_EXTRA + XCHAL_EXTRA_SA_SIZE)

#else

/* No extra storage required */
#define XT_STK_NEXT2            XT_STK_NEXT1   

#endif


#if XCHAL_HAVE_XEA3

/* Total frame size */
#define XT_STK_FRMSZ            (ALIGNUP(0x10, XT_STK_NEXT2))

/* Exception/interrupt frame size */
#define XT_STK_XFRM_SZ          XT_STK_NEXT1

/* Extra save area size (including alignment padding) */
#define XT_STK_XTRA_SZ          (XT_STK_FRMSZ - XT_STK_XFRM_SZ)

/* Alignment padding area size */
#if XCHAL_EXTRA_SA_SIZE != 0
#define XT_STK_ALIGN_SZ         (XT_STK_EXTRA - XT_STK_NEXT1)
#endif

#else

/*
-------------------------------------------------------------------------------
  This is the frame size. Add space for 4 registers (interruptee's base save
  area) and some space for gcc nested functions if any.
-------------------------------------------------------------------------------
*/
#define XT_STK_FRMSZ            (ALIGNUP(0x10, XT_STK_NEXT2) + 0x20)

#endif

/*
-------------------------------------------------------------------------------
  CO-PROCESSOR STATE SAVE AREA FOR A THREAD

  The RTOS must provide an area per thread to save the state of co-processors
  when that thread does not have control. Co-processors are context-switched
  lazily (on demand) only when a new thread uses a co-processor instruction,
  otherwise a thread retains ownership of the co-processor even when it loses
  control of the processor. An Xtensa co-processor exception is triggered when
  any co-processor instruction is executed by a thread that is not the owner,
  and the context switch of that co-processor is then peformed by the handler.
  Ownership represents which thread's state is currently in the co-processor.

  Co-processors may not be used by interrupt or exception handlers. If an
  co-processor instruction is executed by an interrupt or exception handler,
  the co-processor exception handler will trigger a kernel panic and freeze.
  This restriction is introduced to reduce the overhead of saving and restoring
  co-processor state (which can be quite large) and in particular remove that
  overhead from interrupt handlers.

  The co-processor state save area may be in any convenient per-thread location
  such as in the thread control block or above the thread stack area. It need
  not be in the interrupt stack frame since interrupts don't use co-processors.

  Along with the save area for each co-processor, two bitmasks with flags per
  co-processor (laid out as in the CPENABLE reg) help manage context-switching
  co-processors as efficiently as possible:

  XT_CPENABLE
    The contents of a non-running thread's CPENABLE register.
    It represents the co-processors owned (and whose state is still needed)
    by the thread. When a thread is preempted, its CPENABLE is saved here.
    When a thread solicits a context-swtich, its CPENABLE is cleared - the
    compiler has saved the (caller-saved) co-proc state if it needs to.
    When a non-running thread loses ownership of a CP, its bit is cleared.
    When a thread runs, it's XT_CPENABLE is loaded into the CPENABLE reg.
    Avoids co-processor exceptions when no change of ownership is needed.

  XT_CPSTORED
    A bitmask with the same layout as CPENABLE, a bit per co-processor.
    Indicates whether the state of each co-processor is saved in the state
    save area. When a thread enters the kernel, only the state of co-procs
    still enabled in CPENABLE is saved. When the co-processor exception
    handler assigns ownership of a co-processor to a thread, it restores
    the saved state only if this bit is set, and clears this bit.

  XT_CP_CS_ST
    A bitmask with the same layout as CPENABLE, a bit per co-processor.
    Indicates whether callee-saved state is saved in the state save area.
    Callee-saved state is saved by itself on a solicited context switch,
    and restored when needed by the coprocessor exception handler.
    Unsolicited switches will cause the entire coprocessor to be saved
    when necessary.

  XT_CP_ASA
    Pointer to the aligned save area.  Allows it to be aligned more than
    the overall save area (which might only be stack-aligned or TCB-aligned).
    Especially relevant for Xtensa cores configured with a very large data
    path that requires alignment greater than 16 bytes (ABI stack alignment).
-------------------------------------------------------------------------------
*/

#if XCHAL_CP_NUM > 0

/*  Offsets of each coprocessor save area within the 'aligned save area':  */
#define XT_CP0_SA   0
#define XT_CP1_SA   ALIGNUP(XCHAL_CP1_SA_ALIGN, XT_CP0_SA + XCHAL_CP0_SA_SIZE)
#define XT_CP2_SA   ALIGNUP(XCHAL_CP2_SA_ALIGN, XT_CP1_SA + XCHAL_CP1_SA_SIZE)
#define XT_CP3_SA   ALIGNUP(XCHAL_CP3_SA_ALIGN, XT_CP2_SA + XCHAL_CP2_SA_SIZE)
#define XT_CP4_SA   ALIGNUP(XCHAL_CP4_SA_ALIGN, XT_CP3_SA + XCHAL_CP3_SA_SIZE)
#define XT_CP5_SA   ALIGNUP(XCHAL_CP5_SA_ALIGN, XT_CP4_SA + XCHAL_CP4_SA_SIZE)
#define XT_CP6_SA   ALIGNUP(XCHAL_CP6_SA_ALIGN, XT_CP5_SA + XCHAL_CP5_SA_SIZE)
#define XT_CP7_SA   ALIGNUP(XCHAL_CP7_SA_ALIGN, XT_CP6_SA + XCHAL_CP6_SA_SIZE)
#define XT_CP_SA_SIZE   ALIGNUP(16, XT_CP7_SA + XCHAL_CP7_SA_SIZE)

/*  Offsets within the overall save area:  */
#define XT_CPENABLE 0   /* (2 bytes) coprocessors active for this thread */
#define XT_CPSTORED 2   /* (2 bytes) coprocessors saved for this thread */
#define XT_CP_CS_ST 4   /* (2 bytes) coprocessor callee-saved regs stored for this thread */
#define XT_CP_ASA   8   /* (4 bytes) ptr to aligned save area */
/*  Overall size allows for dynamic alignment:  */
#define XT_CP_SIZE  (12 + XT_CP_SA_SIZE + XCHAL_TOTAL_SA_ALIGN)
#else
#define XT_CP_SIZE  0
#endif


/*
-------------------------------------------------------------------------------
  MACROS TO HANDLE ABI SPECIFICS OF FUNCTION ENTRY AND RETURN

  Convenient where the frame size requirements are the same for both ABIs.
    ENTRY(sz), RET(sz) are for framed functions (have locals or make calls).
    ENTRY0,    RET0    are for frameless functions (no locals, no calls).

  where size = size of stack frame in bytes (must be >0 and aligned to 16).
  For framed functions the frame is created and the return address saved at
  base of frame (Call0 ABI) or as determined by hardware (Windowed ABI).
  For frameless functions, there is no frame and return address remains in a0.
  Note: Because CPP macros expand to a single line, macros requiring multi-line
  expansions are implemented as assembler macros.
-------------------------------------------------------------------------------
*/

#ifdef __ASSEMBLER__
#ifdef __XTENSA_CALL0_ABI__
  /* Call0 */
  #define ENTRY(sz)     entry1  sz
    .macro  entry1 size=0x10
    addi    sp, sp, -\size
    s32i    a0, sp, 0
    .endm
  #define ENTRY0
  #define RET(sz)       ret1    sz
    .macro  ret1 size=0x10
    l32i    a0, sp, 0
    addi    sp, sp, \size
    ret
    .endm
  #define RET0          ret
#else
  /* Windowed */
#if XCHAL_HAVE_XEA3
  #define ENTRY(sz)     entry   sp, (sz + 0x20)
  #define ENTRY0        entry   sp, 0x20
#else
  #define ENTRY(sz)     entry   sp, (sz + 0x10)
  #define ENTRY0        entry   sp, 0x10
#endif
  #define RET(sz)       retw
  #define RET0          retw
#endif
#endif


/*
-------------------------------------------------------------------------------
  This flag is meant for internal use. Have all interrupts be dispatched via a
  common wrapper, which takes care of doing some OS-specific stuff common to
  all interrupt handlers. Said stuff cannot safely be handled in the RTOS_ENTER
  and RTOS_EXIT macros.
-------------------------------------------------------------------------------
*/
#if (defined XT_INTEXC_HOOKS)
#define XT_USE_INT_WRAPPER    1
#else
#define XT_USE_INT_WRAPPER    0
#endif

#if XCHAL_HAVE_XEA2 && (XCHAL_NUM_INTERRUPTS > 32) && (defined XT_USE_SWPRI)
#error "Software prioritization of interrupts (XT_USE_SWPRI) not supported for XEA2 with > 32 interrupts."
#endif

#if XCHAL_HAVE_XEA3
#ifdef XT_USE_SWPRI
//#warning "Software prioritization of interrupts (XT_USE_SWPRI) not supported for XEA3."
#endif

#ifdef __ASSEMBLER__

#include "asm-offsets.h"

    // RTOS-specific entry macro. Use only a8, a12-a14.

    .macro  XT_RTOS_INT_ENTER

    .endm

    // RTOS-specific exit macro. Use only a8-a14.
    // (In call0, a15 holds user SP, must be preserved)

    .macro  XT_RTOS_INT_EXIT

    // Check scheduler state and interrupt nest state.

    movi     a8,  port_xSchedulerRunning
    movi     a9,  port_interruptNesting
    l32i     a8,  a8, 0                         // a8 <- port_xSchedulerRunning
    beqz     a8,  .Lnested                      // scheduler not running, no tasks
    l32i     a8,  a9, 0                         // a8 <- port_interruptNesting
    bnez     a8,  .Lnested                      // != 0 means nested, skip ahead
    movi     a8,  port_yield_flag
    l32i     a9,  a8, 0                         // a9 <- port_yield_flag
    beqz     a9,  2f                            // no yield
    movi     a9,  0
    s32i     a9,  a8, 0                         // zero out for next time
    j        .Lyield                            // no context save needed
2:
    movi     a8,  pxCurrentTCB
    l32i     a9,  a8, 0                         // a9 <- pxCurrentTCB
    beqz     a9,  .Lsched                       // no current, go to scheduler
    movi    a10,  port_switch_flag
    l32i    a11, a10, 0                         // a11 <- port_switch_flag
    beqz    a11,  .Lnested                      // = 0 means no switch
    movi    a11,  0
    s32i    a11, a10, 0                         // zero out for next time

    // Preemption, save remaining state of current (outgoing) thread

    addi    a10,  a1, -XT_STK_FRMSZ
    s32i    a10,  a9, TCB_TOP_OF_STACK_OFF      // pxCurrentTCB->pxTopOfStack <- SP

    // Mark as preempted

#if XCHAL_CP_NUM > 0
    l32i    a10,  a9, TCB_END_OF_STACK_OFF
    rsr     a12,  CPENABLE                      // Save and clear CPENABLE
    s16i    a12, a10, XT_CPENABLE
    movi    a12,  0
    wsr     a12,  CPENABLE
#endif

    // Save non-CP TIE state if any
#if XCHAL_EXTRA_SA_SIZE > 0
    addi    a10,  a1, -XT_STK_FRMSZ + XT_STK_ALIGN_SZ    // where to save
#if XCHAL_EXTRA_SA_ALIGN > 16
    movi    a12, -XCHAL_EXTRA_SA_ALIGN
    and     a10, a10, a12                       // align dynamically >16 bytes
#endif
    xchal_ncp_store a10, a11, a12, a13, a14
#endif

    // If windowed ABI, a0-a7 was not saved by dispatch code.
    // These don't go into the exception frame, but below the
    // exception frame pointer.

#ifdef __XTENSA_WINDOWED_ABI__
    addi    a10,  a1, -XT_STK_FRMSZ -32
    s32i     a0, a10, 0
    s32i     a2, a10, 8
    s32i     a3, a10, 12
    s32i     a4, a10, 16
    s32i     a5, a10, 20
    s32i     a6, a10, 24
    s32i     a7, a10, 28
#endif

.Lsched:
    // When we get here, interrupts are disabled. Also, we have saved
    // all registers if needed so all of a0-a15 are available to use.
    // Note we are using the existing stack to make the call below.

    addi     a1,  a1, -XT_STK_FRMSZ -32
#ifdef __XTENSA_WINDOWED_ABI__
    movi    a10, vTaskSwitchContext
    callx8  a10
#else
    movi    a10, vTaskSwitchContext
    callx0  a10
#endif

.Lyield:
    // Come here directly if the outgoing task yielded. pxCurrentTCB
    // has already been updated.

    movi     a9,  pxCurrentTCB
    l32i     a9,  a9, 0                         // a9 <- pxCurrentTCB

#if XCHAL_CP_NUM > 0
    l32i    a10,  a9, TCB_END_OF_STACK_OFF
    l16ui   a10,  a10, XT_CPENABLE
    wsr     a10,  CPENABLE                      // restore CPENABLE
#endif

    l32i     a1,  a9, TCB_TOP_OF_STACK_OFF      // a1 <- pxCurrentTCB->pxTopOfStack
    addi    a10,  a1, XT_STK_FRMSZ - XT_STK_XFRM_SZ
    j 1f //bbci    a10,  31, 1f                        // Bit 31 = solicited flag

    // Solicited restore
    addi     a1,  a1, XT_STK_FRMSZ              // restore original SP
    addi     a2,  a1, -XT_STK_XFRM_SZ
    l32i     a3,  a2, XT_STK_PC                 // a3 = return PC

    movi     a2, PS_DI
    xps  a2, a2                                 // disable interrupts
    movi     a2, 0
    wsr.ms   a2                                 // restore normal DISPST
    rsync
    jx   a3                                     // return to yield point

1:
    // Preempt restore
#if XCHAL_EXTRA_SA_SIZE > 0
    addi    a10,  a1, XT_STK_ALIGN_SZ           // where to restore from
#if XCHAL_EXTRA_SA_ALIGN > 16
    movi    a12, -XCHAL_EXTRA_SA_ALIGN
    and     a10, a10, a12                       // align dynamically >16 bytes
#endif
    xchal_ncp_load a10, a11, a12, a13, a14
#endif

#ifdef __XTENSA_WINDOWED_ABI__
    // Restore a0-a7 for windowed ABI
    addi     a8,  a1, -32
    l32i     a0,  a8, 0
    l32i     a2,  a8, 8
    l32i     a3,  a8, 12
    l32i     a4,  a8, 16
    l32i     a5,  a8, 20
    l32i     a6,  a8, 24
    l32i     a7,  a8, 28
#else
    // For call0, dispatch exit code expects a15 = original SP
    addi    a15,  a1, XT_STK_FRMSZ
#endif

    addi     a1,  a1, XT_STK_FRMSZ              // set up SP for dispatch exit

.Lnested:
    // Return to the current saved context.

    .endm

#endif /* __ASSEMBLER__ */
#endif /* XCHAL_HAVE_XEA3 */

#endif /* XTENSA_CONTEXT_H */
