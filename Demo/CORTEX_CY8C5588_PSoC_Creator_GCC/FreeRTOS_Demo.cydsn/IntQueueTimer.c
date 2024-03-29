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

#include <device.h>
#include "FreeRTOS.h"
#include "task.h"
/*---------------------------------------------------------------------------*/

extern portBASE_TYPE xFirstTimerHandler( void );
extern portBASE_TYPE xSecondTimerHandler( void );
/*---------------------------------------------------------------------------*/

CY_ISR_PROTO( vHighFrequencyFirstISR );
CY_ISR_PROTO( vHighFrequencySecondISR );
/*---------------------------------------------------------------------------*/

/**
 * Installs and starts the ISRs that drive the Interupt Queue Tests.
 */
void vInitialiseTimerForIntQueueTest( void )
{
	taskENTER_CRITICAL();
	{
		/* Initialise and start the First Timer ISR. */
		isr_High_Frequency_2000Hz_ClearPending();
		isr_High_Frequency_2000Hz_StartEx( ( cyisraddress ) vHighFrequencyFirstISR );

		/* Initialise and start the Second Timer ISR. */
		isr_High_Frequency_2001Hz_ClearPending();
		isr_High_Frequency_2001Hz_StartEx( ( cyisraddress ) vHighFrequencySecondISR );
	}
	taskEXIT_CRITICAL();
}
/*---------------------------------------------------------------------------*/

CY_ISR(vHighFrequencyFirstISR)
{
	/* Call back into the test code and context switch if necessary. */
	portEND_SWITCHING_ISR( xFirstTimerHandler() );
}
/*---------------------------------------------------------------------------*/

CY_ISR(vHighFrequencySecondISR)
{
	/* Call back into the test code and context switch if necessary. */
	portEND_SWITCHING_ISR( xSecondTimerHandler() );
}
/*---------------------------------------------------------------------------*/
