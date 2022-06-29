/* Set EDF macro to 1 */
#define configUSE_EDF_SCHEDULER 1


/* Trace hooks */
#define traceTASK_SWITCHED_OUT()  	do\
																		{\
																			if ((int)pxCurrentTCB->pxTaskTag == 1 )\
																			{\
																				GPIO_write(PORT_0, PIN2, PIN_IS_LOW);\
																				task_1_out_time = T1TC;\
																				task_1_total_time += ( task_1_out_time - task_1_in_time);\
																			}\
																			else if ((int)pxCurrentTCB->pxTaskTag == 2)\
																			{\
																				GPIO_write(PORT_0, PIN3, PIN_IS_LOW);\
																				task_2_out_time = T1TC;\
																				task_2_total_time += ( task_2_out_time - task_2_in_time);\
																			}\
																		}while(0)
																		
#define traceTASK_SWITCHED_IN() 		do\
																		{\
																			if ((int)pxCurrentTCB->pxTaskTag == 1 )\
																			{\
																				GPIO_write(PORT_0, PIN2, PIN_IS_HIGH);\
																				task_1_in_time = T1TC;\
																			}\
																			else if ((int)pxCurrentTCB->pxTaskTag == 2)\
																			{\
																				GPIO_write(PORT_0, PIN3, PIN_IS_HIGH);\
																				task_2_in_time = T1TC;\
																			}\
																			system_time = T1TC;\
																			cpu_load = (( task_1_total_time + task_2_total_time) / (float)system_time) * 100;\
																		}while(0)
