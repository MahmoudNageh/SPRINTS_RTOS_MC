/* The tests main file */

/* Tasks Handlers */
TaskHandle_t Task_1_Handler = NULL;
TaskHandle_t Task_2_Handler = NULL;

/* Variables for the Run-Time Analysis */
int task_1_in_time = 0, task_1_out_time = 0, task_1_total_time = 0;
int task_2_in_time = 0, task_2_out_time = 0, task_2_total_time = 0;
int system_time = 0;
int cpu_load =0;

char runTimeStatsBuff[190];
/*-----------------------------------------------------------*/

/* Tasks Periods */
int Task1_P = 5;
int Task2_P = 8;

/* The task to be created */
void Task_1( void * pvParameters )
{
	int i; 
	
	int xLastWakeTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag( NULL, (void *) 1);          /* Task tag for RunTimeStats */
	
	for(;;)
	{
    /* A Heavy load that will take 3ms to execute */ 
		for(i=0; i<19750;i++)
		{
			i=i;
		}
		
    /* RunTimeStats for Run-Time Analysis */
		vTaskGetRunTimeStats( runTimeStatsBuff );
		
		xSerialPutChar('\n');
		
		vSerialPutString(runTimeStatsBuff, 190); 
				
		vTaskDelayUntil( &xLastWakeTime, Task1_P );
		
	}
}

void Task_2( void * pvParameters )
{
	int i; 
	
	int xLastWakeTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag( NULL, (void *) 2);      /* Task tag for RunTimeStats */
	
	for(;;)
	{
		for(i=0; i<19750;i++)                             /* The same load as Task one 3ms */
		{
			i=i;
		}
				
		vTaskDelayUntil( &xLastWakeTime, Task2_P );
		
	}
}

/* Implement Tick Hook */
void vApplicationTickHook( void )
{
	
	GPIO_write(PORT_0, PIN1, PIN_IS_HIGH);
	
	GPIO_write(PORT_0, PIN1, PIN_IS_LOW);
	
	
}
/*-----------------------------------------------------------*/
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	
    	/* Create Tasks here */

	xTaskPeriodicCreate(
			Task_1,                     /* Function that implements the task. */
			"Task 1",                   /* Text name for the task. */
			100,                        /* Stack size in words, not bytes. */
			( void * ) 0,               /* Parameter passed into the task. */
			1,                          /* Priority at which the task is created. */
			&Task_1_Handler,            /* Used to pass out the created task's handle. */
			Task1_P              );     /* Task Period */

			
	xTaskPeriodicCreate(
			Task_2,                     /* Function that implements the task. */
			"Task 2",                   /* Text name for the task. */
			100,                        /* Stack size in words, not bytes. */
			( void * ) 0,               /* Parameter passed into the task. */
			1,                          /* Priority at which the task is created. */
			&Task_2_Handler,            /* Used to pass out the created task's handle. */
			Task2_P              );     /* Task Period */

			
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
