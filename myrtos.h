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

typedef uint16_t myrtSysTimeMs_t;

typedef enum {READY, SLEEPING, MUTEX} myrtTaskStatus_t;
	
//#define LOCKED 0
//#define RELEASED 1
//typedef volatile uint8_t mutex_t;
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

extern myrtTask * currentTask;

void myrtSchedule();
void myrtCreateTask(void * taskFunction, uint8_t thisTaskStackSize);
void myrtSleep(uint16_t timeInMs);
void myrtStart(void (*idleTaskFunction)(void));
void myrtDebugSwitchTasksStates();

mutex_t createMutex();
void lockMutex(mutex_t * mutexToLock);
mybool trylockMutex(mutex_t * mutexToLock);
void releaseMutex(mutex_t * mutexToUnlock);

#endif /* MYRTOS_H_ */