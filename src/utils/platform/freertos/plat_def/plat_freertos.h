/*
 * os.h
 *
 *  Created on: 2015Äê8ÔÂ31ÈÕ
 *      Author: niuyue
 */

#ifndef APPCODE_OS_OS_H_
#define APPCODE_OS_OS_H_

#if defined(OS_FREERTOS)
#include "platform/include/define.h"
#include "platform/include/LIST2.h"

#include "platform/freertos/include/FreeRTOS.h"
#include "platform/freertos/include/list_rtos.h"
#include "platform/freertos/include/projdefs.h"
#include "platform/freertos/include/portable.h"
#include "platform/freertos/include/portmacro.h"
#include "platform/freertos/include/projdefs.h"
#include "platform/freertos/include/croutine.h"
#include "platform/freertos/include/event_groups.h"
#include "platform/freertos/include/portable.h"
#include "platform/freertos/include/queue.h"
#include "platform/freertos/include/semphr.h"
#include "platform/freertos/include/task.h"
#include "platform/freertos/include/timers.h"




/* Value that can be assigned to the eNotifyState member of the TCB. */
typedef enum
{
 eNotWaitingNotification = 0,
 eWaitingNotification,
 eNotified
} eNotifyValue;


typedef struct tskTaskControlBlock
{
 volatile StackType_t *pxTopOfStack; /*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */


 #if ( portUSING_MPU_WRAPPERS == 1 )
  xMPU_SETTINGS xMPUSettings;  /*< The MPU settings are defined as part of the port layer.  THIS MUST BE THE SECOND MEMBER OF THE TCB STRUCT. */
  BaseType_t  xUsingStaticallyAllocatedStack; /* Set to pdTRUE if the stack is a statically allocated array, and pdFALSE if the stack is dynamically allocated. */
 #endif


 ListItem_t   xGenericListItem; /*< The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
 ListItem_t   xEventListItem;  /*< Used to reference a task from an event list. */
 UBaseType_t   uxPriority;   /*< The priority of the task.  0 is the lowest priority. */
 StackType_t   *pxStack;   /*< Points to the start of the stack. */
 char    pcTaskName[ configMAX_TASK_NAME_LEN ];/*< Descriptive name given to the task when created.  Facilitates debugging only. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */


 #if ( portSTACK_GROWTH > 0 )
  StackType_t  *pxEndOfStack;  /*< Points to the end of the stack on architectures where the stack grows up from low memory. */
 #endif


 #if ( portCRITICAL_NESTING_IN_TCB == 1 )
  UBaseType_t  uxCriticalNesting;  /*< Holds the critical section nesting depth for ports that do not maintain their own count in the port layer. */
 #endif


 #if ( configUSE_TRACE_FACILITY == 1 )
  UBaseType_t  uxTCBNumber;  /*< Stores a number that increments each time a TCB is created.  It allows debuggers to determine when a task has been deleted and then recreated. */
  UBaseType_t   uxTaskNumber;  /*< Stores a number specifically for use by third party trace code. */
 #endif


 #if ( configUSE_MUTEXES == 1 )
  UBaseType_t  uxBasePriority;  /*< The priority last assigned to the task - used by the priority inheritance mechanism. */
  UBaseType_t  uxMutexesHeld;
 #endif


 #if ( configUSE_APPLICATION_TASK_TAG == 1 )
  TaskHookFunction_t pxTaskTag;
 #endif


 #if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
  void *pvThreadLocalStoragePointers[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
 #endif


 #if ( configGENERATE_RUN_TIME_STATS == 1 )
  uint32_t  ulRunTimeCounter; /*< Stores the amount of time the task has spent in the Running state. */
 #endif


 #if ( configUSE_NEWLIB_REENTRANT == 1 )
  /* Allocate a Newlib reent structure that is specific to this task.
  Note Newlib support has been included by popular demand, but is not
  used by the FreeRTOS maintainers themselves.  FreeRTOS is not
  responsible for resulting newlib operation.  User must be familiar with
  newlib and must provide system-wide implementations of the necessary
  stubs. Be warned that (at the time of writing) the current newlib design
  implements a system-wide malloc() that must be provided with locks. */
  struct  _reent xNewLib_reent;
 #endif


 #if ( configUSE_TASK_NOTIFICATIONS == 1 )
  volatile uint32_t ulNotifiedValue;
  volatile eNotifyValue eNotifyState;
 #endif


} tskTCB;


/* The old tskTCB name is maintained above then typedefed to the new TCB_t name
below to enable the use of older kernel aware debuggers. */
typedef tskTCB TCB_t;


/*
 * Definition of the queue used by the scheduler.
 * Items are queued by copy, not reference.  See the following link for the
 * rationale: http://www.freertos.org/Embedded-RTOS-Queues.html
 */
typedef struct QueueDefinition
{
 int8_t *pcHead;     /*< Points to the beginning of the queue storage area. */
 int8_t *pcTail;     /*< Points to the byte at the end of the queue storage area.  Once more byte is allocated than necessary to store the queue items, this is used as a marker. */
 int8_t *pcWriteTo;    /*< Points to the free next place in the storage area. */


 union       /* Use of a union is an exception to the coding standard to ensure two mutually exclusive structure members don't appear simultaneously (wasting RAM). */
 {
  int8_t *pcReadFrom;   /*< Points to the last place that a queued item was read from when the structure is used as a queue. */
  UBaseType_t uxRecursiveCallCount;/*< Maintains a count of the number of times a recursive mutex has been recursively 'taken' when the structure is used as a mutex. */
 } u;


 List_t xTasksWaitingToSend;  /*< List of tasks that are blocked waiting to post onto this queue.  Stored in priority order. */
 List_t xTasksWaitingToReceive; /*< List of tasks that are blocked waiting to read from this queue.  Stored in priority order. */


 volatile UBaseType_t uxMessagesWaiting;/*< The number of items currently in the queue. */
 UBaseType_t uxLength;   /*< The length of the queue defined as the number of items it will hold, not the number of bytes. */
 UBaseType_t uxItemSize;   /*< The size of each items that the queue will hold. */


 volatile BaseType_t xRxLock; /*< Stores the number of items received from the queue (removed from the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
 volatile BaseType_t xTxLock; /*< Stores the number of items transmitted to the queue (added to the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */


 #if ( configUSE_TRACE_FACILITY == 1 )
  UBaseType_t uxQueueNumber;
  uint8_t ucQueueType;
  char	pcQueueName[ configMAX_TASK_NAME_LEN ];/*< Descriptive name given to the queue when created.  Facilitates debugging only. */
 #endif


 #if ( configUSE_QUEUE_SETS == 1 )
  struct QueueDefinition *pxQueueSetContainer;
 #endif


} xQUEUE;


/* The old xQUEUE name is maintained above then typedefed to the new Queue_t
name below to enable the use of older kernel aware debuggers. */
typedef xQUEUE Queue_t;


typedef struct xEventGroupDefinition
{
 EventBits_t uxEventBits;
 List_t xTasksWaitingForBits;  /*< List of tasks waiting for a bit to be set. */


 #if( configUSE_TRACE_FACILITY == 1 )
  UBaseType_t uxEventGroupNumber;
  char	pcEventName[ configMAX_TASK_NAME_LEN ];/*< Descriptive name given to the event when created.  Facilitates debugging only. */
 #endif


} EventGroup_t;


/* The definition of the timers themselves. */
typedef struct tmrTimerControl
{
 const char    *pcTimerName;  /*<< Text name.  This is not used by the kernel, it is included simply to make debugging easier. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
 ListItem_t    xTimerListItem;  /*<< Standard linked list item as used by all kernel features for event management. */
 TickType_t    xTimerPeriodInTicks;/*<< How quickly and often the timer expires. */
 UBaseType_t    uxAutoReload;  /*<< Set to pdTRUE if the timer should be automatically restarted once expired.  Set to pdFALSE if the timer is, in effect, a one-shot timer. */
 void      *pvTimerID;   /*<< An ID to identify the timer.  This allows the timer to be identified when the same callback is used for multiple timers. */
 TimerCallbackFunction_t pxCallbackFunction; /*<< The function that will be called when the timer expires. */
 #if( configUSE_TRACE_FACILITY == 1 )
  UBaseType_t   uxTimerNumber;  /*<< An ID assigned by trace tools such as FreeRTOS+Trace */
 #endif
} xTIMER;


/* The old xTIMER name is maintained above then typedefed to the new Timer_t
name below to enable the use of older kernel aware debuggers. */
typedef xTIMER Timer_t;



typedef	TCB_t				OS_TASK;
typedef	Queue_t				OS_SEMAPHORE;
typedef EventGroup_t		OS_EVENTGROUP;
typedef Queue_t				OS_QUEUE;
typedef portBASE_TYPE		OS_STATUS;
typedef uint32_t			OS_HISR;
typedef Timer_t				OS_TIMER;


struct _INTR_CTRL_BLOCK_ITEM
{
	void (* lisr)(uint32_t);
	void (* hisr)();
	char name[8];
};
typedef struct _INTR_CTRL_BLOCK_ITEM	ICB;

#endif

#endif /* APPCODE_OS_OS_H_ */
