;/*
;* FreeRTOS Kernel V11.0.1
;* Copyright (C) 2023 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
;*
;* SPDX-License-Identifier: MIT
;*
;* Permission is hereby granted, free of charge, to any person obtaining a copy of
;* this software and associated documentation files (the "Software"), to deal in
;* the Software without restriction, including without limitation the rights to
;* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
;* the Software, and to permit persons to whom the Software is furnished to do so,
;* subject to the following conditions:
;*
;* The above copyright notice and this permission notice shall be included in all
;* copies or substantial portions of the Software.
;*
;* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
;* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
;* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
;* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
;* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
;*
;* https://www.FreeRTOS.org
;* https://github.com/FreeRTOS
;*
;*/

;------------------------------------------------------------------------------
; Extern symbols
;------------------------------------------------------------------------------
.extern _vTaskSwitchContext
.extern _pxCurrentTCB
.extern _xInterruptNesting
.extern _xPortSwitchRequired
.extern _bsp_common_interrupt_handler

.public _vPortYield
.public _TRAP0_Handler
.public _Irq_Handler
.public _vPortStartFirstTask

.section ".text", text
;------------------------------------------------------------------------------
; Macro definitions
;------------------------------------------------------------------------------
EIPC  .set 0
EIPSW .set 1
EIIC  .set 13
EIIC_MSK .set 0x00000FFF

;------------------------------------------------------------------------------
; Context saving
;------------------------------------------------------------------------------
portSAVE_CONTEXT .macro
    prepare lp, 0

    ;Save general-purpose registers and EIPSW, EIPC, EIIC into stack.
    pushsp  r5, r30
    $nowarning
    pushsp  r1, r2
    $warning

    stsr    EIPSW, r17
    stsr    EIPC, r18
    pushsp  r17, r18

    ;Save the stack pointer in the TCB
    mov     #_pxCurrentTCB, r2
    ld.w    0[r2], r2
    st.w    sp, 0[r2]
.endm

;------------------------------------------------------------------------------
; Context restoring
;------------------------------------------------------------------------------
portRESTORE_CONTEXT .macro
    ;Set the SP to point to the stack of the task being restored.
    mov     #_pxCurrentTCB, r2
    ld.w    0[r2], r2
    ld.w    0[r2], sp

    ;Restore general-purpose registers and EIPSW, EIPC, EIIC.
    popsp   r17, r18
    ldsr    r18, EIPC
    ldsr    r17, EIPSW

    $nowarning
    popsp   r1, r2
    $warning
    popsp   r5, r30

    dispose 0, lp
.endm

;------------------------------------------------------------------------------
; Start the first task.
;------------------------------------------------------------------------------
_vPortStartFirstTask:

    portRESTORE_CONTEXT

    eiret

;------------------------------------------------------------------------------
; _vPortYield
;------------------------------------------------------------------------------
_vPortYield:
    trap    0
    jmp     [lp]

;------------------------------------------------------------------------------
; _TRAP0_Handler
; FreeRTOS yield handler. This is installed as the TRAP exception handler.
;------------------------------------------------------------------------------
_TRAP0_Handler:
    ;Save the context of the current task.
    portSAVE_CONTEXT

    ;Call the scheduler to select the next task.
    jarl    _vTaskSwitchContext, lp

    ;Restore the context of the next task to run.
    portRESTORE_CONTEXT

    eiret

;------------------------------------------------------------------------------
; Interrupt start processing.
;------------------------------------------------------------------------------

_Irq_Handler:
    ;Save used registers.
    pushsp  r5, r19
    $nowarning
    pushsp  r1, r2
    $warning

    stsr    EIPSW, r19
    stsr    EIPC, r18
    stsr    EIIC, r17
    mov     lp, r16
    mov     ep, r15

    pushsp  r15, r19

    ;Increment nesting count.
    mov     #_xInterruptNesting, r19
    ld.w    0[r19], r18
    addi    0x1, r18, r17
    st.w    r17, 0[r19]

    pushsp  r18, r19

    ;Call the interrupt handler.
    stsr    EIIC, r6
    andi    EIIC_MSK, r6, r6
    jarl _bsp_common_interrupt_handler, lp
    synce
    popsp   r18, r19

    ;Restore the old nesting count.
    st.w    r18, 0[r19]

    ;A context switch if no nesting interrupt.
    cmp     0x0, r18
    bne     exit_without_switch

    ;Check if context switch is requested.
    mov     #_xPortSwitchRequired, r19
    ld.w    0[r19], r18
    cmp     0x0, r18
    bne     switch_before_exit

exit_without_switch:
    ;No context switch.  Restore used registers
    popsp   r15, r19

    mov     r15, ep
    mov     r16, lp
    ldsr    r17, EIIC
    ldsr    r18, EIPC
    ldsr    r19, EIPSW

    $nowarning
    popsp   r1, r2
    $warning
    popsp   r5, r19

    eiret

switch_before_exit:
    ;Clear the context switch pending flag.
    st.w r0, 0[r19]

    ;Restore used registers before saving the context to the task stack.
    popsp   r15, r19

    mov     r15, ep
    mov     r16, lp
    ldsr    r17, EIIC
    ldsr    r18, EIPC
    ldsr    r19, EIPSW

    $nowarning
    popsp   r1, r2
    $warning
    popsp   r5, r19

    portSAVE_CONTEXT
    jarl    _vTaskSwitchContext, lp
    portRESTORE_CONTEXT

    eiret

