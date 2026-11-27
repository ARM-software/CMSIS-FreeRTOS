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

#include "xtensa_rtos.h"
#include "asm-offsets.h"
#include "portmacro.h"
#include "xtensa_asm.h"


#if XCHAL_HAVE_XEA3
    .data
    .align      16
    .global     port_yield_flag
port_yield_flag:
    .word       0
#endif

#if (XCHAL_CP_NUM > 0) && (configNUMBER_OF_CORES > 1) && (configUSE_CORE_AFFINITY == 1)
    /* Keep some affinity bitmasks to quickly decide whether to save CP state */
    .data
port_affinity_masks:
    .word       ~0x1
    .word       ~0x2
 #if ( configNUMBER_OF_CORES > 2 )
    .word       ~0x4
    .word       ~0x8
  #if ( configNUMBER_OF_CORES > 4 )
    .word       ~0x10
    .word       ~0x20
    .word       ~0x40
    .word       ~0x80
  #endif
 #endif
#endif


#if portUSING_MPU_WRAPPERS
    .section "privileged_functions"
#else
    .text
#endif


/*
*******************************************************************************
* _frxt_setup_switch
* void _frxt_setup_switch(void);
*
* Sets an internal flag indicating that a task switch is required on return
* from interrupt handling.
*
*******************************************************************************
*/
    .global     _frxt_setup_switch
    .type       _frxt_setup_switch,@function
    .align      4
_frxt_setup_switch:

    ENTRY(16)

    pintdata a2, a3
    movi    a3, 1
    s32i    a3, a2, PORTINT_SWITCH_OFF /* port_switch_flag = 1            */

    RET(16)

    .size _frxt_setup_switch, . - _frxt_setup_switch

#if XCHAL_HAVE_XEA2

/*
*******************************************************************************
*                                            _frxt_int_enter
*                                       void _frxt_int_enter(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_INT_ENTER function for
* freeRTOS. Saves the rest of the interrupt context (not already saved).
* May only be called from assembly code by the 'call0' instruction, with
* interrupts disabled.
* See the detailed description of the XT_RTOS_ENTER macro in xtensa_rtos.h.
*
*******************************************************************************
*/
    .globl  _frxt_int_enter
    .type   _frxt_int_enter,@function
    .align  4
_frxt_int_enter:

    /* Save a12-13 in the stack frame as required by _xt_context_save. */
    s32i    a12, a1, XT_STK_A12
    s32i    a13, a1, XT_STK_A13

    /* Save return address in a safe place (free a0). */
    mov     a12, a0

    /* Save the rest of the interrupted context (preserves A12-13). */
    call0   _xt_context_save

    /*
    Save interrupted task's SP in TCB only if not nesting.
    Manage nesting directly rather than call the generic IntEnter()
    (in windowed ABI we can't call a C function here anyway because PS.EXCM is still set).
    */
    pintdata a3, a2                     /* a3 <- &port_interruptNesting    */
    movi    a2,  port_xSchedulerRunning
    l32i    a2,  a2, 0                  /* a2 = port_xSchedulerRunning     */
    beqz    a2,  1f                     /* scheduler not running, no tasks */
    l32i    a2,  a3, PORTINT_NEST_OFF   /* a2 = port_interruptNesting      */
    addi    a2,  a2, 1                  /* increment nesting count         */
    s32i    a2,  a3, PORTINT_NEST_OFF   /* save nesting count              */
    bnei    a2,  1, .Lnested            /* !=0 before incr, so nested      */

    pxctcb  a2,  a3                     /* pxCurrentTCB or pxCurrentTCBs[] */
    l32i    a2,  a2, 0                  /* a2 = current TCB                */
    beqz    a2,  1f
    s32i    a1,  a2, TCB_TOP_OF_STACK_OFF   /* pxCurrentTCB->pxTopOfStack = SP */
    movi    a1,  xt_interrupt_stack_top /* a1 = top of core 0 intr stack   */
    #if ( configNUMBER_OF_CORES > 1 )
    beqz    a3,  1f                     /* a3 contains coreid from pxctcb  */
    movi    a2,  configISR_STACK_SIZE
.Lset_int_stack:
    add     a1,  a1, a2
    addi    a3,  a3, -1
    bnez    a3,  .Lset_int_stack
    #endif

.Lnested:
1:
    mov     a0,  a12                    /* restore return addr and return  */
    ret

#endif /* XCHAL_HAVE_XEA2 */

#if XCHAL_HAVE_XEA2

/*
*******************************************************************************
*                                            _frxt_int_exit
*                                       void _frxt_int_exit(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_INT_EXIT function for
* FreeRTOS. If required, calls vPortYieldFromInt() to perform task context
* switching, restore the (possibly) new task's context, and return to the
* exit dispatcher saved in the task's stack frame at XT_STK_EXIT.
* May only be called from assembly code by the 'call0' instruction. Does not
* return to caller.
* See the description of the XT_RTOS_ENTER macro in xtensa_rtos.h.
*
*******************************************************************************
*/
    .globl  _frxt_int_exit
    .type   _frxt_int_exit,@function
    .align  4
_frxt_int_exit:

    pintdata a3, a2                     /* a3 <- &port_interruptNesting    */
    movi    a2,  port_xSchedulerRunning
    rsil    a0,  XT_IRQ_LOCK_LEVEL      /* lock out interrupts             */
    l32i    a2,  a2, 0                  /* a2 = port_xSchedulerRunning     */
    beqz    a2,  .Lnoswitch             /* scheduler not running, no tasks */
    l32i    a2,  a3, PORTINT_NEST_OFF   /* a2 = port_interruptNesting      */
    addi    a2,  a2, -1                 /* decrement nesting count         */
    s32i    a2,  a3, PORTINT_NEST_OFF   /* save nesting count              */
    bnez    a2,  .Lnesting              /* !=0 after decr so still nested  */

    pxctcb  a2,  a4                     /* pxCurrentTCB or pxCurrentTCBs[] */
    l32i    a2,  a2, 0                  /* a2 = current TCB                */
    beqz    a2,  1f                     /* no task ? go to dispatcher      */
    l32i    a1,  a2, TCB_TOP_OF_STACK_OFF         /* SP = pxCurrentTCB->pxTopOfStack */

    l32i    a2,  a3, PORTINT_SWITCH_OFF /* a2 = port_switch_flag           */
    beqz    a2,  .Lnoswitch             /* flag = 0 means no switch reqd   */
    movi    a2,  0
    s32i    a2,  a3, PORTINT_SWITCH_OFF /* zero out the flag for next time */

1:
    /*
    Call0 ABI callee-saved regs a12-15 need to be saved before possible preemption.
    However a12-13 were already saved by _frxt_int_enter().
    */
    #ifdef __XTENSA_CALL0_ABI__
    s32i    a14, a1, XT_STK_A14
    s32i    a15, a1, XT_STK_A15
    #endif

    #ifdef __XTENSA_CALL0_ABI__
    call0   vPortYieldFromInt       /* call dispatch inside the function; never returns */
    #else
    call4   vPortYieldFromInt       /* this one returns */
    call0   _frxt_dispatch          /* tail-call dispatcher */
    /* Never returns here. */
    #endif

.Lnoswitch:
    /*
    If we came here then about to resume the interrupted task.
    */

.Lnesting:
    /*
    We come here only if there was no context switch, that is if this
    is a nested interrupt, or the interrupted task was not preempted.
    In either case there's no need to load the SP.
    */

#if portUSING_MPU_WRAPPERS
    call0   _xt_mpu_restore
#endif

    /* Restore full context from interrupt stack frame */
    call0   _xt_context_restore

    /*
    Must return via the exit dispatcher corresponding to the entry point from which
    this was called. Interruptee's A0, A1, PS, PC are restored and the interrupt
    stack frame is deallocated in the exit dispatcher.
    */
    l32i    a0,  a1, XT_STK_EXIT
    ret

#endif /* XCHAL_HAVE_XEA2 */

/*
**********************************************************************************************************
*                                    DISPATCH THE HIGH READY TASK
*                                     void _frxt_dispatch(void)
*
* Switch context to the highest priority ready task, restore its state and dispatch control to it.
*
* This is a common dispatcher that acts as a shared exit path for all the context switch functions
* including vPortYield() and vPortYieldFromInt(), all of which tail-call this dispatcher
* (for windowed ABI vPortYieldFromInt() calls it indirectly via _frxt_int_exit() ).
*
* The Xtensa port uses different stack frames for solicited and unsolicited task suspension (see
* comments on stack frames in xtensa_context.h). This function restores the state accordingly.
* If restoring a task that solicited entry, restores the minimal state and leaves CPENABLE clear.
* If restoring a task that was preempted, restores all state including the task's CPENABLE.
*
* Entry:
*   pxCurrentTCB  points to the TCB of the task to suspend,
*   Because it is tail-called without a true function entrypoint, it needs no 'entry' instruction.
*
* Exit:
*   If incoming task called vPortYield() (solicited), this function returns as if from vPortYield().
*   If incoming task was preempted by an interrupt, this function jumps to exit dispatcher.
*
**********************************************************************************************************
*/
    .globl  _frxt_dispatch
    .type   _frxt_dispatch,@function
    .align  4
_frxt_dispatch:

#if XCHAL_HAVE_XEA3
    /* Skip "entry" - nothing to save, never returns. */

    movi    a2, PS_STACK_KERNEL | PS_DI     /* Set PS.STACK = Kernel and    */
    movi    a3, PS_STACK_MASK | PS_DI_MASK  /* disable interrupts.          */
    xps     a2, a3

    movi    a2, port_yield_flag
    movi    a3, 1
    s32i    a3, a2, 0                       /* Indicate thread yield.       */

#ifdef __XTENSA_CALL0_ABI__
    #if ( configNUMBER_OF_CORES > 1 )
    coreid  a2
    #endif
    call0   vTaskSwitchContext  // Get next TCB to resume
    pxctcb  a2,  a3                         /* pxCurrentTCB or pxCurrentTCBs[] */
#else
    // When (configNUMBER_OF_CORES > 1) we call vTaskSwitchContext(coreId)
    // ... and pxctcb places coreid into second register parameter (for SMP)
    pxctcb  a2,  a10                        /* pxCurrentTCB or pxCurrentTCBs[] */
    call8   vTaskSwitchContext  // Get next TCB to resume
#endif  // __XTENSA_CALL0_ABI__

    l32i    a3,  a2, 0                      /* a3 = pxCurrentTCB           */
    l32i    a1,  a3, TCB_TOP_OF_STACK_OFF   /* SP = next_TCB->pxTopOfStack */
    addi    a1,  a1, XT_STK_FRMSZ           /* Adjust for the dummy frame  */

#ifdef __XTENSA_CALL0_ABI__
    mov    a15,  a1                         /* Dispatch code expects a15 = old SP */
#endif

    movi    a0, _xt_dispatch + 3            /* Jump to dispatch code. It will */
    ret                                     /* check for ready thread or idle */
                                            /* and handle accordingly.        */

    ill                                     /* Should never get back here. */
#endif

#if XCHAL_HAVE_XEA2
    #ifdef __XTENSA_CALL0_ABI__
    #if ( configNUMBER_OF_CORES > 1 )
    coreid  a2
    #endif
    call0   vTaskSwitchContext  // Get next TCB to resume
    pxctcb  a2,  a3                         /* pxCurrentTCB or pxCurrentTCBs[] */
    #else
    // When (configNUMBER_OF_CORES > 1) we call vTaskSwitchContext(coreId)
    // ... and pxctcb places coreid into second register parameter (for SMP)
    pxctcb  a2,  a6                         /* pxCurrentTCB or pxCurrentTCBs[] */
    call4   vTaskSwitchContext  // Get next TCB to resume
    #endif
    l32i    a3,  a2, 0
    l32i    sp,  a3, TCB_TOP_OF_STACK_OFF     /* SP = next_TCB->pxTopOfStack */
    s32i    a3,  a2, 0

    /* Determine the type of stack frame. */
    l32i    a2,  sp, XT_STK_EXIT        /* exit dispatcher or solicited flag */
    bnez    a2,  .L_frxt_dispatch_stk

.L_frxt_dispatch_sol:

    /* Solicited stack frame. Restore minimal context and return from vPortYield(). */

#if portUSING_MPU_WRAPPERS
    movi    a2, SYSCALL_raise_priv
    syscall
    mov     a9, a2
    call0   _xt_mpu_restore
    bnez    a9, skip_lowering_priv;
    rsr.ps  a2
    addi    a2, a2, PS_RING(1)
    wsr.ps  a2
skip_lowering_priv:
#endif

    #if XCHAL_HAVE_LOOPS
    /* LCOUNT was not saved upon solicited yield; clear it to a known state */
    movi    a3,  0
    wsr     a3,  LCOUNT
    #endif

    l32i    a3,  sp, XT_STK_PS
    #ifdef __XTENSA_CALL0_ABI__
    l32i    a12, sp, XT_STK_A12
    l32i    a13, sp, XT_STK_A13
    l32i    a14, sp, XT_STK_A14
    l32i    a15, sp, XT_STK_A15
    #endif
    l32i    a0,  sp, XT_STK_PC
    #if XCHAL_CP_NUM > 0
    /* Ensure wsr.CPENABLE is complete (should be, it was cleared on entry). */
    rsync
    #endif
    /* As soons as PS is restored, interrupts can happen. No need to sync PS. */
    wsr     a3,  PS
    #ifdef __XTENSA_CALL0_ABI__
    addi    sp,  sp, XT_STK_FRMSZ
    ret
    #else
    retw
    #endif

.L_frxt_dispatch_stk:

    #if XCHAL_CP_NUM > 0
    /* Restore CPENABLE from task's co-processor save area. */
    pxctcb  a3, a2                          /* pxCurrentTCB or pxCurrentTCBs[] */
    l32i    a3, a3, 0
    l32i    a2, a3, TCB_END_OF_STACK_OFF          /* a2 = base of coproc save area */
    l16ui   a3, a2, XT_CPENABLE         /* a3 = saved CPENABLE value     */
    wsr     a3, CPENABLE
    #endif

#if portUSING_MPU_WRAPPERS
    call0   _xt_mpu_restore
#endif

    /* Interrupt stack frame. Restore full context and return to exit dispatcher. */
    call0   _xt_context_restore

    /* In Call0 ABI, restore callee-saved regs (A12, A13 already restored). */
    #ifdef __XTENSA_CALL0_ABI__
    l32i    a14, sp, XT_STK_A14
    l32i    a15, sp, XT_STK_A15
    #endif

    #if XCHAL_CP_NUM > 0
    /* Ensure wsr.CPENABLE has completed. */
    rsync
    #endif

    /*
    Must return via the exit dispatcher corresponding to the entrypoint from which
    this was called. Interruptee's A0, A1, PS, PC are restored and the interrupt
    stack frame is deallocated in the exit dispatcher.
    */
    l32i    a0, sp, XT_STK_EXIT
    ret
#endif /* XCHAL_HAVE_XEA2 */

/*
**********************************************************************************************************
*                            PERFORM A SOLICTED CONTEXT SWITCH (from a task)
*                                        void vPortYield(void)
*
* This function saves the minimal state needed for a solicited task suspension, clears CPENABLE,
* then tail-calls the dispatcher _frxt_dispatch() to perform the actual context switch
*
* At Entry:
*   pxCurrentTCB  points to the TCB of the task to suspend
*   Callable from C (obeys ABI conventions on entry).
*
* Does not return to caller.
*
**********************************************************************************************************
*/
    .globl  vPortYield
    .type   vPortYield,@function
    .align  4
vPortYield:

#if XCHAL_HAVE_XEA3
#ifdef __XTENSA_CALL0_ABI__
    addi    sp, sp, -16
    s32i    a0, sp, 0                       /* save return address          */
#else
    entry   sp, 48
    s32i    a0, sp, 0                       /* save return address          */
    ssai    0
    spillw                                  /* spill all registers          */
#endif

    /*
    Save register state into exception frame. This is safe to do with
    interrupts enabled, and we will keep the frame allocated when we
    call the dispatch function.
    */

    movi    a0, .Lret
    rsr.ps  a2
    addi    sp, sp, -XT_STK_XFRM_SZ
    s32i    a0, sp, XT_STK_PC               /* save return PC               */
    s32i    a2, sp, XT_STK_PS               /* save PS                      */
#ifdef __XTENSA_CALL0_ABI__
    s32i    a12, sp, XT_STK_A12             /* callee-saved registers       */
    s32i    a13, sp, XT_STK_A13
    s32i    a14, sp, XT_STK_A14
    s32i    a15, sp, XT_STK_A15
#endif

    movi    a2, PS_STACK_KERNEL | PS_DI     /* Set PS.STACK = Kernel and    */
    movi    a8, PS_STACK_MASK | PS_DI_MASK  /* disable interrupts.          */
    xps     a2, a8

    pxctcb  a3, a4                          /* pxCurrentTCB or pxCurrentTCBs[] */
    l32i    a4, a3, 0                       /* a4 = pxCurrentTCB            */

#if XCHAL_CP_NUM > 0
    /* Save coprocessor callee-saved state (if any). At this point CPENABLE */
    /* should still reflect which CPs were in use (enabled).                */
    call0   _xt_coproc_savecs

    /* Clear CPENABLE and give up all co-procs. */
    l32i    a3, a4, TCB_END_OF_STACK_OFF    /* a3 = pxCurrentTCB->cp_state  */
    movi    a5, 0
    s16i    a5, a3, XT_CPENABLE
    wsr     a5, CPENABLE                    /* disable all co-processors    */
#endif

    addi    a5, sp, XT_STK_XFRM_SZ - XT_STK_FRMSZ
    s32i    a5, a4, TCB_TOP_OF_STACK_OFF    /* pxCurrentTCB->pxTopOfStack = SP  */

    call0   _frxt_dispatch                  /* Does not return, see below   */

    /* Execution returns here. Interrupts should be disabled. */
    /* NOTE: we expect original SP to have been restored. */

    .align  4

.Lret:
    addi    sp, sp, -XT_STK_XFRM_SZ         /* Prepare to restore state        */
    l32i    a2, sp, XT_STK_PS               /* Retrieve PS value               */
#ifdef __XTENSA_CALL0_ABI__
    l32i    a12, sp, XT_STK_A12             /* Callee-saved registers          */
    l32i    a13, sp, XT_STK_A13
    l32i    a14, sp, XT_STK_A14
    l32i    a15, sp, XT_STK_A15
#endif
    addi    sp, sp, XT_STK_XFRM_SZ
    wsr.ps  a2                              /* Safe to enable interrupts       */
    rsync

#ifdef __XTENSA_CALL0_ABI__
    l32i    a0, sp, 0
    addi    sp, sp, 16
    ret
#else
    l32i    a0, sp, 0
    retw
#endif
#endif /* XCHAL_HAVE_XEA3 */

#if XCHAL_HAVE_XEA2
    #ifdef __XTENSA_CALL0_ABI__
    addi    a2, sp, -XT_STK_FRMSZ           /* avoid addi/addmi relaxation that */
    mov     sp, a2                          /* might temporarily move sp up     */
    #else
    entry   sp, XT_STK_FRMSZ
    #endif

    rsr     a2,  PS
    s32i    a0,  sp, XT_STK_PC
    s32i    a2,  sp, XT_STK_PS
    #ifdef __XTENSA_CALL0_ABI__
    s32i    a12, sp, XT_STK_A12         /* save callee-saved registers      */
    s32i    a13, sp, XT_STK_A13
    s32i    a14, sp, XT_STK_A14
    s32i    a15, sp, XT_STK_A15
    #else
    /* Spill register windows. Calling xthal_window_spill() causes extra    */
    /* spills and reloads, so we will set things up to call the _nw version */
    /* instead to save cycles.                                              */
    movi    a6,  ~(PS_WOE_MASK|PS_INTLEVEL_MASK)  /* spills a4-a7 if needed */
    and     a2,  a2, a6                           /* clear WOE, INTLEVEL    */
    addi    a2,  a2, XT_IRQ_LOCK_LEVEL            /* set INTLEVEL           */
    wsr     a2,  PS
    rsync
    call0   xthal_window_spill_nw
    l32i    a2,  sp, XT_STK_PS                    /* restore PS             */
    wsr     a2,  PS
    #endif

    rsil    a2,  XT_IRQ_LOCK_LEVEL      /* disable low/med interrupts       */

    #if XCHAL_CP_NUM > 0
    /* Save coprocessor callee-saved state (if any). At this point CPENABLE */
    /* should still reflect which CPs were in use (enabled).                */
    call0   _xt_coproc_savecs
    #endif

    pxctcb  a2,  a3                         /* pxCurrentTCB or pxCurrentTCBs[] */
    movi    a3,  0
    l32i    a2,  a2, 0                  /* a2 = pxCurrentTCB                */
    s32i    a3,  sp, XT_STK_EXIT        /* 0 to flag as solicited frame     */
    s32i    sp,  a2, TCB_TOP_OF_STACK_OFF         /* pxCurrentTCB->pxTopOfStack = SP  */

    #if XCHAL_CP_NUM > 0
    /* Clear CPENABLE, also in task's co-processor state save area. */
    l32i    a2,  a2, TCB_END_OF_STACK_OFF         /* a2 = pxCurrentTCB->cp_state      */
    movi    a3,  0
    wsr     a3,  CPENABLE
    beqz    a2,  1f
    s16i    a3,  a2, XT_CPENABLE        /* clear saved cpenable             */
1:
    #endif /* XCHAL_CP_NUM > 0 */

    /* Tail-call dispatcher. */
    call0   _frxt_dispatch
    /* Never reaches here. */
#endif /* XCHAL_HAVE_XEA2 */

/*
**********************************************************************************************************
*                         PERFORM AN UNSOLICITED CONTEXT SWITCH (from an interrupt)
*                                        void vPortYieldFromInt(void)
*
* This calls the context switch hook (removed), saves and clears CPENABLE, then tail-calls the dispatcher
* _frxt_dispatch() to perform the actual context switch.
*
* At Entry:
*   Interrupted task context has been saved in an interrupt stack frame at pxCurrentTCB->pxTopOfStack.
*   pxCurrentTCB  points to the TCB of the task to suspend,
*   Callable from C (obeys ABI conventions on entry).
*
* At Exit:
*   Windowed ABI defers the actual context switch until the stack is unwound to interrupt entry.
*   Call0 ABI tail-calls the dispatcher directly (no need to unwind) so does not return to caller.
*
**********************************************************************************************************
*/
    .globl  vPortYieldFromInt
    .type   vPortYieldFromInt,@function
    .align  4
vPortYieldFromInt:

    ENTRY(16)

#if XCHAL_CP_NUM > 0
    /* Save CPENABLE in task's co-processor save area, and clear CPENABLE.  */
    pxctcb  a3, a6                      /* pxCurrentTCB or pxCurrentTCBs[] */
    l32i    a3, a3, 0
    l32i    a2, a3, TCB_END_OF_STACK_OFF

    rsr     a4, CPENABLE

  #if ( configNUMBER_OF_CORES > 1 )
    beqz    a4, .Lnocp_save

    #if ( configUSE_CORE_AFFINITY == 1 )
    /* Task can run on other cores if affinity mask has bit(s) set for other cores */
    movi    a5, port_affinity_masks
    addx4   a5, a6, a5
    l32i    a5, a5, 0
    l32i    a3, a3, TCB_CORE_AFFINITY_MASK_OFF
    bnone   a3, a5, .Lnocp_save         /* skip save if task only runnable on current core */
.Lcp_save:
    #endif /* ( configUSE_CORE_AFFINITY == 1 ) */

    #ifdef __XTENSA_CALL0_ABI__
    /* No need to save a0 since we will call _frxt_dispatch later, which doesn't return */
    call0   _xt_coproc_saveall
    #else
    mov     a6, a2
    call4   _xt_coproc_saveall
    #endif  /* __XTENSA_CALL0_ABI__ */
    j       .Lcp_done
.Lnocp_save:
  #endif /* ( configNUMBER_OF_CORES > 1 ) */

    s16i    a4, a2, XT_CPENABLE         /* cp_state->cpenable = CPENABLE;   */

    movi    a4, 0
    wsr     a4, CPENABLE                /* disable all co-processors        */
.Lcp_done:
#endif /* XCHAL_CP_NUM > 0 */

#ifdef __XTENSA_CALL0_ABI__
    /* Tail-call dispatcher. */
    call0   _frxt_dispatch
    /* Never reaches here. */
#else
    RET(16)
#endif

/*
--------------------------------------------------------------------------------
  Task entry wrapper. Set up as the resume address in the initial stack frame.
--------------------------------------------------------------------------------
*/

    .global     _xt_task_start
    .type       _xt_task_start,@function
    .align      4

_xt_task_start:
#if XCHAL_CP_NUM > 0
    movi    a0, 0
    wsr     a0, CPENABLE                    /* disable all coprocessors */
#endif
#if XCHAL_HAVE_LOOPS
    #if XCHAL_CP_NUM == 0
    movi    a0,  0                          /* skip if (a0 <- 0) by above loop logic */
    #endif
    wsr     a0,  LCOUNT                     /* ensure loops are disabled */
#endif

#if XCHAL_HAVE_XEA3
    /* On entry, a8 = task entry point, a9 = top of stack / coproc save area */
    pxctcb  a0, a3                          /* pxCurrentTCB or pxCurrentTCBs[] */
    l32i    a0, a0, 0
    s32i    a9, a0, TCB_END_OF_STACK_OFF    /* TCB->pxEndOfStack = CP save area */
    movi    a0, 0                           /* terminate call stack for gdb     */
    jx      a8
#endif

#if XCHAL_HAVE_XEA2
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS        */
    wsr     a0, PS
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC        */
    wsr     a0, EPC_1
    pxctcb  a3, a0                          /* pxCurrentTCB or pxCurrentTCBs[] */
    l32i    a3, a3, 0
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0        */
    l32i    sp, sp, XT_STK_A1               /* remove interrupt stack frame     */
    s32i    sp, a3, TCB_END_OF_STACK_OFF    /* TCB->pxEndOfStack = CP save area */
    rsync                                   /* ensure PS and EPC written        */
    rfe                                     /* PS.EXCM is cleared               */
#endif

#if XCHAL_HAVE_XEA3
/*
--------------------------------------------------------------------------------
  XEA3 User-mode task entry wrapper.  Same as _xt_task_start but sets PS_RING
  bit after privileged init is complete.
--------------------------------------------------------------------------------
*/

    .global     _xt_task_start_user
    .type       _xt_task_start_user,@function
    .align      4

_xt_task_start_user:
#if XCHAL_CP_NUM > 0
    movi    a0, 0
    wsr     a0, CPENABLE                    /* disable all coprocessors */
#endif

    /* On entry, a8 = task entry point, a9 = top of stack / coproc save area */
    pxctcb  a0, a3                          /* pxCurrentTCB or pxCurrentTCBs[] */
    l32i    a0, a0, 0
    s32i    a9, a0, TCB_END_OF_STACK_OFF    /* TCB->pxEndOfStack = CP save area */
    rsr     a0, PS
    movi    a3, (1 << PS_RING_SHIFT)
    or      a0, a0, a3
    wsr     a0, PS
    movi    a0, 0                           /* terminate call stack for gdb     */
    jx      a8
#endif

/*
**********************************************************************************************************
*                                        _xt_task_coproc_state
*                                   void _xt_task_coproc_state(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_CP_STATE function for FreeRTOS.
*
* May only be called when a task is running, not within an interrupt handler (returns 0 in that case).
* May only be called from assembly code by the 'call0' instruction. Does NOT obey ABI conventions.
* Returns in a15 a pointer to the base of the co-processor state save area for the current task.
* May trash a8, which is already trashed by all callers in xtensa_coproc_handler.S.
* See the detailed description of the XT_RTOS_ENTER macro in xtensa_rtos.h.
*
**********************************************************************************************************
*/
#if XCHAL_CP_NUM > 0

    .globl  _xt_task_coproc_state
    .type   _xt_task_coproc_state,@function
    .align  4

_xt_task_coproc_state:

    movi    a15, port_xSchedulerRunning /* if (port_xSchedulerRunning                   */
    l32i    a15, a15, 0
    beqz    a15, 2f
    pintdata a15, a8                    /* a15 <- &port_interruptNesting                */
    l32i    a15, a15, PORTINT_NEST_OFF
    bnez    a15, 1f
    pxctcb  a15, a8                         /* pxCurrentTCB or pxCurrentTCBs[] */
    l32i    a15, a15, 0                 /* && pxCurrentTCB != 0) {                      */
    beqz    a15, 2f
    l32i    a15, a15, TCB_END_OF_STACK_OFF        /* a15 = pxCurrentTCB->pxEndOfStack             */
    ret
1:
    movi    a15, 0
2:
    ret

#endif /* XCHAL_CP_NUM > 0 */


#if (defined configNUMBER_OF_CORES) && ( configNUMBER_OF_CORES > 1 )

/*
-------------------------------------------------------------------------------
  Initial (startup) stack for SMP systems

  For SMP configurations, (configNUMBER_OF_CORES * XT_SYSTEM_STACK_SIZE) bytes
  are allocated.  Avoid placing this in per-core memory (usually dataram) as 
  that is a valuable resource.
-------------------------------------------------------------------------------
*/

    .data

    .global xt_startup_stacks
    .type   xt_startup_stacks,@object
    .align  16

xt_startup_stacks:
    .space  XT_SYSTEM_STACK_SIZE        // Working space, argv[0], overflow
    .space  XT_USER_SIZE                // Extra for core 0: main()/printf()/...

    .global xt_startup_stack_top
    .type   xt_startup_stack_top,@object
xt_startup_stack_top:
    .rept   configNUMBER_OF_CORES - 1
    .space  XT_SYSTEM_STACK_SIZE
    .endr


    .text
    .global __stack_init
    .type   __stack_init,@function
    .align  4

__stack_init:

    // NOTE: called via call0/callx0, return address is in a0.
    // NOTE: must not rely on valid stack pointer / BSS values.

    movi    a2, XT_SYSTEM_STACK_SIZE    // must be multiple of 16 bytes
    coreid  a3

    movi    a4, xt_startup_stack_top    // default SP (top of core 0 stack)
2:
    beqz    a3, 3f
    add     a4, a4, a2
    addi    a3, a3, -1
    j       2b
3:
    mov     a1, a4                      // set stack pointer
    ret

    .size   __stack_init, . - __stack_init


    .text
    .global __bss_init
    .type   __bss_init,@function
    .align  4

__bss_init:
    // void __bss_init(uint32_t * table_start, uint32_t * table_end)
    //
    // table_start -- points to first entry in BSS table (2 words)
    // table_end   -- points to end of BSS table
    //
    // This function clears BSS sections in dataram for all cores.
    // BSS sections not in dataram are cleared only by core 0.

    abi_entry  16, 8
    coreid  a4

.L1:
    bgeu    a2, a3, .Lret           // at table end, finish
    l32i    a5, a2, 0               // entry start address
    l32i    a6, a2, 4               // entry end address
    addi    a2, a2, 8               // advance to next entry
    beq     a5, a6, .L1             // empty section, skip

#if defined (XCHAL_DATARAM0_VADDR)
    movi    a7, XCHAL_DATARAM0_VADDR
    movi    a8, XCHAL_DATARAM0_SIZE
    sub     a7, a5, a7              // a7 = start_addr - dataram0_start_addr
    bgeu    a7, a8, .L2             // not within dataram0 range
    j       .Lclear
#endif
.L2:
#if defined (XCHAL_DATARAM1_VADDR)
    movi    a7, XCHAL_DATARAM1_VADDR
    movi    a8, XCHAL_DATARAM1_SIZE
    sub     a7, a5, a7              // a7 = start_addr - dataram1_start_addr
    bgeu    a7, a8, .L3             // not within dataram1 range
    j       .Lclear
#endif
.L3:
    bnez    a4, .L1                 // not core 0, skip
.Lclear:
    movi    a7, 0                   // value to store
    sub     a10, a6, a5             // a10 = length, assumed a multiple of 4
    bbci.l  a10, 2, .L1z
    s32i    a7, a5, 0               // clear 4 bytes to make length multiple of 8
    addi    a5, a5, 4
.L1z:
    bbci.l  a10, 3, .L2z
    s32i    a7, a5, 0               // clear 8 bytes to make length multiple of 16
    s32i    a7, a5, 4
    addi    a5, a5, 8
.L2z:
    srli    a10, a10, 4             // length is now multiple of 16, divide by 16
    floopnez  a10, clearzte
    s32i    a7, a5,  0              // clear 16 bytes at a time...
    s32i    a7, a5,  4
    s32i    a7, a5,  8
    s32i    a7, a5, 12
    addi    a5, a5, 16
    floopend  a10, clearzte
    bltu    a2, a3, .L1

.Lret:
    abi_return

    .size   __bss_init, . - __bss_init

#endif /* configNUMBER_OF_CORES > 1 */
