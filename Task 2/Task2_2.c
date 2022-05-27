/* Task 2: Use an existing RTOS project to write a program with 2 tasks.
 * Both tasks are responsible for writing on UART any string 10 times each
 * task cycle. The task cycles are, respectively, 100ms, and 500ms.
 * The task with cycle 500ms shall have an empty loop that loops 100,000 times
 * (To simulate heavy load) after every time a string is written on UART.
 * Each task writing to UART shall not be interrupted by the other task.
 */

/* #include "semph.h"  in main.c 
 * configSUPPORT_DYNAMIC_ALLOCATION set to 1 in FreeRTOSConfig.h
 * configUSE_MUTEXES set to 1 in FreeRTOSConfig.h
 * configUSE_PREEMPTION set to 1 in FreeRTOSConfig.h
 */

/*-----------------------------------------------------------*/


/* Tasks Handlers */
TaskHandle_t String1TaskHandler       = NULL;
TaskHandle_t String2TaskHandler       = NULL;

/*Mutex Handler */
SemaphoreHandle_t UART_Mutex          = NULL;



/* 2 Strings for each task */

// const signed portCHAR String_1[5] = {"HELLO"};
// const signed portCHAR String_2[5] = {"WORLD"};

/* Used Chars for easier fault detection in UART */
const signed portCHAR String_1 = 'A';
const signed portCHAR String_2 = 'B';

/* to variables for the for loops*/  
uint8_t i; 					/* i is a uint8 because it goes to only 10 */
uint32_t j; 					/* j count to 100,000 so it's a uint32 */


/* The String 1 task to be created, prints a string 10 times each 100ms */
void String_1_Task( void * pvParameters )
{
    for( ;; )
    {
	    if( UART_Mutex != NULL )
	    {
		    if( xSemaphoreTake( UART_Mutex, 0 ) == pdTRUE )     /* See if we can obtain the Mutex. */
		    {
           		/* We were able to obtain the Mutex and can now access the shared resource. */
			    
			    for (i= 0; i < 10; i++)
			    {
				    
				    xSerialPutChar (String_1);
			    
			    }
			    
			    xSemaphoreGive( UART_Mutex );  		 /* Release the Mutex. */
		    }
		    
	    }
	    
	    vTaskDelay(100);
    }
	
}

/* The String 2 task to be created, prints a string 10 times each 500ms with a heavy load */
void String_2_Task( void * pvParameters )
{
    for( ;; )
    {
	    if( UART_Mutex != NULL )
	    {
		    if( xSemaphoreTake( UART_Mutex, 0 ) == pdTRUE )    /* See if we can obtain the Mutex. */
		    {
			    /* We were able to obtain the Mutex and can now access the shared resource. */
			    
			    for (i= 0; i < 10; i++)
			    {
				    for (j = 0; j < 100000; j++)
				    { }
				    
				    xSerialPutChar (String_2);
				    
			    }
			    
			    xSemaphoreGive( UART_Mutex );	/* Release the Mutex. */
		    
		    }
	    
	    }
	    
	    vTaskDelay(500);
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

	/* Create a mutex type semaphore. */
    	UART_Mutex = xSemaphoreCreateMutex( );
	
    	/* Create Tasks here */

	xTaskCreate(
		String_1_Task,          /* Function that implements the task. */
                "String 1 Task",        /* Text name for the task. */
                100,                    /* Stack size in words, not bytes. */
                ( void * ) 0,           /* Parameter passed into the task. */
                2,                      /* Priority at which the task is created. */        
                &String1TaskHandler );  /* Used to pass out the created task's handle. */
								

	xTaskCreate(
		String_2_Task,               /* Function that implements the task. */
                "String 2 Task",             /* Text name for the task. */
                100,                         /* Stack size in words, not bytes. */
                ( void * ) 0,                /* Parameter passed into the task. */
                1,                           /* Priority at which the task is created. */
                &String2TaskHandler );      /* Used to pass out the created task's handle. */
								


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
