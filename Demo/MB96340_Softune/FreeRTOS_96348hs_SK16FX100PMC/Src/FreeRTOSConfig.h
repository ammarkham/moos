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

/* Device specific includes. */
#include "mb96348hs.h"

/* 
 * The below define should be same as the option selected by the Memory 
 * Model (Project->Setup Project->C Compiler->Category->Target Depend ) 
 *
 * Valid settings here include:
 * ------- Memory models ---------      Data	  Code
 * portSMALL							16 Bit    16 Bit
 * portMEDIUM							16 Bit    24 Bit
 * portCOMPACT							24 Bit    16 Bit
 * portLARGE							24 Bit    24 Bit
 */
#define configMEMMODEL portMEDIUM

/* Demo specific definition - set this to 1 if you want to include the task
that writes trace and debug information to the UART.  If it is set to 0 then
the ComTest tasks will be included in place of the trace task. */
#define INCLUDE_TraceListTasks		0

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *----------------------------------------------------------*/
#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			1
#define configUSE_TICK_HOOK			0
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 180 ) /* This can be greatly reduced when using the small or medium memory model. */
#define configCPU_CLOCK_HZ			( ( unsigned long ) 56000000 )	/* Clock setup from start.asm in the demo application. */
#define configCLKP1_CLOCK_HZ		( ( unsigned long ) 56000000 )	/* Clock setup from start.asm in the demo application. */
#define configTICK_RATE_HZ			( (portTickType) 1000 )
#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 6 )
#define configTOTAL_HEAP_SIZE		( (size_t) (20000) )
#define configMAX_TASK_NAME_LEN		( 20 )
#define configUSE_16_BIT_TICKS		1
#define configIDLE_SHOULD_YIELD		1
#define configUSE_MUTEXES			1
#define configUSE_TRACE_FACILITY	1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES			1
#define configMAX_CO_ROUTINE_PRIORITIES ( 4 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet			1
#define INCLUDE_uxTaskPriorityGet			1
#define INCLUDE_vTaskDelete					1
#define INCLUDE_vTaskCleanUpResources		1
#define INCLUDE_vTaskSuspend				1
#define INCLUDE_vResumeFromISR				1
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay					1
#define INCLUDE_xTaskGetSchedulerState		1
#define INCLUDE_xTaskGetCurrentTaskHandle	1


#define configKERNEL_INTERRUPT_PRIORITY 6

#endif /* FREERTOS_CONFIG_H */
