/*
 * os.h
 *
 *  Created on: 2015Äê9ÔÂ1ÈÕ
 *      Author: niuyue
 */

#ifndef PLATFORM_RTOS_H_
#define PLATFORM_RTOS_H_

#define OS_FREERTOS 	1

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "platform_def.h"
#define SYS_TICK_RATE_HZ		(configTICK_RATE_HZ)
#define MS_TO_TICK_COUNT(X)		((unsigned long)X*SYS_TICK_RATE_HZ/1000)
#define TICK_COUNT_TO_MS(X)		((unsigned long)X*1000/SYS_TICK_RATE_HZ)

#if defined(OS_NUCLEUS)
#include "nucleus.h"                        /* RTOS API defines */
#endif

#if defined(OS_NUCLEUS)

/*suspend time*/
#define	OS_SUSPEND_NO_TIMEOUT	(NU_SUSPEND)
#define OS_NO_SUSPEND			(NU_NO_SUSPEND)

/*option*/
#define OS_OPTION_CONSUME		BIT(0)
#define OS_OPTION_AND			BIT(1)
#define OS_OPTION_OR			0

/*suspend type*/
#define	OS_FIFO					NU_FIFO
#define OS_PRIORITY				NU_PRIORITY

/*queue type*/
#define OS_QUEUE_TYPE_FIXED		NU_FIXED_SIZE
#define OS_TIMEOUT				NU_TIMEOUT

/*err code, others in errno.h*/
#define OS_SUCCESS				NU_SUCCESS
#elif defined(OS_FREERTOS)
/*suspend time*/
#define	OS_SUSPEND_NO_TIMEOUT	(~0)
#define OS_NO_SUSPEND			0

/*option*/
#define OS_OPTION_CONSUME		BIT(0)
#define OS_OPTION_AND			BIT(1)
#define OS_OPTION_OR			0

#define	OS_FIFO					6
#define	OS_FIXED_SIZE			7
#define OS_PRIORITY				11
#define OS_QUEUE_TYPE_FIXED		0
#define OS_TIMEOUT				-50
#define OS_SUCCESS				0
#endif

struct _os_task_arg
{
	uint32_t argc;
	void *argv;
	uint8_t preempt;
	uint8_t auto_start;
    uint8_t r[2];
    OS_TASK* task;
};
typedef struct _os_task_arg OS_ARG_TASK;

struct _os_queue_arg
{
	uint8_t message_type;
	uint8_t suspend_type;
    uint8_t r[2];
    uint32_t message_size;
    OS_QUEUE *queue;
};
typedef struct _os_queue_arg OS_ARG_QUEUE;

struct _os_events_arg
{
	uint8_t operation;
	uint8_t waitforall;
	uint8_t r[2];
	uint32_t suspend;
	OS_EVENTGROUP *group;
};
typedef struct _os_events_arg OS_ARG_EVENTS;


uint32_t OS_Retrieve_Clock(void);
void OS_Sleep(uint32_t msDelay);
OS_TASK* OS_Current_Task_Pointer(void);
OS_STATUS OS_Change_Currrnt_Task_Priority(uint8_t prio);
OS_STATUS OS_Terminate_Task(OS_TASK* task, void* args);
OS_STATUS OS_Create_Task(OS_TASK** task, char* name, void (*task_entry)(void),
						 uint32_t stack_size,
						 uint8_t priority, uint32_t time_slice,
						 void* args);
OS_STATUS OS_Delete_Task(OS_TASK *task, void* args);
OS_STATUS OS_Reset_Task(OS_TASK *task, void* args);
OS_STATUS OS_Terminal_Task(OS_TASK *task, void* args);
OS_STATUS OS_Resume_Task(OS_TASK *task, void* args);
OS_STATUS OS_Create_Queue(OS_QUEUE** queue, char *name,
                      uint32_t queue_size,
					  uint32_t message_size, void* args);
OS_STATUS OS_Delete_Queue(OS_QUEUE *queue, void* args);
OS_STATUS OS_Reset_Queue(OS_QUEUE *queue, void* args);
OS_STATUS OS_Send_To_Queue(OS_QUEUE *queue, void *message,
                           uint32_t size, uint32_t suspend, void* args);
OS_STATUS OS_Receive_From_Queue(OS_QUEUE *queue, void *message,
                                uint32_t size, uint32_t* actual_size,
								uint32_t suspend, void* args);
OS_STATUS OS_Create_Semaphore(OS_SEMAPHORE** sem, char *name,
                        uint32_t initial_count, uint8_t suspend_type, void* args);
OS_STATUS OS_Delete_Semaphore(OS_SEMAPHORE *sem);
OS_STATUS OS_Reset_Semaphore(OS_SEMAPHORE *sem, uint32_t initial_count, void* args);
OS_STATUS OS_Obtain_Semaphore(OS_SEMAPHORE* sem, uint32_t flags, void* args);
OS_STATUS OS_Release_Semaphore(OS_SEMAPHORE* sem, void* args);
OS_STATUS OS_Create_Event_Group(OS_EVENTGROUP** group, char *name, void* args);
OS_STATUS OS_Delete_Event_Group(OS_EVENTGROUP* group, void* args);
OS_STATUS OS_Set_Events(OS_EVENTGROUP *group, uint32_t events, void* args);
OS_STATUS OS_Retrieve_Events(OS_EVENTGROUP *group, uint32_t events, uint32_t* rd_event, void* args);

OS_STATUS OS_Register_LISR(uint32_t vector, void (*new_lisr)(uint32_t), void (**old_lisr)(uint32_t));
OS_STATUS OS_Create_HISR(OS_HISR *hisr, char *name,
                        void (*hisr_entry)(void), uint8_t priority,
						void *stack_address, uint32_t stack_size, uint8_t int_id);
OS_STATUS OS_Activate_HISR(OS_HISR *hisr);
void OS_Init(void* args);
void OS_Entry(void);

#endif /* PLATFORM_RTOS_H_ */
