/* Task 1: Use an existing RTOS project to write a program with two tasks.
 * A task to read a button state and a task to turn an LED ON/OFF.
 * The LED shall toggle its state between ON/OFF with every press on the button.
 * The action of toggling the LED shall take place after the user presses the button once and releases it.
 */

/* include "semphr.h" in main.c
 * PIN0 in PORT0 set of INPUT in GPIO_cfg.c
 * configSUPPORT_DYNAMIC_ALLOCATION set to 1 in FreeRTOSConfig.h
 * configUSE_PREEMPTION set to 0 in FreeRTOSConfig.h
 */


/*-----------------------------------------------------------*/


/* Tasks Handlers */
TaskHandle_t ButtonTaskHandler       = NULL;
TaskHandle_t LEDTaskHandler       = NULL;

/* Semaphore Handler */
SemaphoreHandle_t LedSem;


pinState_t ButtonState;				/* for the button state pressed or not */ 
pinState_t LEDState;				/* for the current LED state ON or OFF */
pinState_t ToggleState;				/* for the toggling, should take action or not */

/* The button task to be created, Checks the button each 100ms */
void Button_Task( void * pvParameters )
{
    for( ;; )
    {
	    ButtonState = GPIO_read(PORT_0, PIN0);
	    
	    xSemaphoreTake( LedSem , ( TickType_t ) 0 );                                  /* Obtain the Semaphore  */
			
	    if (ButtonState == PIN_IS_HIGH)                                               /* If the button is pressed let the toggle state to be TRUE */
	    {
		    
		    ToggleState = pdTRUE;
	    
	    }
	    
	    else if (  ButtonState == PIN_IS_LOW && ToggleState == pdTRUE )               /* Take action after the button is released AND the toggle state should be TRUE*/
	    {
		    
		    xSemaphoreGive( LedSem );                                             /* Give the Semaphore to the LED task to do the toggling */
		    
		    ToggleState = pdFALSE;                                                /* Reset the toggle state */
	    }
	    
	    vTaskDelay(100);
    }
	
}

/* The LED task to be created */
void LED_Task( void * pvParameters )
{
    for( ;; )
    {
	    if (xSemaphoreTake( BinSem, ( TickType_t ) 0 ) == pdTRUE)                       /* See if we can obtain the Semaphore */
	    {
		    LEDState = GPIO_read(PORT_0, PIN1);                                     /* Read the current state of the LED ON or OFF */
		    
		    GPIO_write(PORT_0, PIN1, !LEDState);                                    /* Toggle the LED State */
		    
		    xSemaphoreGive( LedSem );                                               /* Release the Semaphore */
		    
	    }
	    
	    vTaskDelay(100);
    }
	
}


/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	
	/* Create a binary type semaphore. */
	LedSem = xSemaphoreCreateBinary();
		
		
    	/* Create Tasks here */

	xTaskCreate(
		Button_Task,          /* Function that implements the task. */
                "Button Task",        /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                ( void * ) 0,         /* Parameter passed into the task. */
                1,                    /* Priority at which the task is created. */
                &ButtonTaskHandler ); /* Used to pass out the created task's handle. */
								
								
	xTaskCreate(
		LED_Task,          /* Function that implements the task. */
                "LED Task",        /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                ( void * ) 0,         /* Parameter passed into the task. */
                1,                    /* Priority at which the task is created. */
                &LEDTaskHandler ); /* Used to pass out the created task's handle. */

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


