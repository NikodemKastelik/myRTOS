/*
 * mymutex.c
 *
 * Created: 2018-03-04 23:05:32
 *  Author: Nikodem Kastelik
 */ 

#include "myrtos.h"
#include "myrtos_port.h"

mutex_t createMutex()
{
	mutex_t mutex = RELEASED;
	return mutex;
}

void lockMutex(mutex_t * mutexToLock)
{
	DISABLE_INTERRUPTS();
	if(*mutexToLock == RELEASED)
	{
		*mutexToLock = LOCKED;
	}
	else
	{
		currentTask->thisTaskStatus = MUTEX;
		currentTask->blockSource.mutex = mutexToLock;
		myrtSchedule();
		// * lock mutex when it is available
		*mutexToLock = LOCKED; 
	}
	ENABLE_INTERRUPTS();
}

mybool trylockMutex(mutex_t * mutexToLock)
{
	DISABLE_INTERRUPTS();
	mybool isSuceeded;
	if(*mutexToLock == RELEASED)
	{
		*mutexToLock = LOCKED;
		isSuceeded = TRUE; 
	}
	else
	{
		isSuceeded = FALSE;
	}
	ENABLE_INTERRUPTS();
	return isSuceeded;
}

void releaseMutex(mutex_t * mutexToUnlock)
{
	*mutexToUnlock = RELEASED;
}