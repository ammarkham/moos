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

/* High speed timer test as described in main.c. */

/* Scheduler includes. */
#include "FreeRTOS.h"

/* Library includes. */
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "interrupt.h"
#include "sysctl.h"
#include "lmi_timer.h"

/* The set frequency of the interrupt.  Deviations from this are measured as
the jitter. */
#define timerINTERRUPT_FREQUENCY		( 20000UL )

/* The expected time between each of the timer interrupts - if the jitter was
zero. */
#define timerEXPECTED_DIFFERENCE_VALUE	( configCPU_CLOCK_HZ / timerINTERRUPT_FREQUENCY )

/* The highest available interrupt priority. */
#define timerHIGHEST_PRIORITY			( 0 )

/* Misc defines. */
#define timerMAX_32BIT_VALUE			( 0xffffffffUL )
#define timerTIMER_1_COUNT_VALUE		( * ( ( unsigned long * ) ( TIMER1_BASE + 0x48 ) ) )

/*-----------------------------------------------------------*/

/* Interrupt handler in which the jitter is measured. */
void Timer0IntHandler( void );

/* Stores the value of the maximum recorded jitter between interrupts. */
volatile unsigned long ulMaxJitter = 0UL;

/* Counts the total number of times that the high frequency timer has 'ticked'.
This value is used by the run time stats function to work out what percentage
of CPU time each task is taking. */
volatile unsigned long ulHighFrequencyTimerTicks = 0UL;
/*-----------------------------------------------------------*/

void vSetupHighFrequencyTimer( void )
{
unsigned long ulFrequency;

	/* Timer zero is used to generate the interrupts, and timer 1 is used
	to measure the jitter. */
	SysCtlPeripheralEnable( SYSCTL_PERIPH_TIMER0 );
    SysCtlPeripheralEnable( SYSCTL_PERIPH_TIMER1 );
    TimerConfigure( TIMER0_BASE, TIMER_CFG_32_BIT_PER );
    TimerConfigure( TIMER1_BASE, TIMER_CFG_32_BIT_PER );

	/* Set the timer interrupt to be above the kernel - highest. */
	IntPrioritySet( INT_TIMER0A, timerHIGHEST_PRIORITY );

	/* Just used to measure time. */
    TimerLoadSet(TIMER1_BASE, TIMER_A, timerMAX_32BIT_VALUE );

	/* Ensure interrupts do not start until the scheduler is running. */
	portDISABLE_INTERRUPTS();

	/* The rate at which the timer will interrupt. */
	ulFrequency = configCPU_CLOCK_HZ / timerINTERRUPT_FREQUENCY;
    TimerLoadSet( TIMER0_BASE, TIMER_A, ulFrequency );
    IntEnable( INT_TIMER0A );
    TimerIntEnable( TIMER0_BASE, TIMER_TIMA_TIMEOUT );

	/* Enable both timers. */
    TimerEnable( TIMER0_BASE, TIMER_A );
    TimerEnable( TIMER1_BASE, TIMER_A );
}
/*-----------------------------------------------------------*/

void Timer0IntHandler( void )
{
unsigned long ulDifference;
volatile unsigned long ulCurrentCount;
static unsigned long ulMaxDifference = 0, ulLastCount = 0;

	/* We use the timer 1 counter value to measure the clock cycles between
	the timer 0 interrupts. */
	ulCurrentCount = timerTIMER_1_COUNT_VALUE;

	TimerIntClear( TIMER0_BASE, TIMER_TIMA_TIMEOUT );

	if( ulCurrentCount < ulLastCount )
	{
		/* How many times has timer 1 counted since the last interrupt? */
		ulDifference = 	ulLastCount - ulCurrentCount;

		/* Is this the largest difference we have measured yet? */
		if( ulDifference > ulMaxDifference )
		{
			ulMaxDifference = ulDifference;
			ulMaxJitter = ulMaxDifference - timerEXPECTED_DIFFERENCE_VALUE;
		}
	}

	ulLastCount = ulCurrentCount;

	/* Keep a count of the total number of 20KHz ticks.  This is used by the
	run time stats functionality to calculate how much CPU time is used by
	each task. */
	ulHighFrequencyTimerTicks++;
}





