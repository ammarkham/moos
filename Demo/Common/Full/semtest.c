/*
    FreeRTOS V7.0.2 - Copyright (C) 2011 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/**
 * Creates two sets of two tasks.  The tasks within a set share a variable, access 
 * to which is guarded by a semaphore.
 * 
 * Each task starts by attempting to obtain the semaphore.  On obtaining a 
 * semaphore a task checks to ensure that the guarded variable has an expected 
 * value.  It then clears the variable to zero before counting it back up to the 
 * expected value in increments of 1.  After each increment the variable is checked 
 * to ensure it contains the value to which it was just set. When the starting 
 * value is again reached the task releases the semaphore giving the other task in 
 * the set a chance to do exactly the same thing.  The starting value is high 
 * enough to ensure that a tick is likely to occur during the incrementing loop.
 *
 * An error is flagged if at any time during the process a shared variable is 
 * found to have a value other than that expected.  Such an occurrence would 
 * suggest an error in the mutual exclusion mechanism by which access to the 
 * variable is restricted.
 *
 * The first set of two tasks poll their semaphore.  The second set use blocking 
 * calls.
 *
 * \page SemTestC semtest.c
 * \ingroup DemoFiles
 * <HR>
 */

/*
Changes from V1.2.0:

	+ The tasks that operate at the idle priority now use a lower expected
	  count than those running at a higher priority.  This prevents the low
	  priority tasks from signaling an error because they have not been 
	  scheduled enough time for each of them to count the shared variable to
	  the high value.

Changes from V2.0.0

	+ Delay periods are now specified using variables and constants of
	  portTickType rather than unsigned long.

Changes from V2.1.1

	+ The stack size now uses configMINIMAL_STACK_SIZE.
	+ String constants made file scope to decrease stack depth on 8051 port.
*/

#include <stdlib.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Demo app include files. */
#include "semtest.h"
#include "print.h"

/* The value to which the shared variables are counted. */
#define semtstBLOCKING_EXPECTED_VALUE		( ( unsigned long ) 0xfff )
#define semtstNON_BLOCKING_EXPECTED_VALUE	( ( unsigned long ) 0xff  )

#define semtstSTACK_SIZE			configMINIMAL_STACK_SIZE

#define semtstNUM_TASKS				( 4 )

#define semtstDELAY_FACTOR			( ( portTickType ) 10 )

/* The task function as described at the top of the file. */
static void prvSemaphoreTest( void *pvParameters );

/* Structure used to pass parameters to each task. */
typedef struct SEMAPHORE_PARAMETERS
{
	xSemaphoreHandle xSemaphore;
	volatile unsigned long *pulSharedVariable;
	portTickType xBlockTime;
} xSemaphoreParameters;

/* Variables used to check that all the tasks are still running without errors. */
static volatile short sCheckVariables[ semtstNUM_TASKS ] = { 0 };
static volatile short sNextCheckVariable = 0;

/* Strings to print if USE_STDIO is defined. */
const char * const pcPollingSemaphoreTaskError = "Guarded shared variable in unexpected state.\r\n";
const char * const pcSemaphoreTaskStart = "Guarded shared variable task started.\r\n";

/*-----------------------------------------------------------*/

void vStartSemaphoreTasks( unsigned portBASE_TYPE uxPriority )
{
xSemaphoreParameters *pxFirstSemaphoreParameters, *pxSecondSemaphoreParameters;
const portTickType xBlockTime = ( portTickType ) 100;

	/* Create the structure used to pass parameters to the first two tasks. */
	pxFirstSemaphoreParameters = ( xSemaphoreParameters * ) pvPortMalloc( sizeof( xSemaphoreParameters ) );

	if( pxFirstSemaphoreParameters != NULL )
	{
		/* Create the semaphore used by the first two tasks. */
		vSemaphoreCreateBinary( pxFirstSemaphoreParameters->xSemaphore );

		if( pxFirstSemaphoreParameters->xSemaphore != NULL )
		{
			/* Create the variable which is to be shared by the first two tasks. */
			pxFirstSemaphoreParameters->pulSharedVariable = ( unsigned long * ) pvPortMalloc( sizeof( unsigned long ) );

			/* Initialise the share variable to the value the tasks expect. */
			*( pxFirstSemaphoreParameters->pulSharedVariable ) = semtstNON_BLOCKING_EXPECTED_VALUE;

			/* The first two tasks do not block on semaphore calls. */
			pxFirstSemaphoreParameters->xBlockTime = ( portTickType ) 0;

			/* Spawn the first two tasks.  As they poll they operate at the idle priority. */
			xTaskCreate( prvSemaphoreTest, "PolSEM1", semtstSTACK_SIZE, ( void * ) pxFirstSemaphoreParameters, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL );
			xTaskCreate( prvSemaphoreTest, "PolSEM2", semtstSTACK_SIZE, ( void * ) pxFirstSemaphoreParameters, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL );
		}
	}

	/* Do exactly the same to create the second set of tasks, only this time 
	provide a block time for the semaphore calls. */
	pxSecondSemaphoreParameters = ( xSemaphoreParameters * ) pvPortMalloc( sizeof( xSemaphoreParameters ) );
	if( pxSecondSemaphoreParameters != NULL )
	{
		vSemaphoreCreateBinary( pxSecondSemaphoreParameters->xSemaphore );

		if( pxSecondSemaphoreParameters->xSemaphore != NULL )
		{
			pxSecondSemaphoreParameters->pulSharedVariable = ( unsigned long * ) pvPortMalloc( sizeof( unsigned long ) );
			*( pxSecondSemaphoreParameters->pulSharedVariable ) = semtstBLOCKING_EXPECTED_VALUE;
			pxSecondSemaphoreParameters->xBlockTime = xBlockTime / portTICK_RATE_MS;

			xTaskCreate( prvSemaphoreTest, "BlkSEM1", semtstSTACK_SIZE, ( void * ) pxSecondSemaphoreParameters, uxPriority, ( xTaskHandle * ) NULL );
			xTaskCreate( prvSemaphoreTest, "BlkSEM2", semtstSTACK_SIZE, ( void * ) pxSecondSemaphoreParameters, uxPriority, ( xTaskHandle * ) NULL );
		}
	}
}
/*-----------------------------------------------------------*/

static void prvSemaphoreTest( void *pvParameters )
{
xSemaphoreParameters *pxParameters;
volatile unsigned long *pulSharedVariable, ulExpectedValue;
unsigned long ulCounter;
short sError = pdFALSE, sCheckVariableToUse;

	/* See which check variable to use.  sNextCheckVariable is not semaphore 
	protected! */
	portENTER_CRITICAL();
		sCheckVariableToUse = sNextCheckVariable;
		sNextCheckVariable++;
	portEXIT_CRITICAL();

	/* Queue a message for printing to say the task has started. */
	vPrintDisplayMessage( &pcSemaphoreTaskStart );

	/* A structure is passed in as the parameter.  This contains the shared 
	variable being guarded. */
	pxParameters = ( xSemaphoreParameters * ) pvParameters;
	pulSharedVariable = pxParameters->pulSharedVariable;

	/* If we are blocking we use a much higher count to ensure loads of context
	switches occur during the count. */
	if( pxParameters->xBlockTime > ( portTickType ) 0 )
	{
		ulExpectedValue = semtstBLOCKING_EXPECTED_VALUE;
	}
	else
	{
		ulExpectedValue = semtstNON_BLOCKING_EXPECTED_VALUE;
	}

	for( ;; )
	{
		/* Try to obtain the semaphore. */
		if( xSemaphoreTake( pxParameters->xSemaphore, pxParameters->xBlockTime ) == pdPASS )
		{
			/* We have the semaphore and so expect any other tasks using the
			shared variable to have left it in the state we expect to find
			it. */
			if( *pulSharedVariable != ulExpectedValue )
			{
				vPrintDisplayMessage( &pcPollingSemaphoreTaskError );
				sError = pdTRUE;
			}
			
			/* Clear the variable, then count it back up to the expected value
			before releasing the semaphore.  Would expect a context switch or
			two during this time. */
			for( ulCounter = ( unsigned long ) 0; ulCounter <= ulExpectedValue; ulCounter++ )
			{
				*pulSharedVariable = ulCounter;
				if( *pulSharedVariable != ulCounter )
				{
					if( sError == pdFALSE )
					{
						vPrintDisplayMessage( &pcPollingSemaphoreTaskError );
					}
					sError = pdTRUE;
				}
			}

			/* Release the semaphore, and if no errors have occurred increment the check
			variable. */
			if(	xSemaphoreGive( pxParameters->xSemaphore ) == pdFALSE )
			{
				vPrintDisplayMessage( &pcPollingSemaphoreTaskError );
				sError = pdTRUE;
			}

			if( sError == pdFALSE )
			{
				if( sCheckVariableToUse < semtstNUM_TASKS )
				{
					( sCheckVariables[ sCheckVariableToUse ] )++;
				}
			}

			/* If we have a block time then we are running at a priority higher
			than the idle priority.  This task takes a long time to complete
			a cycle	(deliberately so to test the guarding) so will be starving
			out lower priority tasks.  Block for some time to allow give lower
			priority tasks some processor time. */
			vTaskDelay( pxParameters->xBlockTime * semtstDELAY_FACTOR );
		}
		else
		{
			if( pxParameters->xBlockTime == ( portTickType ) 0 )
			{
				/* We have not got the semaphore yet, so no point using the
				processor.  We are not blocking when attempting to obtain the
				semaphore. */
				taskYIELD();
			}
		}
	}
}
/*-----------------------------------------------------------*/

/* This is called to check that all the created tasks are still running. */
portBASE_TYPE xAreSemaphoreTasksStillRunning( void )
{
static short sLastCheckVariables[ semtstNUM_TASKS ] = { 0 };
portBASE_TYPE xTask, xReturn = pdTRUE;

	for( xTask = 0; xTask < semtstNUM_TASKS; xTask++ )
	{
		if( sLastCheckVariables[ xTask ] == sCheckVariables[ xTask ] )
		{
			xReturn = pdFALSE;
		}

		sLastCheckVariables[ xTask ] = sCheckVariables[ xTask ];
	}

	return xReturn;
}


