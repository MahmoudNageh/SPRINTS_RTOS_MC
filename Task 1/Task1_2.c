/* Task 2: Use an existing RTOS project to write a program that
 * toggles 3 LEDs with 3 different tasks at the following rates
 * respectively, 100ms, 500ms, and 1000ms. 
 */


/* The Task Handler */
TaskHandle_t LED1Handler = NULL;
TaskHandle_t LED2Handler = NULL;
TaskHandle_t LED3Handler = NULL;


static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/* The task to be created */
void LED_1_Task( void * pvParameters )
{
    for( ;; )
    {
	    GPIO_write(PORT_0, PIN0, PIN_IS_HIGH); /* Set Pin 0 to High */
	    
	    vTaskDelay(100);                       /* Wait for 100ms = 0.1s */
	    
	    GPIO_write(PORT_0, PIN0, PIN_IS_LOW);  /* Set Pin 0 to Low */
	    
	    vTaskDelay(100);                       /* Wait for 100ms = 0.1s */
    }
}

void LED_2_Task( void * pvParameters )
{
    for( ;; )
    {
	    GPIO_write(PORT_0, PIN1, PIN_IS_HIGH); /* Set Pin 1 to High */
	    
	    vTaskDelay(400);                       /* Wait for 400ms = 0.4s */
	    
	    GPIO_write(PORT_0, PIN1, PIN_IS_LOW);  /* Set Pin 1 to Low */
	    
	    vTaskDelay(400);                       /* Wait for 400ms = 0.4s */
    }
}

void LED_3_Task( void * pvParameters )
{
    for( ;; )
    {
	    GPIO_write(PORT_0, PIN2, PIN_IS_HIGH); /* Set Pin 2 to High */
	    
	    vTaskDelay(1000);                       /* Wait for 1000ms = 1s */
	    
	    GPIO_write(PORT_0, PIN2, PIN_IS_LOW);  /* Set Pin 2 to Low */
	    
	    vTaskDelay(1000);                       /* Wait for 1000ms = 1s */
    }
}


int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

    /* Create Tasks here, Storing the handle */

	xTaskCreate(
			LED_1_Task,           /* Function that implements the task. */
			"LED 1 Task",         /* Text name for the task. */
			100,                  /* Stack size in words, not bytes. */
			( void * ) 0,         /* Parameter passed into the task. */
			1,                    /* Priority at which the task is created. */
			&LED1Handelr );       /* Used to pass out the created task's handle. */
	
		xTaskCreate(
			LED_2_Task,           /* Function that implements the task. */
			"LED 2 Task",         /* Text name for the task. */
			100,                  /* Stack size in words, not bytes. */
			( void * ) 0,         /* Parameter passed into the task. */
			1,                    /* Priority at which the task is created. */
			&LED2Handler );       /* Used to pass out the created task's handle. */
	
		xTaskCreate(
			LED_3_Task,           /* Function that implements the task. */
			"LED 3 Task",         /* Text name for the task. */
			100,                  /* Stack size in words, not bytes. */
			( void * ) 0,         /* Parameter passed into the task. */
			1,                    /* Priority at which the task is created. */
			&LED3Handler );       /* Used to pass out the created task's handle. */

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
