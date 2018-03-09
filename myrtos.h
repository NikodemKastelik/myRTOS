/*
 * myrtos.h
 *
 * Created: 2018-02-25 10:27:12
 *  Author: Nikodem Kastelik
 */ 


#ifndef MYRTOS_H_
#define MYRTOS_H_
#include <inttypes.h>
#include "mybool.h"

#define FIFO_SIZE	128

typedef uint16_t myrtSysTimeMs_t;
typedef enum {READY, SLEEPING, MUTEX, QUEUE} myrtTaskStatus_t;
	
typedef enum {LOCKED, RELEASED} mutex_t;

typedef union
{
	myrtSysTimeMs_t timeWhenUnblocked;
	mutex_t * mutex;
}myrtBlockSource_t;

typedef struct  
{
	void * thisTaskStackPointer;
	myrtTaskStatus_t thisTaskStatus;
	myrtBlockSource_t blockSource; 
}
myrtTask;

typedef struct
{
	char fifo[FIFO_SIZE];
	uint16_t writeIndex;
	uint16_t readIndex;
	mutex_t readWriteMutex;
} myQueue;

extern myrtTask * currentTask;

void myrtSchedule();
void myrtCreateTask(void * taskFunction, uint8_t thisTaskStackSize);
void myrtSleep(uint16_t timeInMs);
void myrtStart(void (*idleTaskFunction)(void));

mutex_t createMutex();
mybool lockMutex(mutex_t * mutexToLock, mybool isBlocking);
void releaseMutex(mutex_t * mutexToUnlock);

void queuePut(myQueue * Queue, const char * arr);
mybool queueGet(myQueue * Queue, char * arr);
void queueInit(myQueue * Queue);
mybool queueDoesContainItem(myQueue * Queue);

#endif /* MYRTOS_H_ */