/* Task 1: Use an existing RTOS project to write a program with two tasks.
 * A task to read a button state and a task to turn an LED ON/OFF.
 * The LED shall toggle its state between ON/OFF with every press on the button.
 * The action of toggling the LED shall take place after the user presses the button once and releases it.
 */

/* PIN0 in PORT0 set of INPUT in GPIO_cfg.c */

/*-----------------------------------------------------------*/



/* Tasks Handlers */
TaskHandle_t ButtonTaskHandler       = NULL;
TaskHandle_t LEDTaskHandler        = NULL;



/* One for the Button state for the Button task
 * and the other is for the Pressed time on the button 
 */
pinState_t ButtonState;
pinState_t LEDState;
pinState_t ToggleState;

/* The button task to be created, Checks the button each 100ms */
void Button_Task( void * pvParameters )
{
    for( ;; )
    {
	    ButtonState = GPIO_read(PORT_0, PIN0);
			
	    if (ButtonState == PIN_IS_HIGH)                       /* If the button is pressed let the toggle state to be TRUE */
	    {
		    
		    ToggleState = pdTRUE;
	    
	    }
	    
			vTaskDelay(100);
    }
}

/* The LED task to be created */
void LED_Task( void * pvParameters )
{
    for( ;; )
    {
	    if (ButtonState == PIN_IS_LOW)                          /* Take action after the button is released */
	    {
		    if (ToggleState == pdTRUE)                      /* If the Toggle State is TRUE it means that the button has been pressed */
		    {
			    LEDState = GPIO_read(PORT_0, PIN1);     /* Read the current state of the LED ON or OFF */
			    
			    GPIO_write(PORT_0, PIN1, !LEDState);    /* Toggle the LED State */
			    
			    ToggleState = pdFALSE;                  /* Resed the Toggle state */ 
		    }
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


