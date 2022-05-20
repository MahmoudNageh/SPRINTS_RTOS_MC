/* Task 3: Use an existing RTOS project to write a program where a user shall be
 able to control LED toggling rate by pressing a push button, And the action is
 taken after releasing the button. Press longer than two seconds and less than
 4 seconds will toggle the LED with periodicity 400ms. Press longer than 4 seconds
 will toggle the LED with periodicity 100ms. Press shorter than 2 seconds will turn the LED OFF. */

/* Tasks Handlers */
TaskHandle_t ButtonTaskHandler       = NULL;
TaskHandle_t LedTaskHandler          = NULL;
TaskHandle_t PressTimeTaskHandler    = NULL;


static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/* enum for the Pressed time values
   this should be in a confg file */

typedef enum
{
	STATE_OFF,
	STATE_400,
	STATE_100
	
}PressState_t;

/* One will contain the Button state for the button task
  and the other will have info about the Pressed time on the button */
pinState_t ButtonState;
PressState_t Press_Time_State;

/* The button task to be created, Checks the button each 100ms */
void Button_Task( void * pvParameters )
{
    for( ;; )
    {
	    ButtonState = GPIO_read(PORT_0, PIN0);
	    
	    vTaskDelay(100);
    }
}

/* The Press time task to be created */
void Press_Time_Task( void * pvParameters )
{
    for( ;; )
    {
	    if (ButtonState == PIN_IS_HIGH)                        /* If the button is pressed delay for 2s */
	    {
		    Press_Time_State = STATE_OFF;                  /* Set the Press time state to be 0 becasue it's in the 0-2s margin */
		    
		    vTaskDelay(2000);
		    
		    if (ButtonState == PIN_IS_HIGH)                /* If the button is still pressed delay for another 2s */
		    {
			    Press_Time_State = STATE_400;          /* Set the Press time state to be ( 400ms ) becasue it's in the 2-4s margin */
			   
			    vTaskDelay(2000);
			    
			    if (ButtonState == PIN_IS_HIGH)        /* If the button is still pressed delay for another 2s just to confirm*/
			    {
				    Press_Time_State = STATE_100;  /* Set the Press time state to be ( 100ms ) becasue it's in the 4+ s margin */
				   
				    vTaskDelay(2000);
			    }
		    }
	    }
	    vTaskDelay(100);
    }
}

/* The LED task to be created, a simple check for the state and the LED toggles corresponding to the state */
void LED_Task( void * pvParameters )
{
    for( ;; )
    {
	    if (ButtonState == PIN_IS_LOW)      /* The task works when the button is released */
	    {
		    if (Press_Time_State == STATE_100)
		    {
			    GPIO_write(PORT_0, PIN1, PIN_IS_HIGH); /* Set Pin 0 to High */
			    
			    vTaskDelay(100);                       /* Wait for 100ms = 0.1s */
			    
			    GPIO_write(PORT_0, PIN1, PIN_IS_LOW);  /* Set Pin 0 to Low */
			    
			    vTaskDelay(100);                       /* Wait for 100ms = 0.1s */
		    }
		    
		    else if (Press_Time_State == STATE_400)
		    {
			    GPIO_write(PORT_0, PIN1, PIN_IS_HIGH); /* Set Pin 0 to High */
			    
			    vTaskDelay(400);                       /* Wait for 400ms = 0.4s */
			    
			    GPIO_write(PORT_0, PIN1, PIN_IS_LOW);  /* Set Pin 0 to Low */
			    
			    vTaskDelay(400);                       /* Wait for 400ms = 0.4s */
		    }
		    
		    else if (Press_Time_State == STATE_OFF)
		    {
			    GPIO_write(PORT_0, PIN1, PIN_IS_LOW);  /* Set Pin 0 to Low */
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
			Press_Time_Task,          /* Function that implements the task. */
			"Press Time Task",        /* Text name for the task. */
			100,                      /* Stack size in words, not bytes. */
			( void * ) 0,             /* Parameter passed into the task. */
			1,                        /* Priority at which the task is created. */
			&PressTimeTaskHandler );  /* Used to pass out the created task's handle. */

								
	xTaskCreate(
			LED_Task,             /* Function that implements the task. */
			"LED Task",           /* Text name for the task. */
			100,                  /* Stack size in words, not bytes. */
			( void * ) 0,         /* Parameter passed into the task. */
			1,                    /* Priority at which the task is created. */
			&LedTaskHandler );    /* Used to pass out the created task's handle. */

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
