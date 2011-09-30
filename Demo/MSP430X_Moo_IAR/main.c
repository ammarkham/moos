


/* Standard includes. */
#include <stdlib.h>
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
/* Standard demo includes. */
#include "ParTest.h"
#include "flash.h"

//#include "msp430x26x.h"
//#include "io430.h"


/* Demo task priorities. */
//#define mainCHECK_TASK_PRIORITY			( tskIDLE_PRIORITY + 3 )
//#define mainCOM_TEST_PRIORITY			( tskIDLE_PRIORITY + 2 )
//#define mainQUEUE_POLL_PRIORITY			( tskIDLE_PRIORITY + 2 )
#define mainLED_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )


/* The constants used in the calculation. */
#define intgCONST1				( ( portLONG ) 123 )
#define intgCONST2				( ( portLONG ) 234567 )
#define intgCONST3				( ( portLONG ) -3 )
#define intgCONST4				( ( portLONG ) 7 )
#define intgEXPECTED_ANSWER		( ( ( intgCONST1 + intgCONST2 ) * intgCONST3 ) / intgCONST4 )


portBASE_TYPE xLocalError = pdFALSE;
volatile unsigned portLONG ulIdleLoops = 0UL;

// Uncomment the following to test the LEDs

/*
 * Perform the hardware setup required by the ES449 in order to run the demo
 * application.
 */
static void prvSetupHardware( void );

/*
void main( void )
    {
      
        volatile unsigned long ul; // volatile so it is not optimized away. 

        //Initialise the LED outputs - note that prvSetupHardware() might also 
        //have to be called! 
        vParTestInitialise();

        // Toggle the LEDs repeatedly.
        for( ;; )
        {
            // We don't want to use the RTOS features yet, so just use a very 
            //crude delay mechanism instead. 
            for( ul = 0; ul < 0xfffff; ul++ )
            {
            }
            //volatile unsigned int i;
            //P4OUT ^= 0x04;
            //i = 65000;
            //do(i--);
            //while ( i != 0);

            // Toggle the first four LEDs (on the assumption there are at least 4 fitted. 
            vParTestToggleLED( 0 );
            vParTestToggleLED( 1 );
            //vParTestToggleLED( 2 );
            //vParTestToggleLED( 3 );
        }

        //return 0;
    }
*/


static void prvSetupHardware( void )
{
	/* Stop the watchdog. */
	WDTCTL = WDTPW + WDTHOLD;
         

	/* Setup DCO+ for ( xtal * D * (N + 1) ) operation. */
	//FLL_CTL0 |= DCOPLUS + XCAP18PF;

	/* X2 DCO frequency, 8MHz nominal DCO */
	//SCFI0 |= FN_4;

	/* (121+1) x 32768 x 2 = 7.99 Mhz */
	//SCFQCTL = mainMAX_FREQUENCY;

	/* Setup the IO.  This is just copied from the demo supplied by SoftBaugh
	 for the ES449 demo board. */
	P1SEL = 0x32;
	P2SEL = 0x00;
	P3SEL = 0x00;
	P4SEL = 0xFC;
	P5SEL = 0xFF;
}

int main( void )
    {
        /* Setup the microcontroller hardware for the demo. */
        prvSetupHardware();
		vParTestInitialise();

        /* Leave this function. */
        //vCreateFlashTasks(); 
        /* Start the standard demo application tasks. */
		vStartLEDFlashTasks( mainLED_TASK_PRIORITY );

        /* All other functions that create tasks are commented out.
        
            vCreatePollQTasks();
            vCreateComTestTasks();
            Etc.

            xTaskCreate( vCheckTask, "check", STACK_SIZE, NULL, TASK_PRIORITY, NULL );
        */

        //Start the scheduler. 
        vTaskStartScheduler();

        /* Should never get here! */
        return 0;
    }

/* The idle hook is just a copy of the standard integer maths tasks.  See
Demo/Common/integer.c for rationale. */

void vApplicationIdleHook( void )
{
/* These variables are all effectively set to constants so they are volatile to
ensure the compiler does not just get rid of them. */
volatile portLONG lValue;

	/* Keep performing a calculation and checking the result against a constant. */
	for( ;; )
	{
		/* Perform the calculation.  This will store partial value in
		registers, resulting in a good test of the context switch mechanism. */
		lValue = intgCONST1;
		lValue += intgCONST2;

		/* Yield in case cooperative scheduling is being used. */
		#if configUSE_PREEMPTION == 0
		{
			taskYIELD();
		}
		#endif

		/* Finish off the calculation. */
		lValue *= intgCONST3;
		lValue /= intgCONST4;

		/* If the calculation is found to be incorrect we stop setting the
		TaskHasExecuted variable so the check task can see an error has
		occurred. */
		if( lValue != intgEXPECTED_ANSWER ) /*lint !e774 volatile used to prevent this being optimised out. */
		{
			/* Don't bother with mutual exclusion - it is only read from the
			check task and never written. */
			xLocalError = pdTRUE;
		}
		/* Yield in case cooperative scheduling is being used. */
		#if configUSE_PREEMPTION == 0
		{
			taskYIELD();
		}
		#endif

        ulIdleLoops++;

        /* Place the processor into low power mode. */
        LPM3;
	}
}

/* The MSP430X port uses this callback function to configure its tick interrupt.
This allows the application to choose the tick interrupt source.
configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
interrupt vector for the chosen tick interrupt source.  This implementation of
vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
case configTICK_VECTOR is set to TIMER0_A0_VECTOR. */
void vApplicationSetupTimerInterrupt( void )
{
const unsigned short usACLK_Frequency_Hz = 32768;

	/* Ensure the timer is stopped. */
	TA0CTL = 0;

	/* Run the timer from the ACLK. */
	TA0CTL = TASSEL_1;

	/* Clear everything to start with. */
	TA0CTL |= TACLR;

	/* Set the compare match value according to the tick rate we want. */
	TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

	/* Enable the interrupts. */
	TA0CCTL0 = CCIE;

	/* Start up clean. */
	TA0CTL |= TACLR;

	/* Up mode. */
	TA0CTL |= MC_1;
}