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

#include "FreeRTOS.h"
#include "IntQueueTimer.h"
#include "IntQueue.h"

#define timerINTERRUPT3_FREQUENCY	( 2000UL )
#define timerINTERRUPT4_FREQUENCY	( 2001UL )

void vT3InterruptHandler( void );
void vT4InterruptHandler( void );

/* As these interrupts use the FreeRTOS interrupt entry point, the IPL settings
in the following prototypes have no effect.  The interrupt priorities are set
by the ConfigIntTimerX() library calls in vInitialiseTimerForIntQueueTest(). */
void __attribute__( (interrupt(ipl0), vector(_TIMER_3_VECTOR))) vT3InterruptWrapper( void );
void __attribute__( (interrupt(ipl0), vector(_TIMER_4_VECTOR))) vT4InterruptWrapper( void );

void vInitialiseTimerForIntQueueTest( void )
{
	/* Timer 1 is used for the tick interrupt, timer 2 is used for the high
	frequency interrupt test.  This file therefore uses timers 3 and 4. */

	T3CON = 0;
	TMR3 = 0;
	PR3 = ( unsigned short ) ( configPERIPHERAL_CLOCK_HZ / timerINTERRUPT3_FREQUENCY );

	/* Setup timer 3 interrupt priority to be above the kernel priority. */
	ConfigIntTimer3( T3_INT_ON | ( configMAX_SYSCALL_INTERRUPT_PRIORITY - 1 ) );

	/* Clear the interrupt as a starting condition. */
	IFS0bits.T3IF = 0;

	/* Enable the interrupt. */
	IEC0bits.T3IE = 1;

	/* Start the timer. */
	T3CONbits.TON = 1;


	/* Do the same for timer 4. */
	T4CON = 0;
	TMR4 = 0;
	PR4 = ( unsigned short ) ( configPERIPHERAL_CLOCK_HZ / timerINTERRUPT4_FREQUENCY );

	/* Setup timer 4 interrupt priority to be above the kernel priority. */
	ConfigIntTimer4( T4_INT_ON | ( configMAX_SYSCALL_INTERRUPT_PRIORITY ) );

	/* Clear the interrupt as a starting condition. */
	IFS0bits.T4IF = 0;

	/* Enable the interrupt. */
	IEC0bits.T4IE = 1;

	/* Start the timer. */
	T4CONbits.TON = 1;
}
/*-----------------------------------------------------------*/

void vT3InterruptHandler( void )
{
	IFS0bits.T3IF = 0;
	portEND_SWITCHING_ISR( xFirstTimerHandler() );
}
/*-----------------------------------------------------------*/

void vT4InterruptHandler( void )
{
	IFS0bits.T4IF = 0;
	portEND_SWITCHING_ISR( xSecondTimerHandler() );
}


