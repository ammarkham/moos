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
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/tcpip.h"

/* lwIP netif includes */
#include "netif/etharp.h"

/* applications includes */
#include "apps/httpserver_raw/httpd.h"

/* The constants that define the IP address, net mask, gateway address and MAC
address are located at the bottom of FreeRTOSConfig.h. */
#define LWIP_PORT_INIT_IPADDR(addr)   IP4_ADDR((addr), configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3 )
#define LWIP_PORT_INIT_GW(addr)       IP4_ADDR((addr), configGW_IP_ADDR0, configGW_IP_ADDR1, configGW_IP_ADDR2, configGW_IP_ADDR3 )
#define LWIP_PORT_INIT_NETMASK(addr)  IP4_ADDR((addr), configNET_MASK0,configNET_MASK1,configNET_MASK2,configNET_MASK3)
#define LWIP_MAC_ADDR_BASE            { configMAC_ADDR0, configMAC_ADDR1, configMAC_ADDR2, configMAC_ADDR3, configMAC_ADDR4, configMAC_ADDR5 }

/* Definitions of the various SSI callback functions within the pccSSITags 
array.  If pccSSITags is updated, then these definitions must also be updated. */
#define ssiTASK_STATS_INDEX			0
#define ssiRUN_TIME_STATS_INDEX		1

/*
 * The SSI handler callback function passed to lwIP.
 */
static unsigned short uslwIPAppsSSIHandler( int iIndex, char *pcBuffer, int iBufferLength );

/*-----------------------------------------------------------*/

/* The SSI strings that are embedded in the served html files.  If this array
is changed, then the index position defined by the #defines such as 
ssiTASK_STATS_INDEX above must also be updated. */
static const char *pccSSITags[] = 
{
	"rtos_stats",
	"run_stats"
};

/*-----------------------------------------------------------*/

/* Called from the TCP/IP thread. */
void lwIPAppsInit( void *pvArgument )
{
ip_addr_t xIPAddr, xNetMask, xGateway;
extern err_t ethernetif_init( struct netif *xNetIf );
static struct netif xNetIf;

	( void ) pvArgument;

	/* Set up the network interface. */
	ip_addr_set_zero( &xGateway );
	ip_addr_set_zero( &xIPAddr );
	ip_addr_set_zero( &xNetMask );

	LWIP_PORT_INIT_GW(&xGateway);
	LWIP_PORT_INIT_IPADDR(&xIPAddr);
	LWIP_PORT_INIT_NETMASK(&xNetMask);

	netif_set_default( netif_add( &xNetIf, &xIPAddr, &xNetMask, &xGateway, NULL, ethernetif_init, tcpip_input ) );
	netif_set_up( &xNetIf );

	/* Initialise the raw http server. */
	httpd_init();

	/* Install the server side include handler. */
	http_set_ssi_handler( uslwIPAppsSSIHandler, pccSSITags, sizeof( pccSSITags ) / sizeof( char * ) );
}
/*-----------------------------------------------------------*/

static unsigned short uslwIPAppsSSIHandler( int iIndex, char *pcBuffer, int iBufferLength )
{
static unsigned int uiUpdateCount = 0;
static char cUpdateString[ 200 ];
extern char *pcMainGetTaskStatusMessage( void );

	/* Unused parameter. */
	( void ) iBufferLength;

	/* The SSI handler function that generates text depending on the index of
	the SSI tag encountered. */
	
	switch( iIndex )
	{
		case ssiTASK_STATS_INDEX :
			vTaskList( ( signed char * ) pcBuffer );
			break;

		case ssiRUN_TIME_STATS_INDEX :
			vTaskGetRunTimeStats( ( signed char * ) pcBuffer );
			break;
	}

	/* Include a count of the number of times an SSI function has been executed
	in the returned string. */
	uiUpdateCount++;
	sprintf( cUpdateString, "\r\n\r\n%u\r\nStatus - %s", uiUpdateCount, pcMainGetTaskStatusMessage() );
	strcat( pcBuffer, cUpdateString );
	return strlen( pcBuffer );
}

