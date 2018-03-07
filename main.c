/*  
 * myRTOS.c
 * 
 * Created: 2018-02-24 11:52:54
 * Author : Nikodem Kastelik
 * 
 */ 

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "myrtos.h"

myQueue msgQueue;

void Task1()
{
	char arr[5];
	while(1)
	{
		if(queueGet(&msgQueue, arr))
		{
			PORTB ^= 0xFF;
			myrtSleep(10);
			PORTB ^= 0xFF;
		}
		else
		{
			myrtSchedule();
		}
	}
}

void Task2()
{
	char arr[] = "abc";
	while(1)
	{
	
	}
}

void idleTask()
{
	while(1)
	{
		PORTD ^= 0xFF;
	}
}

int main(void)
{	
	DDRC = 0xFF;
	DDRB = 0xFF;
	DDRD = 0xFF;
	
	queueInit(&msgQueue);
	
	myrtCreateTask(Task1, 20);
	myrtCreateTask(Task2, 20);
	
	myrtStart(idleTask);
	while(1);
}

