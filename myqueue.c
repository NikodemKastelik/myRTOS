/*
 * myqueue.c
 *
 * Created: 2018-03-07 14:22:23
 *  Author: Nikodem Kastelik
 */

#include "myrtos.h"

#define WAIT_FOR_NULL 1

/** @brief Puts string on queue
      * @param my_queue* Queue - pointer to queue to put string to
      * @param char* arr - array filled with string to put
      * @return nothing
      */
void queuePut(myQueue * Queue, const char * arr)
{
	lockMutex(&(Queue->readWriteMutex));
	char c;
	uint16_t i=0;
	while(WAIT_FOR_NULL)
	{
		c = arr[i++];
		Queue->fifo[Queue->writeIndex] = c;
		Queue->writeIndex = (Queue->writeIndex +1) % FIFO_SIZE;
		if(c == '\0')
		{
			break;	 
		}
	}
	releaseMutex(&(Queue->readWriteMutex));
}

/** @brief Get string from queue
      * @param my_queue* Queue - pointer to queue to get string from
      * @param char* arr - array to get filled by queue 
      * @return nothing (indirectly array containing popped string)
      */
mybool queueGet(myQueue * Queue, char * arr)
{
	lockMutex(&(Queue->readWriteMutex));
	mybool status;
	if(queueDoesContainItem(Queue))
	{
		char c;
		uint16_t i=0;
		while(WAIT_FOR_NULL)
		{
			c = Queue->fifo[Queue->readIndex];
			arr[i++] = c;
			Queue->readIndex = (Queue->readIndex +1) % FIFO_SIZE;
			if(c == '\0')
			{
				break;
			}
		}
		status = SUCCESS;
	}
	else
	{
		status = FAILURE;
	}
	releaseMutex(&(Queue->readWriteMutex));
	return status;
}

void queueInit(myQueue * Queue)
{
	Queue->readIndex = 0;
	Queue->writeIndex = 0;
	Queue->readWriteMutex = createMutex();
}

mybool queueDoesContainItem(myQueue * Queue)
{
	return Queue->readIndex == Queue->writeIndex ? FALSE : TRUE;
}