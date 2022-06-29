/* THE CHANGES AND UPDATES DONE IN TASK.C*/



/* New Ready Task declaration */
#if ( configUSE_EDF_SCHEDULER == 1 )

		PRIVILEGED_DATA static List_t xReadyTasksListEDF;        /*< Ready tasks orderedby their deadline. */

#endif

/*-----------------------------------------------*/

static void prvInitialiseTaskLists( void )
{
  ....
  
    #if ( configUSE_EDF_SCHEDULER == 1 )
				{
    
						vListInitialise( &xReadyTasksListEDF );     /* Initializing the new Ready List */
    
				}
		#endif /* configUSE_EDF_SCHEDULER */
  
  
/*-----------------------------------------------*/
  
/* Adding tasks to the Ready List according to their deadlines*/
 
#if (configUSE_EDF_SCHEDULER == 0)
		#define prvAddTaskToReadyList( pxTCB )                                                                 \
				traceMOVED_TASK_TO_READY_STATE( pxTCB );                                                           \
				taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority );                                                \
				listINSERT_END( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ), &( ( pxTCB )->xStateListItem ) ); \
				tracePOST_MOVED_TASK_TO_READY_STATE( pxTCB )
#else 
		#define prvAddTaskToReadyList( pxTCB ) /*xStateListItem must contain thedeadline value */              \
				vListInsert( &(xReadyTasksListEDF), &( ( pxTCB )->xStateListItem ) )
#endif
      
/*-----------------------------------------------*/

/* A new period variable is added in the tskTaskControlBlock */      
typedef struct tskTaskControlBlock          /* Task Control Block Structure */
{
  ....
		/* The period of a task */
		#if ( configUSE_EDF_SCHEDULER == 1 )
					TickType_t xTaskPeriod;           /*< Stores the period in tick of the task. > */
		#endif
} tskTCB;
  
/*-----------------------------------------------*/

/* New Task Method */
  
#if ( configUSE_EDF_SCHEDULER == 0 )
				
						BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
																		const char * const pcName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
																		const configSTACK_DEPTH_TYPE usStackDepth,
																		void * const pvParameters,
																		UBaseType_t uxPriority,
																		TaskHandle_t * const pxCreatedTask )
																	
#else /* onfigUSE_EDF_SCHEDULER ==1 */
					
						BaseType_t xTaskPeriodicCreate( TaskFunction_t pxTaskCode,
																						const char * const pcName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
																						const configSTACK_DEPTH_TYPE usStackDepth,
																						void * const pvParameters,
																						UBaseType_t uxPriority,
																						TaskHandle_t * const pxCreatedTask,
																						TickType_t period )
             
/*-----------------------------------------------*/
             
/* Creating the idle task with the new task method */
              
void vTaskStartScheduler( void )
{
  ....
      
						#if (configUSE_EDF_SCHEDULER == 1)
								{
								TickType_t initIDLEPeriod = 100;
								xReturn = xTaskPeriodicCreate( prvIdleTask,
																							 configIDLE_TASK_NAME,
																							 configMINIMAL_STACK_SIZE,
																							 (void * ) NULL,
																							 ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ),
																							 &xIdleTaskHandle,
																							 initIDLEPeriod );
								}
  ....
}
  
/*-----------------------------------------------*/
  
/* Updating Switch Context */

void vTaskSwitchContext( void )
{
  ....
        /* Select a new task to run accorging to the priority */
				#if (configUSE_EDF_SCHEDULER == 0)
						{
								taskSELECT_HIGHEST_PRIORITY_TASK(); /*lint !e9079 void * is used as this macro is used with timers and co-routines too.  Alignment is known to be fine as the type of the pointer stored and retrieved is the same. */
						}
        /* Select a new task to run accorging to the earliest deadline */
				#else /* configUSE_EDF_SCHEDULER == 1 */
						{
							pxCurrentTCB = (TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( &(xReadyTasksListEDF ) );
						}
					#endif
}

/*-----------------------------------------------*/
  
/* Updating the new task deadline.
 * Updating the IDLE task deadline.
 * Requesting switch context after a task has been added to the ready list */
  
  BaseType_t xTaskIncrementTick( void )
{
  ....
    /* Place the unblocked task into the appropriate ready
     * list. */
										
    #if ( configUSE_EDF_SCHEDULER == 1 )
    
        /* Calculating the new task deadline */
        listSET_LIST_ITEM_VALUE( &( ( pxTCB )->xStateListItem ), ( pxTCB)->xTaskPeriod + xTickCount);
    
        /* Updating the IDLE task deadline */
        listSET_LIST_ITEM_VALUE( &( ( xIdleTaskHandle )->xStateListItem ), ( xIdleTaskHandle)->xTaskPeriod + xTickCount);
    
    #endif
    
        /* Adding the task to the ready list*/
        prvAddTaskToReadyList( pxTCB );
    
    #if ( configUSE_EDF_SCHEDULER == 1 )
    
        /*Whenever a task is added to the ready list the Schedualr should run and context switching should occur */
        xSwitchRequired = pdTRUE;
    
    #endif
  
  ....
}

/*-----------------------------------------------*/

