/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

	PUBLIC _prvRegTest1Implementation
	PUBLIC _prvRegTest2Implementation
	
	EXTERN _ulRegTest1CycleCount
	EXTERN _ulRegTest2CycleCount

	RSEG CODE:CODE(4)

/* This function is explained in the comments at the top of main.c. */
_prvRegTest1Implementation:

	/* Put a known value in each register. */
	MOV	#1, R1						
	MOV	#2, R2						
	MOV	#3, R3						
	MOV	#4, R4						
	MOV	#5, R5						
	MOV	#6, R6						
	MOV	#7, R7						
	MOV	#8, R8						
	MOV	#9, R9						
	MOV	#10, R10					
	MOV	#11, R11					
	MOV	#12, R12					
	MOV	#13, R13					
	MOV	#14, R14					
	MOV	#15, R15					
	
	/* Loop, checking each iteration that each register still contains the
	expected value. */
TestLoop1:								

	/* Push the registers that are going to get clobbered. */
	PUSHM	R14-R15						
	
	/* Increment the loop counter to show this task is still getting CPU time. */
	MOV	#_ulRegTest1CycleCount, R14	
	MOV	[ R14 ], R15				
	ADD	#1, R15						
	MOV	R15, [ R14 ]				
	
	/* Yield to extend the text coverage.  Set the bit in the ITU SWINTR register. */
	MOV	#1, R14						
	MOV 	#0872E0H, R15				
	MOV.B	R14, [R15]					
	NOP								
	NOP								
	
	/* Restore the clobbered registers. */
	POPM	R14-R15						
	
	/* Now compare each register to ensure it still contains the value that was
	set before this loop was entered. */
	CMP	#1, R1						
	BNE	RegTest1Error				
	CMP	#2, R2						
	BNE	RegTest1Error				
	CMP	#3, R3						
	BNE	RegTest1Error				
	CMP	#4, R4						
	BNE	RegTest1Error				
	CMP	#5, R5						
	BNE	RegTest1Error				
	CMP	#6, R6						
	BNE	RegTest1Error				
	CMP	#7, R7						
	BNE	RegTest1Error				
	CMP	#8, R8						
	BNE	RegTest1Error				
	CMP	#9, R9						
	BNE	RegTest1Error				
	CMP	#10, R10					
	BNE	RegTest1Error				
	CMP	#11, R11					
	BNE	RegTest1Error				
	CMP	#12, R12					
	BNE	RegTest1Error				
	CMP	#13, R13					
	BNE	RegTest1Error				
	CMP	#14, R14					
	BNE	RegTest1Error				
	CMP	#15, R15					
	BNE	RegTest1Error				

	/* All comparisons passed, start a new itteratio of this loop. */
	BRA		TestLoop1				
	
RegTest1Error:							
	/* A compare failed, just loop here so the loop counter stops incrementing
	- causing the check task to indicate the error. */
	BRA RegTest1Error					
/*-----------------------------------------------------------*/

/* This function is explained in the comments at the top of main.c. */
_prvRegTest2Implementation:

	/* Put a known value in each register. */
	MOV	#10H, R1					
	MOV	#20H, R2					
	MOV	#30H, R3					
	MOV	#40H, R4					
	MOV	#50H, R5					
	MOV	#60H, R6					
	MOV	#70H, R7					
	MOV	#80H, R8					
	MOV	#90H, R9					
	MOV	#100H, R10					
	MOV	#110H, R11					
	MOV	#120H, R12					
	MOV	#130H, R13					
	MOV	#140H, R14					
	MOV	#150H, R15					
	
	/* Loop, checking each iteration that each register still contains the
	expected value. */
TestLoop2:								

	/* Push the registers that are going to get clobbered. */
	PUSHM	R14-R15						
	
	/* Increment the loop counter to show this task is still getting CPU time. */
	MOV	#_ulRegTest2CycleCount, R14	
	MOV	[ R14 ], R15				
	ADD	#1, R15						
	MOV	R15, [ R14 ]				
	
	/* Restore the clobbered registers. */
	POPM	R14-R15						
	
	/* Now compare each register to ensure it still contains the value that was
	set before this loop was entered. */
	CMP	#10H, R1					
	BNE	RegTest2Error				
	CMP	#20H, R2					
	BNE	RegTest2Error				
	CMP	#30H, R3					
	BNE	RegTest2Error				
	CMP	#40H, R4					
	BNE	RegTest2Error				
	CMP	#50H, R5					
	BNE	RegTest2Error				
	CMP	#60H, R6					
	BNE	RegTest2Error				
	CMP	#70H, R7					
	BNE	RegTest2Error				
	CMP	#80H, R8					
	BNE	RegTest2Error				
	CMP	#90H, R9					
	BNE	RegTest2Error				
	CMP	#100H, R10					
	BNE	RegTest2Error				
	CMP	#110H, R11					
	BNE	RegTest2Error				
	CMP	#120H, R12					
	BNE	RegTest2Error				
	CMP	#130H, R13					
	BNE	RegTest2Error				
	CMP	#140H, R14					
	BNE	RegTest2Error				
	CMP	#150H, R15					
	BNE	RegTest2Error				

	/* All comparisons passed, start a new itteratio of this loop. */
	BRA	TestLoop2					
	
RegTest2Error:							
	/* A compare failed, just loop here so the loop counter stops incrementing
	- causing the check task to indicate the error. */
	BRA RegTest2Error					

	
	END
