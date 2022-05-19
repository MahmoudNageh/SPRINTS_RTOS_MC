/* Task 1: Use an existing RTOS project to write a program with one task to toggle an LED every 1000ms. */


/* The Task Handler */
TaskHandle_t ToggleTaskHandler = NULL;


static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/* The task to be created */
void Toggle_Task( void * pvParameters )
{
    for( ;; )
    {
	    GPIO_write(PORT_0, PIN0, PIN_IS_HIGH); /* Set Pin 0 to High */
	    
	    vTaskDelay(1000);                      /* Wait for 1000ms = 1s */
	    
	    GPIO_write(PORT_0, PIN0, PIN_IS_LOW); /* Set Pin 0 to Low */
	    
	    vTaskDelay(1000);                     /* Wait for 1000ms = 1s */
    }
}


int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

    /* Create Tasks here, Storing the handle */

	xTaskCreate(
			Toggle_Task,          /* Function that implements the task. */
			"Toggle Task",        /* Text name for the task. */
			100,                  /* Stack size in words, not bytes. */
			( void * ) 0,         /* Parameter passed into the task. */
			1,                    /* Priority at which the task is created. */
			&ToggleTaskHandler ); /* Used to pass out the created task's handle. */

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
