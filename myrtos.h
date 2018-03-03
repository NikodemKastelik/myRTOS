/*
 * myrtos.h
 *
 * Created: 2018-02-25 10:27:12
 *  Author: Nikodem Kastelik
 */ 


#ifndef MYRTOS_H_
#define MYRTOS_H_

#include <inttypes.h>

typedef uint16_t myrtSysTimeMs_t;

typedef enum {READY, BLOCKED, SLEEPING, MUTEX, SEMAPHORE} myrtTaskStatus_t;

typedef union
{
	myrtSysTimeMs_t timeWhenUnblocked;
	void * mutex;
	void * semaphore;
}myrtBlockSource_t;

typedef struct  
{
	void * thisTaskStackPointer;
	myrtTaskStatus_t thisTaskStatus;
	myrtBlockSource_t blockSource; 
}
myrtTask;

void myrtSchedule();
void myrtCreateTask(void * taskFunction, uint8_t thisTaskStackSize);
void myrtSleep(uint16_t timeInMs);
 
void myrtStart(void (*idleTaskFunction)(void));
 
void myrtDebugSwitchTasksStates();

#endif /* MYRTOS_H_ */