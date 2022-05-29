/* Task 3: Use an existing RTOS project to write a program with 4 tasks. 
 * Two tasks will detect rising and falling edges on two buttons.
 * Every edge is an event that will be sent to a consumer task to write on UART.
 * A task will send periodic string every 100ms to consumer task to write on UART.
 */

/* #include "queue.h"  in main.c 
 * PIN0 and PIN1 in PORT0 set of INPUT in GPIO_cfg.c
 * configSUPPORT_DYNAMIC_ALLOCATION set to 1 in FreeRTOSConfig.h
 * configUSE_PREEMPTION set to 0 in FreeRTOSConfig.h
 */

/*-----------------------------------------------------------*/

/* Tasks Handlers */
TaskHandle_t Button1TaskHandler       = NULL;
TaskHandle_t Button2TaskHandler       = NULL;
TaskHandle_t StringTaskHandler        = NULL;
TaskHandle_t PrintTaskHandler         = NULL;

/* Queue Handler */
QueueHandle_t UART_Queue              = NULL;



pinState_t Button_1_State;                          /* Contains the current Button 1 state */
pinState_t Old_1_State = PIN_IS_HIGH;               /* Contains the previous state of Button 1 */

pinState_t Button_2_State;                          /* Contains the current Button 2 state */
pinState_t Old_2_State = PIN_IS_HIGH;               /* Contains the previous state of Button 2 */


/* 5 Strings for each state */
const signed portCHAR String[8]          ="-------";
const signed portCHAR B1_RisingEdge[8]   ="B1 Rise";
const signed portCHAR B1_FallingEdge[8]  ="B1 Fall";
const signed portCHAR B2_RisingEdge[8]   ="B2 Rise";
const signed portCHAR B2_FallingEdge[8]  ="B2 Fall";

signed portCHAR Queue_Copy_Buffer[8];               /* The buffer that will have the copied string in the queue */



/* The button 1 task to be created, Checks the button each 100ms */
void Button_1_Task( void * pvParameters )
{
    for( ;; )
    {
	    Button_1_State = GPIO_read(PORT_0, PIN0);               /* Reads the button state */
	    
	    if (Button_1_State != Old_1_State)                      /* If the current state isn't like the previous it means that this is an edge */
	    {
		    if (Button_1_State == PIN_IS_HIGH)              /* If the current state is HIGH it is a Rising Edge */
		    {
			    xQueueSend(	
				     	UART_Queue,                 /* Queue Handler */
				    	(void *) &B1_RisingEdge,    /* The string that will be sent to the Queue */
				    	(TickType_t) 100 );         /* Ticks to wait */
		    }
		    
		    else                                            /* If the current state is LOW it is a Falling Edge */
		    {
			    xQueueSend(
				    	UART_Queue,
				    	(void *) &B1_FallingEdge,
				    	(TickType_t) 100 );
		    }
	    }
	    
	    Old_1_State = Button_1_State;                           /* let the current state the previous */
			
	    vTaskDelay(100);
    }
}

/* The button 2 task to be created, Identical to Button 1 Taks, Checks the button each 100ms */
void Button_2_Task( void * pvParameters )
{
    for( ;; )
    {
	    Button_2_State = GPIO_read(PORT_0, PIN1);               /* Reads the button state */
	    
	    if (Button_2_State != Old_2_State)                      /* If the current state isn't like the previous it means that this is an edge */
	    {
		    if (Button_2_State == PIN_IS_HIGH)              /* If the current state is HIGH it is a Rising Edge */
		    {
			    xQueueSend(	
				     	UART_Queue,                 /* Queue Handler */
				    	(void *) &B2_RisingEdge,    /* The string that will be sent to the Queue */
				    	(TickType_t) 100 );         /* Ticks to wait */
		    }
		    
		    else                                            /* If the current state is LOW it is a Falling Edge */
		    {
			    xQueueSend(
				    	UART_Queue,
				    	(void *) &B2_FallingEdge,
				    	(TickType_t) 100 );
		    }
	    }
	    
	    Old_2_State = Button_2_State;                           /* let the current state the previous */
			
	    vTaskDelay(100);
    }
}

/* A String task that sends a string to UART every 100ms */
void String_Task( void * pvParameters )
{
    for( ;; )
    {
	    xQueueSend(
		    	UART_Queue,
                    	(void *) &String,
                    	(TickType_t) 100 );
	    
	    vTaskDelay(100);
		
    }
}

/* Task that reads from the Queue */
void Print_Task( void *pvParameters )
{
	for( ;; )
	{
		if( UART_Queue != NULL )
		{
			if( xQueueReceive(UART_Queue,                          /* Receive a message from the queue to hold a string */
					  &( Queue_Copy_Buffer ),           /* Queue_Copy_Buffer will hold a copy of xMessage. */
					  ( TickType_t ) 0 ) == pdPASS )    /* Block for 10 ticks if a message is not immediately available. */
				{	
				
					vSerialPutString(Queue_Copy_Buffer, 8);
				
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

	/* Creat Queue */
	UART_Queue = xQueueCreate( 10, sizeof( String ) ); /* Creat a Queue that can hold 10 Strings */
	
        /* Create Tasks here */
	xTaskCreate(
			Button_1_Task,          /* Function that implements the task. */
			"Button 1 Task",        /* Text name for the task. */
			100,                    /* Stack size in words, not bytes. */
			( void * ) 0,           /* Parameter passed into the task. */
			1,                      /* Priority at which the task is created. */
			&Button1TaskHandler );  /* Used to pass out the created task's handle. */
								
	xTaskCreate(
			Button_2_Task,               /* Function that implements the task. */
			"Button 2 Task",             /* Text name for the task. */
			100,                         /* Stack size in words, not bytes. */
			( void * ) 0,                /* Parameter passed into the task. */
			1,                           /* Priority at which the task is created. */
			&Button2TaskHandler );       /* Used to pass out the created task's handle. */

	xTaskCreate(
			String_Task,               /* Function that implements the task. */
			"String Task",             /* Text name for the task. */
			100,                       /* Stack size in words, not bytes. */
			( void * ) 0,              /* Parameter passed into the task. */
			1,                         /* Priority at which the task is created. */
			&StringTaskHandler );      /* Used to pass out the created task's handle. */								

	xTaskCreate(
			Print_Task,               /* Function that implements the task. */
			"Print Task",             /* Text name for the task. */
			100,                      /* Stack size in words, not bytes. */
			( void * ) 0,             /* Parameter passed into the task. */
			1,                        /* Priority at which the task is created. */
			&PrintTaskHandler );      /* Used to pass out the created task's handle. */		

						


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


