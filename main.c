/* 
 * myRTOS.c
 * 
 * Created: 2018-02-24 11:52:54
 * Author : Nikodem Kastelik
 */ 

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "myrtos.h"

void Task1()
{
	while(1)
	{
		PORTC ^= 0xFF;
		myrtSleep(10);	
	}
}

void Task2()
{
	while(1)
	{
		PORTB ^= 0xFF;
		myrtSleep(20);	
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
	
	myrtCreateTask(Task1, 10);
	myrtCreateTask(Task2, 10);
	
	myrtStart(idleTask);
	while(1);
}

