/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "queue.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

/* Tasks Handlers */
TaskHandle_t Button1TaskHandler       = NULL;
TaskHandle_t Button2TaskHandler       = NULL;
TaskHandle_t StringTaskHandler        = NULL;
TaskHandle_t PrintTaskHandler         = NULL;
QueueHandle_t xQueue1                 = NULL;


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/



pinState_t Button_1_State;
pinState_t Old_1_State = PIN_IS_HIGH;

pinState_t Button_2_State;
pinState_t Old_2_State = PIN_IS_HIGH;


const signed portCHAR String[8]          ="-------";
const signed portCHAR B1_RisingEdge[8]   ="B1 Rise";
const signed portCHAR B1_FallingEdge[8]  ="B1 Fall";
const signed portCHAR B2_RisingEdge[8]   ="B2 Rise";
const signed portCHAR B2_FallingEdge[8]  ="B2 Fall";
signed portCHAR Queue_Buffer[8];



/* The button task to be created, Checks the button each 100ms */
void Button_1_Task( void * pvParameters )
{
    for( ;; )
    {
	    Button_1_State = GPIO_read(PORT_0, PIN0);
	    
			if (Button_1_State != Old_1_State)
			{
				
				if (Button_1_State == PIN_IS_HIGH)
				{
					xQueueSend(
										 xQueue1,
                    (void *) &B1_RisingEdge,
                    (TickType_t) 0
                   );
				}
				
				else 
				{
					xQueueSend(
										 xQueue1,
                    (void *) &B1_FallingEdge,
                    (TickType_t) 0
                   );
				}
				
			}
			
			Old_1_State = Button_1_State;
			
	    vTaskDelay(100);
    }
}


void Button_2_Task( void * pvParameters )
{
    for( ;; )
    {
	    Button_2_State = GPIO_read(PORT_0, PIN1);
	    
			if (Button_2_State != Old_2_State)
			{
				
				if (Button_2_State == PIN_IS_HIGH)
				{
					xQueueSend(
										 xQueue1,
                    (void *) &B2_RisingEdge,
                    (TickType_t) 0
                   );
				}
				
				else 
				{
					xQueueSend(
										 xQueue1,
                    (void *) &B2_FallingEdge,
                    (TickType_t) 0
                   );
				}
				
			}
			
			Old_2_State = Button_2_State;
			
	    vTaskDelay(100);
    }
}


void String_Task( void * pvParameters )
{
    for( ;; )
    {
			
			xQueueSend(
										xQueue1,
                    (void *) &String,
                    (TickType_t) 0
                   );
			
			vTaskDelay(100);
		
    }
}

/* Task that reads from the queues. */
void Print_Task( void *pvParameters )
{
	for( ;; )
    {
		 if( xQueue1 != NULL )
		 {
				/* Receive a message from the created queue to hold complex struct AMessage
				structure.  Block for 10 ticks if a message is not immediately available.
				The value is read into a struct AMessage variable, so after calling
				xQueueReceive() xRxedStructure will hold a copy of xMessage. */
				if( xQueueReceive( xQueue1,
													 &( Queue_Buffer ),
													 ( TickType_t ) 0 ) == pdPASS )
				{
					 /* A now contains a copy of xMessage. */
					
					vSerialPutString(Queue_Buffer, 8);
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
	xQueue1 = xQueueCreate( 10, sizeof( String ) );
	
	xTaskCreate(
								Button_1_Task,          /* Function that implements the task. */
                "Button 1 Task",        /* Text name for the task. */
                100,                  /* Stack size in words, not bytes. */
                ( void * ) 0,         /* Parameter passed into the task. */
                1,                    /* Priority at which the task is created. */
                &Button1TaskHandler ); /* Used to pass out the created task's handle. */
								

	xTaskCreate(
								Button_2_Task,               /* Function that implements the task. */
                "Button 2 Task",             /* Text name for the task. */
                100,                      /* Stack size in words, not bytes. */
                ( void * ) 0,             /* Parameter passed into the task. */
                1,                        /* Priority at which the task is created. */
                &Button2TaskHandler );      /* Used to pass out the created task's handle. */

	xTaskCreate(
								String_Task,               /* Function that implements the task. */
                "String Task",             /* Text name for the task. */
                100,                      /* Stack size in words, not bytes. */
                ( void * ) 0,             /* Parameter passed into the task. */
                1,                        /* Priority at which the task is created. */
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


