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

/*
 *******************************************************************************
 * NOTE 1: The Win32 port is a simulation (or is that emulation?) only!  Do not
 * expect to get real time behaviour from the Win32 port or this demo
 * application.  It is provided as a convenient development and demonstration
 * test bed only.  This was tested using Windows XP on a dual core laptop.
 *
 * Windows will not be running the FreeRTOS simulator threads continuously, so
 * the timing information in the FreeRTOS+Trace logs have no meaningful units.
 * See the documentation page for the Windows simulator for an explanation of
 * the slow timing:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 * - READ THE WEB DOCUMENTATION FOR THIS PORT FOR MORE INFORMATION ON USING IT -
 *
 * NOTE 2:  This project provides two demo applications.  A simple blinky style
 * project, and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the comprehensive test and demo version.
 *
 * NOTE 3:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, are defined in
 * main.c.
 *******************************************************************************
 *
 * main() creates all the demo application tasks, then starts the scheduler.
 * The web documentation provides more details of the standard demo application
 * tasks, which provide no particular functionality but do provide a good
 * example of how to use the FreeRTOS API.
 *
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Check" task - This only executes every five seconds but has a high priority
 * to ensure it gets processor time.  Its main function is to check that all the
 * standard demo tasks are still operational.  While no errors have been
 * discovered the check task will print out "OK" and the current simulated tick
 * time.  If an error is discovered in the execution of a task then the check
 * task will print out an appropriate error message.
 *
 */


/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* Kernel includes. */
#include <FreeRTOS.h>
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Standard demo includes. */
#include "BlockQ.h"
#include "integer.h"
#include "semtest.h"
#include "PollQ.h"
#include "GenQTest.h"
#include "QPeek.h"
#include "recmutex.h"
#include "flop.h"
#include "TimerDemo.h"
#include "countsem.h"
#include "death.h"
#include "dynamic.h"
#include "QueueSet.h"
#include "QueueOverwrite.h"
#include "EventGroupsDemo.h"
#include "IntSemTest.h"
#include "TaskNotify.h"
#include "QueueSetPolling.h"
#include "blocktim.h"
#include "AbortDelay.h"

/* Priorities at which the tasks are created. */
#define mainCHECK_TASK_PRIORITY			( configMAX_PRIORITIES - 2 )
#define mainQUEUE_POLL_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainSEM_TEST_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY			( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define mainFLASH_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define mainINTEGER_TASK_PRIORITY		( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY		( tskIDLE_PRIORITY )
#define mainFLOP_TASK_PRIORITY			( tskIDLE_PRIORITY )
#define mainQUEUE_OVERWRITE_PRIORITY	( tskIDLE_PRIORITY )

#define mainTIMER_TEST_PERIOD			( 50 )

/* Task function prototypes. */
static void prvCheckTask( void *pvParameters );

/* A task that is created from the idle task to test the functionality of
eTaskStateGet(). */
static void prvTestTask( void *pvParameters );

/*
 * Called from the idle task hook function to demonstrate a few utility
 * functions that are not demonstrated by any of the standard demo tasks.
 */
static void prvDemonstrateTaskStateAndHandleGetFunctions( void );

/*
 * Called from the idle task hook function to demonstrate the use of
 * xTimerPendFunctionCall() as xTimerPendFunctionCall() is not demonstrated by
 * any of the standard demo tasks.
 */
static void prvDemonstratePendingFunctionCall( void );

/*
 * The function that is pended by prvDemonstratePendingFunctionCall().
 */
static void prvPendedFunction( void *pvParameter1, uint32_t ulParameter2 );

/*
 * A task to demonstrate the use of the xQueueSpacesAvailable() function.
 */
static void prvDemoQueueSpaceFunctions( void *pvParameters );

/*-----------------------------------------------------------*/

/* The variable into which error messages are latched. */
static char *pcStatusMessage = "OK";

/* This semaphore is created purely to test using the vSemaphoreDelete() and
semaphore tracing API functions.  It has no other purpose. */
static SemaphoreHandle_t xMutexToDelete = NULL;

/*-----------------------------------------------------------*/

int main_full( void )
{
	/* Start the check task as described at the top of this file. */
	xTaskCreate( prvCheckTask, "Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );

	/* Create the standard demo tasks. */
	vStartTaskNotifyTask();
	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
	vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
	vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
	vStartIntegerMathTasks( mainINTEGER_TASK_PRIORITY );
	vStartGenericQueueTasks( mainGEN_QUEUE_TASK_PRIORITY );
	vStartQueuePeekTasks();
	vStartMathTasks( mainFLOP_TASK_PRIORITY );
	vStartRecursiveMutexTasks();
	vStartCountingSemaphoreTasks();
	vStartDynamicPriorityTasks();
	vStartQueueSetTasks();
	vStartQueueOverwriteTask( mainQUEUE_OVERWRITE_PRIORITY );
	xTaskCreate( prvDemoQueueSpaceFunctions, "QSpace", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	vStartEventGroupTasks();
	vStartInterruptSemaphoreTasks();
	vStartQueueSetPollingTask();
	vCreateBlockTimeTasks();
	vCreateAbortDelayTasks();

	#if( configUSE_PREEMPTION != 0  )
	{
		/* Don't expect these tasks to pass when preemption is not used. */
		vStartTimerDemoTask( mainTIMER_TEST_PERIOD );
	}
	#endif

	/* The suicide tasks must be created last as they need to know how many
	tasks were running prior to their creation.  This then allows them to
	ascertain whether or not the correct/expected number of tasks are running at
	any given time. */
	vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );

	/* Create the semaphore that will be deleted in the idle task hook.  This
	is done purely to test the use of vSemaphoreDelete(). */
	xMutexToDelete = xSemaphoreCreateMutex();

	/* Start the scheduler itself. */
	vTaskStartScheduler();

	/* Should never get here unless there was not enough heap space to create
	the idle and other system tasks. */
	return 0;
}
/*-----------------------------------------------------------*/

static void prvCheckTask( void *pvParameters )
{
TickType_t xNextWakeTime;
const TickType_t xCycleFrequency = pdMS_TO_TICKS( 2500UL );

	/* Just to remove compiler warning. */
	( void ) pvParameters;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ;; )
	{
		/* Place this task in the blocked state until it is time to run again. */
		vTaskDelayUntil( &xNextWakeTime, xCycleFrequency );

		/* Check the standard demo tasks are running without error. */
		#if( configUSE_PREEMPTION != 0 )
		{
			/* These tasks are only created when preemption is used. */
			if( xAreTimerDemoTasksStillRunning( xCycleFrequency ) != pdTRUE )
			{
				pcStatusMessage = "Error: TimerDemo";
			}
		}
		#endif

		if( xAreTaskNotificationTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error:  Notification";
		}

		if( xAreInterruptSemaphoreTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: IntSem";
		}
		else if( xAreEventGroupTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: EventGroup";
		}
	    else if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
	    {
			pcStatusMessage = "Error: IntMath";
	    }
		else if( xAreGenericQueueTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: GenQueue";
		}
		else if( xAreQueuePeekTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: QueuePeek";
		}
		else if( xAreBlockingQueuesStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: BlockQueue";
		}
	    else if( xAreSemaphoreTasksStillRunning() != pdTRUE )
	    {
			pcStatusMessage = "Error: SemTest";
	    }
	    else if( xArePollingQueuesStillRunning() != pdTRUE )
	    {
			pcStatusMessage = "Error: PollQueue";
	    }
		else if( xAreMathsTaskStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Flop";
		}
	    else if( xAreRecursiveMutexTasksStillRunning() != pdTRUE )
	    {
			pcStatusMessage = "Error: RecMutex";
		}
		else if( xAreCountingSemaphoreTasksStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: CountSem";
		}
		else if( xIsCreateTaskStillRunning() != pdTRUE )
		{
			pcStatusMessage = "Error: Death";
		}
		else if( xAreDynamicPriorityTasksStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Dynamic";
		}
		else if( xAreQueueSetTasksStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Queue set";
		}
		else if( xIsQueueOverwriteTaskStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Queue overwrite";
		}
		else if( xAreQueueSetPollTasksStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Queue set polling";
		}
		else if( xAreBlockTimeTestTasksStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Block time";
		}
		else if( xAreAbortDelayTestTasksStillRunning() != pdPASS )
		{
			pcStatusMessage = "Error: Abort delay";
		}

		/* This is the only task that uses stdout so its ok to call printf()
		directly. */
		printf( ( char * ) "%s - %u\r\n", pcStatusMessage, ( unsigned int ) xTaskGetTickCount() );
		fflush( stdout );
	}
}
/*-----------------------------------------------------------*/

static void prvTestTask( void *pvParameters )
{
const unsigned long ulMSToSleep = 5;

	/* Just to remove compiler warnings. */
	( void ) pvParameters;

	/* This task is just used to test the eTaskStateGet() function.  It
	does not have anything to do. */
	for( ;; )
	{
		/* Sleep to reduce CPU load, but don't sleep indefinitely in case there are
		tasks waiting to be terminated by the idle task. */
		Sleep( ulMSToSleep );
	}
}
/*-----------------------------------------------------------*/

/* Called from vApplicationIdleHook(), which is defined in main.c. */
void vFullDemoIdleFunction( void )
{
const unsigned long ulMSToSleep = 15;
void *pvAllocated;

	/* Sleep to reduce CPU load, but don't sleep indefinitely in case there are
	tasks waiting to be terminated by the idle task. */
	Sleep( ulMSToSleep );

	/* Demonstrate a few utility functions that are not demonstrated by any of
	the standard demo tasks. */
	prvDemonstrateTaskStateAndHandleGetFunctions();

	/* Demonstrate the use of xTimerPendFunctionCall(), which is not
	demonstrated by any of the standard demo tasks. */
	prvDemonstratePendingFunctionCall();

	/* If xMutexToDelete has not already been deleted, then delete it now.
	This is done purely to demonstrate the use of, and test, the
	vSemaphoreDelete() macro.  Care must be taken not to delete a semaphore
	that has tasks blocked on it. */
	if( xMutexToDelete != NULL )
	{
		vSemaphoreDelete( xMutexToDelete );
		xMutexToDelete = NULL;
	}

	/* Exercise heap_5 a bit.  The malloc failed hook will trap failed
	allocations so there is no need to test here. */
	pvAllocated = pvPortMalloc( ( rand() % 100 ) + 1 );
	vPortFree( pvAllocated );
}
/*-----------------------------------------------------------*/

/* Called by vApplicationTickHook(), which is defined in main.c. */
void vFullDemoTickHookFunction( void )
{
	/* Call the periodic timer test, which tests the timer API functions that
	can be called from an ISR. */
	#if( configUSE_PREEMPTION != 0 )
	{
		/* Only created when preemption is used. */
		vTimerPeriodicISRTests();
	}
	#endif

	/* Call the periodic queue overwrite from ISR demo. */
	vQueueOverwritePeriodicISRDemo();

	/* Write to a queue that is in use as part of the queue set demo to
	demonstrate using queue sets from an ISR. */
	vQueueSetAccessQueueSetFromISR();
	vQueueSetPollingInterruptAccess();

	/* Exercise event groups from interrupts. */
	vPeriodicEventGroupsProcessing();

	/* Exercise giving mutexes from an interrupt. */
	vInterruptSemaphorePeriodicTest();

	/* Exercise using task notifications from an interrupt. */
	xNotifyTaskFromISR();
}
/*-----------------------------------------------------------*/

static void prvPendedFunction( void *pvParameter1, uint32_t ulParameter2 )
{
static uint32_t ulLastParameter1 = 1000UL, ulLastParameter2 = 0UL;
uint32_t ulParameter1;

	ulParameter1 = ( uint32_t ) pvParameter1;

	/* Ensure the parameters are as expected. */
	configASSERT( ulParameter1 == ( ulLastParameter1 + 1 ) );
	configASSERT( ulParameter2 == ( ulLastParameter2 + 1 ) );

	/* Remember the parameters for the next time the function is called. */
	ulLastParameter1 = ulParameter1;
	ulLastParameter2 = ulParameter2;
}
/*-----------------------------------------------------------*/

static void prvDemonstratePendingFunctionCall( void )
{
static uint32_t ulParameter1 = 1000UL, ulParameter2 = 0UL;
const TickType_t xDontBlock = 0; /* This is called from the idle task so must *not* attempt to block. */

	/* prvPendedFunction() just expects the parameters to be incremented by one
	each time it is called. */
	ulParameter1++;
	ulParameter2++;

	/* Pend the function call, sending the parameters. */
	xTimerPendFunctionCall( prvPendedFunction, ( void * ) ulParameter1, ulParameter2, xDontBlock );
}
/*-----------------------------------------------------------*/

static void prvDemonstrateTaskStateAndHandleGetFunctions( void )
{
TaskHandle_t xIdleTaskHandle, xTimerTaskHandle;
char *pcTaskName;
static portBASE_TYPE xPerformedOneShotTests = pdFALSE;
TaskHandle_t xTestTask;

	/* Demonstrate the use of the xTimerGetTimerDaemonTaskHandle() and
	xTaskGetIdleTaskHandle() functions.  Also try using the function that sets
	the task number. */
	xIdleTaskHandle = xTaskGetIdleTaskHandle();
	xTimerTaskHandle = xTimerGetTimerDaemonTaskHandle();

	/* This is the idle hook, so the current task handle should equal the
	returned idle task handle. */
	if( xTaskGetCurrentTaskHandle() != xIdleTaskHandle )
	{
		pcStatusMessage = "Error:  Returned idle task handle was incorrect";
	}

	/* Check the timer task handle was returned correctly. */
	pcTaskName = pcTaskGetName( xTimerTaskHandle );
	if( strcmp( pcTaskName, "Tmr Svc" ) != 0 )
	{
		pcStatusMessage = "Error:  Returned timer task handle was incorrect";
	}

	/* This task is running, make sure it's state is returned as running. */
	if( eTaskStateGet( xIdleTaskHandle ) != eRunning )
	{
		pcStatusMessage = "Error:  Returned idle task state was incorrect";
	}

	/* If this task is running, then the timer task must be blocked. */
	if( eTaskStateGet( xTimerTaskHandle ) != eBlocked )
	{
		pcStatusMessage = "Error:  Returned timer task state was incorrect";
	}

	/* Other tests that should only be performed once follow.  The test task
	is not created on each iteration because to do so would cause the death
	task to report an error (too many tasks running). */
	if( xPerformedOneShotTests == pdFALSE )
	{
		/* Don't run this part of the test again. */
		xPerformedOneShotTests = pdTRUE;

		/* Create a test task to use to test other eTaskStateGet() return values. */
		if( xTaskCreate( prvTestTask, "Test", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTestTask ) == pdPASS )
		{
			/* If this task is running, the test task must be in the ready state. */
			if( eTaskStateGet( xTestTask ) != eReady )
			{
				pcStatusMessage = "Error: Returned test task state was incorrect 1";
			}

			/* Now suspend the test task and check its state is reported correctly. */
			vTaskSuspend( xTestTask );
			if( eTaskStateGet( xTestTask ) != eSuspended )
			{
				pcStatusMessage = "Error: Returned test task state was incorrect 2";
			}

			/* Now delete the task and check its state is reported correctly. */
			vTaskDelete( xTestTask );
			if( eTaskStateGet( xTestTask ) != eDeleted )
			{
				pcStatusMessage = "Error: Returned test task state was incorrect 3";
			}
		}
	}
}
/*-----------------------------------------------------------*/

static void prvDemoQueueSpaceFunctions( void *pvParameters )
{
QueueHandle_t xQueue = NULL;
const unsigned portBASE_TYPE uxQueueLength = 10;
unsigned portBASE_TYPE uxReturn, x;

	/* Remove compiler warnings. */
	( void ) pvParameters;

	/* Create the queue that will be used.  Nothing is actually going to be
	sent or received so the queue item size is set to 0. */
	xQueue = xQueueCreate( uxQueueLength, 0 );
	configASSERT( xQueue );

	for( ;; )
	{
		for( x = 0; x < uxQueueLength; x++ )
		{
			/* Ask how many messages are available... */
			uxReturn = uxQueueMessagesWaiting( xQueue );

			/* Check the number of messages being reported as being available
			is as expected, and force an assert if not. */
			if( uxReturn != x )
			{
				/* xQueue cannot be NULL so this is deliberately causing an
				assert to be triggered as there is an error. */
				configASSERT( xQueue == NULL );
			}

			/* Ask how many spaces remain in the queue... */
			uxReturn = uxQueueSpacesAvailable( xQueue );

			/* Check the number of spaces being reported as being available
			is as expected, and force an assert if not. */
			if( uxReturn != ( uxQueueLength - x ) )
			{
				/* xQueue cannot be NULL so this is deliberately causing an
				assert to be triggered as there is an error. */
				configASSERT( xQueue == NULL );
			}

			/* Fill one more space in the queue. */
			xQueueSendToBack( xQueue, NULL, 0 );
		}

		/* Perform the same check while the queue is full. */
		uxReturn = uxQueueMessagesWaiting( xQueue );
		if( uxReturn != uxQueueLength )
		{
			configASSERT( xQueue == NULL );
		}

		uxReturn = uxQueueSpacesAvailable( xQueue );

		if( uxReturn != 0 )
		{
			configASSERT( xQueue == NULL );
		}

		/* The queue is full, start again. */
		xQueueReset( xQueue );

		#if( configUSE_PREEMPTION == 0 )
			taskYIELD();
		#endif
	}
}


