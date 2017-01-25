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

/* High speed timer test as described in main.c. */


/* Scheduler includes. */
#include "FreeRTOS.h"

/* The maximum value the 16bit timer can contain. */
#define timerMAX_COUNT				0xffff

/* The timer 2 interrupt handler.  As this interrupt uses the FreeRTOS assembly
entry point the IPL setting in the following function prototype has no effect.
The interrupt priority is set by ConfigIntTimer2() in vSetupTimerTest(). */
void __attribute__( (interrupt(IPL0AUTO), vector(_TIMER_2_VECTOR))) vT2InterruptWrapper( void );

/*-----------------------------------------------------------*/

/* Incremented every 20,000 interrupts, so should count in seconds. */
unsigned long ulHighFrequencyTimerInterrupts = 0;

/* The frequency at which the timer is interrupting. */
static unsigned long ulFrequencyHz;

/*-----------------------------------------------------------*/

void vSetupTimerTest( unsigned short usFrequencyHz )
{
	/* Remember the frequency so it can be used from the ISR. */
	ulFrequencyHz = ( unsigned long ) usFrequencyHz;

	/* T2 is used to generate interrupts above the kernel and max syscall 
	interrupt priority. */
	T2CON = 0;
	TMR2 = 0;

	/* Timer 2 is going to interrupt at usFrequencyHz Hz. */
	PR2 = ( unsigned short ) ( ( configPERIPHERAL_CLOCK_HZ / ( unsigned long ) usFrequencyHz ) - 1 );

	/* Setup timer 2 interrupt priority to be above the kernel priority so
	the timer jitter is not effected by the kernel activity. */
	IPC2bits.T2IP = ( configMAX_SYSCALL_INTERRUPT_PRIORITY + 1 );

	/* Clear the interrupt as a starting condition. */
	IFS0bits.T2IF = 0;

	/* Enable the interrupt. */
	IEC0bits.T2IE = 1;

	/* Start the timer. */
	T2CONbits.TON = 1;
}
/*-----------------------------------------------------------*/

void vT2InterruptHandler( void )
{
static unsigned long ulMaxNestingDepth = 0;

	/* Keep a count of interrupts so the check timer can ensure they are
	occurring at the expected rate. */
	ulHighFrequencyTimerInterrupts++;

	/* Establish the maximum nesting count reached to ensure the test is doing
	what it is supposed to. */
	if( uxInterruptNesting > ulMaxNestingDepth )
	{
		ulMaxNestingDepth = uxInterruptNesting;
	}

	/* Clear the timer interrupt. */
	IFS0CLR = _IFS0_T2IF_MASK;
}


