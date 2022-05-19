TaskHandle_t ToggleTaskHandler = NULL;

/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/


void Toggle_Task( void * pvParameters )
{
    for( ;; )
    {
			GPIO_write(PORT_0, PIN0, PIN_IS_HIGH);
			
			vTaskDelay(1000);
				
			GPIO_write(PORT_0, PIN0, PIN_IS_LOW);
			
			vTaskDelay(1000);
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
