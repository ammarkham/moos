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


/* Standard includes. */
#include <stdlib.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Application includes. */
#include "tcp.h"

/* Misc constants. */
#define tcpPOLL_DELAY					( ( portTickType ) 12 / portTICK_RATE_MS )
#define tcpCONNECTION_DELAY				( ( portTickType ) 8 / portTICK_RATE_MS )
/*-----------------------------------------------------------*/

/*
 * This task initialises the hardware then processes one TCP connection at a
 * time.  When an HTTP client connects we just simply send a single page then
 * disconnect - reset the socket data and wait for the next connection.
 */
void vHTTPServerTask( void *pvParameters )
{
	/* Reset the network hardware. */
	vTCPHardReset();

	/* Loop, processing connections are they arrive. */
	for( ;; )
	{
		/* Initialise the TCP interface.

		The current minimal implementation does not check for buffer overflows
		in the WIZnet hardware, so simply resets all the buffers for each
		connection - and only processes one connection at a time. */
		if( lTCPSoftReset() )
		{	  
			/* Create the socket that is going to accept incoming connections. */
			if( lTCPCreateSocket() )
			{
				/* Wait for a connection. */
				vTCPListen();

				/* Process connections as they arrive.  This function will only
				return once the connection has been closed. */
				lProcessConnection();
			}
		}

		/* If we get here then the connection completed or failed.  Wait a 
		while then try or start again. */
		vTaskDelay( tcpCONNECTION_DELAY );		
	}
}

