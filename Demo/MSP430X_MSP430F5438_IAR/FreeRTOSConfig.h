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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				1
#define configUSE_TICK_HOOK				1
#define configCPU_CLOCK_HZ				( 25000000UL )	
#define configTICK_RATE_HZ				( ( portTickType ) 1000 )
#define configMAX_PRIORITIES			( ( unsigned portBASE_TYPE ) 5 )
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 10 * 1024 ) )
#define configMAX_TASK_NAME_LEN			( 10 )
#define configUSE_TRACE_FACILITY		0
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			1
#define configUSE_MUTEXES				1
#define configQUEUE_REGISTRY_SIZE		5
#define configGENERATE_RUN_TIME_STATS	1
#define configCHECK_FOR_STACK_OVERFLOW	2
#define configUSE_RECURSIVE_MUTEXES		0
#define configUSE_MALLOC_FAILED_HOOK	1
#define configUSE_APPLICATION_TASK_TAG	0

#if __DATA_MODEL__ == __DATA_MODEL_SMALL__
	#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 110 )
#else
	#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 80 )
#endif

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				0
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

/* The MSP430X port uses a callback function to configure its tick interrupt.
This allows the application to choose the tick interrupt source.
configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
interrupt vector for the chosen tick interrupt source.  This implementation of
vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
case configTICK__VECTOR is set to TIMER0_A0_VECTOR. */
#define configTICK_VECTOR				TIMER0_A0_VECTOR

/* Prevent the following definitions being included when FreeRTOSConfig.h
is included from an asm file. */
#ifdef __ICC430__
	extern void vConfigureTimerForRunTimeStats( void );
	extern volatile unsigned long ulStatsOverflowCount;
#endif /* __ICCARM__ */

/* Configure a 16 bit timer to generate the time base for the run time stats.
The timer is configured to interrupt each time it overflows so a count of
overflows can be kept - that way a 32 bit time value can be constructed from
the timers current count value and the number of overflows. */
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vConfigureTimerForRunTimeStats()
	
/* Construct a 32 bit time value for use as the run time stats time base.  This
comes from the current value of a 16 bit timer combined with the number of times
the timer has overflowed. */
#define portALT_GET_RUN_TIME_COUNTER_VALUE( ulCountValue )						\
	{																			\
		/* Stop the counter counting temporarily. */							\
		TA1CTL &= ~MC__CONTINOUS;												\
																				\
		/* Check to see if any counter overflow interrupts are pending. */		\
		if( ( TA1CTL & TAIFG ) != 0 )											\
		{																		\
			/* An overflow has occurred but not yet been processed. */			\
			ulStatsOverflowCount++;												\
																				\
			/* Clear the interrupt. */											\
			TA1CTL &= ~TAIFG;													\
		}																		\
																				\
		/* Generate a 32 bit counter value by combinging the current peripheral	\
		counter value with the number of overflows. */							\
		ulCountValue = ( ulStatsOverflowCount << 16UL );						\
		ulCountValue |= ( unsigned long ) TA1R;									\
		TA1CTL |= MC__CONTINOUS;												\
	}
#endif /* FREERTOS_CONFIG_H */

