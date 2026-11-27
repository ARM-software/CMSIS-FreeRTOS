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

//	Copyright: Copyright (C) Texas Instruments Incorporated
//	All rights reserved not granted herein.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//  Redistributions of source code must retain the above copyright 
//  notice, this list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the 
//  documentation and/or other materials provided with the   
//  distribution.
//
//  Neither the name of Texas Instruments Incorporated nor the names of
//  its contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
    #include "portdefines.h"
    .extern pxCurrentTCB
    .extern xTaskIncrementTick
    .extern vTaskSwitchContext
    .extern ulTaskHasFPUContext

    .global portTICK_ISR
    .global portRESTORE_FIRST_CONTEXT
    .global vPortYield


    .macro portSAVE_CONTEXT
        ST.32 *(A15++#8), A14
        || MV A14, RPC
        ST.32 *(A15-#4), A14
        ST.32 *(A15++#8), DSTS
        ST.32 *(A15-#4), ESTS
        ST.64 *(A15++#8), XA0
        ST.64 *(A15++#8), XA2
        ST.64 *(A15++#8), XA4
        ST.64 *(A15++#8), XA6
        ST.64 *(A15++#8), XA8
        ST.64 *(A15++#8), XA10
        ST.64 *(A15++#8), XA12
        ST.64 *(A15++#8), XD0
        ST.64 *(A15++#8), XD2
        ST.64 *(A15++#8), XD4
        ST.64 *(A15++#8), XD6
        
        ;Check if task has FPU context
        || LD.32	 D0, @ulTaskHasFPUContext

        ST.64 *(A15++#8), XD8
        || BCMPZD  .+0x54, D.EQ, D0
        ST.64 *(A15++#8), XD10
        ST.64 *(A15++#8), XD12
        ST.64 *(A15++#8), XD14

        ST.64 *(A15++#8), XM0
        ST.64 *(A15++#8), XM2
        ST.64 *(A15++#8), XM4
        ST.64 *(A15++#8), XM6
        ST.64 *(A15++#8), XM8
        ST.64 *(A15++#8), XM10
        ST.64 *(A15++#8), XM12
        ST.64 *(A15++#8), XM14
        ST.64 *(A15++#8), XM16
        ST.64 *(A15++#8), XM18
        ST.64 *(A15++#8), XM20
        ST.64 *(A15++#8), XM22
        ST.64 *(A15++#8), XM24
        ST.64 *(A15++#8), XM26
        ST.64 *(A15++#8), XM28
        ST.64 *(A15++#8), XM30

        ; Store FPU context indicator
        ST.32 *(A15++#8), D0 
        ; Store the new top of stack for the task.
        || LD.32	A14, @pxCurrentTCB
        ST.32	*A14, A15
    .endm

    .macro portRESTORE_CONTEXT
        LD.32 A0, @pxCurrentTCB
        LD.32 A15, *A0

        ;Check if task has FPU context
        LD.32	 D0, *(A15-=#8)
        ST.32 @ulTaskHasFPUContext, D0
        || BCMPZ  .+0x4a, D.EQ, D0

        LD.64 XM30, *(A15-=#8)
        LD.64 XM28, *(A15-=#8)
        LD.64 XM26, *(A15-=#8)
        LD.64 XM24, *(A15-=#8)
        LD.64 XM22, *(A15-=#8)
        LD.64 XM20, *(A15-=#8)
        LD.64 XM18, *(A15-=#8)
        LD.64 XM16, *(A15-=#8)
        LD.64 XM14, *(A15-=#8)
        LD.64 XM12, *(A15-=#8)
        LD.64 XM10, *(A15-=#8)
        LD.64 XM8,  *(A15-=#8)
        LD.64 XM6,  *(A15-=#8)
        LD.64 XM4,  *(A15-=#8)
        LD.64 XM2,  *(A15-=#8)
        LD.64 XM0,  *(A15-=#8)
        
        LD.64 XD14, *(A15-=#8)
        LD.64 XD12, *(A15-=#8)
        LD.64 XD10, *(A15-=#8)
        LD.64 XD8,  *(A15-=#8)
        LD.64 XD6,  *(A15-=#8)
        LD.64 XD4,  *(A15-=#8)
        LD.64 XD2,  *(A15-=#8)
        LD.64 XD0,  *(A15-=#8)
        LD.64 XA12, *(A15-=#8)
        LD.64 XA10, *(A15-=#8)
        LD.64 XA8,  *(A15-=#8)
        LD.64 XA6,  *(A15-=#8)
        LD.64 XA4,  *(A15-=#8)
        LD.64 XA2,  *(A15-=#8)
        LD.64 XA0,  *(A15-=#8)
        LD.32 ESTS, *(A15-#4)
        LD.32 DSTS, *(A15-=#8)
        LD.32 RPC,  *(A15-#4)
        LD.32 A14,  *(A15-=#8)

    .endm

    .macro portTICK_TIMER_CLEAR_OVERFLOW
        ; Clear CPUTIMER overflow bit
        MV  A4, #PORT_TICK_TIMER_O_TCR
        LD.S16  D0, *(A4)
        OR      D0, #0x00008000
        ST.W0 *(A4), D0 

    .endm

portTICK_ISR:
	; Save the context of the current task.
    portSAVE_CONTEXT

    ; Clear tick timer and increment tick counter
    portTICK_TIMER_CLEAR_OVERFLOW
    CALL     xTaskIncrementTick
    BCMPZ    skipTaskSwitchContext, D.EQ, D0
    CALL     @vTaskSwitchContext

skipTaskSwitchContext:
    ; Restore the context of the task selected to execute.
    portRESTORE_CONTEXT

    RETI.INT


vPortYield:
	; Save the context of the current task.
    portSAVE_CONTEXT

    ; Select the next task to execute.
    CALL      @vTaskSwitchContext

    ; Restore the context of the task selected to execute.
    portRESTORE_CONTEXT
    RETI.INT


portRESTORE_FIRST_CONTEXT:
    portRESTORE_CONTEXT
    RET