/*
 * myrtos.c
 *
 * Created: 2018-02-24 12:00:44
 *  Author: Nikodem Kastelik
 */

#include "myrtos.h"
#include "myrt_config.h"
#include "myrtos_port.h"
#include <avr/io.h>
#include <avr/interrupt.h>

myrtSysTimeMs_t systemTime;

myrtTask allTasks[TASKS_COUNT_WITH_IDLE];
myrtTask * currentTask;

static inline __attribute__((always_inline)) void myrtSwitchContext(myrtTask * nextTaskToExecute) 
{
    asm volatile (
            "push r31 \n\t"
            "push r30 \n\t"
            "push r29 \n\t"
            "push r28 \n\t"
            "push r27 \n\t"
            "push r26 \n\t"
            "push r25 \n\t"
            "push r24 \n\t"
            "push r23 \n\t"
            "push r22 \n\t"
            "push r21 \n\t"
            "push r20 \n\t"
            "push r19 \n\t"
            "push r18 \n\t"
            "push r17 \n\t"
            "push r16 \n\t"
            "push r15 \n\t"
            "push r14 \n\t"
            "push r13 \n\t"
            "push r12 \n\t"
            "push r11 \n\t"
            "push r10 \n\t"
            "push r9 \n\t"
            "push r8 \n\t"
            "push r7 \n\t"
            "push r6 \n\t"
            "push r5 \n\t"
            "push r4 \n\t"
            "push r3 \n\t"
            "push r2 \n\t"
            "push r1 \n\t"
            "push r0 \n\t"
            "in   r0, %[_SREG] \n\t"
            "push r0 \n\t"
            "lds  r26, currentTask \n\t"
            "lds  r27, currentTask+1 \n\t"
            "sbiw r26, 0 \n\t"
            "breq change_context \n\t" // check if current task is null
            "in   r0, %[_SPL] \n\t"
            "st   X+, r0 \n\t"
            "in   r0, %[_SPH] \n\t"
            "st   X+, r0 \n\t"
            "change_context:"
            "mov  r26, %A[_next_task] \n\t"
            "mov  r27, %B[_next_task] \n\t"
            "sts  currentTask, r26 \n\t"
            "sts  currentTask+1, r27 \n\t"
            "ld   r0, X+ \n\t"
            "out  %[_SPL], r0 \n\t"
            "ld   r0, X+ \n\t"
            "out  %[_SPH], r0 \n\t"
            "pop  r31 \n\t"
            "bst  r31, %[_I] \n\t"
            "bld  r31, %[_T] \n\t" // store I flag in T bit in order to restore it later
            "andi r31, %[_I_MASK] \n\t"
            "out  %[_SREG], r31 \n\t"
            "pop  r0 \n\t"
            "pop  r1 \n\t"
            "pop  r2 \n\t"
            "pop  r3 \n\t"
            "pop  r4 \n\t"
            "pop  r5 \n\t"
            "pop  r6 \n\t"
            "pop  r7 \n\t"
            "pop  r8 \n\t"
            "pop  r9 \n\t"
            "pop  r10 \n\t"
            "pop  r11 \n\t"
            "pop  r12 \n\t"
            "pop  r13 \n\t"
            "pop  r14 \n\t"
            "pop  r15 \n\t"
            "pop  r16 \n\t"
            "pop  r17 \n\t"
            "pop  r18 \n\t"
            "pop  r19 \n\t"
            "pop  r20 \n\t"
            "pop  r21 \n\t"
            "pop  r22 \n\t"
            "pop  r23 \n\t"
            "pop  r24 \n\t"
            "pop  r25 \n\t"
            "pop  r26 \n\t"
            "pop  r27 \n\t"
            "pop  r28 \n\t"
            "pop  r29 \n\t"
            "pop  r30 \n\t"
            "pop  r31 \n\t"
            "brtc 2f \n\t" //if the T flag is clear, do the non-interrupt enable return
            "reti \n\t"
            "2: \n\t"
            "ret \n\t"
            "" ::
            [_SREG] "i" _SFR_IO_ADDR(SREG),
            [_I] "i" SREG_I,
            [_T] "i" SREG_T,
            [_I_MASK] "i" (~(1 << SREG_I)),
            [_SPL] "i" _SFR_IO_ADDR(SPL),
            [_SPH] "i" _SFR_IO_ADDR(SPH),
            [_next_task] "r" (nextTaskToExecute)
		);	
}

static inline __attribute__((always_inline)) void myrtWakeupSleepingTasks()
{
	for(uint8_t taskIndex = 0; taskIndex < TASKS_COUNT_WITH_IDLE ; taskIndex++)
	{
		myrtTask * taskToUpdate = &allTasks[taskIndex];
		if(taskToUpdate->thisTaskStatus == SLEEPING)
		{
			if(taskToUpdate->blockSource.timeWhenUnblocked == systemTime)
			{
				taskToUpdate->thisTaskStatus = READY;
			}
		}
	}
}

static inline __attribute__((always_inline)) myrtTask * myrtChooseNextTask()
{
	myrtTask * nextTaskToExecute;
	for(uint8_t taskIndex = 0; taskIndex < TASKS_COUNT_WITH_IDLE ; taskIndex++)
	{
		nextTaskToExecute = &allTasks[taskIndex];
		if(nextTaskToExecute->thisTaskStatus == READY)
		{
			break;
		}
		else if(nextTaskToExecute->thisTaskStatus == MUTEX)
		{
			if(*(nextTaskToExecute->blockSource.mutex) == RELEASED)
			{
				nextTaskToExecute->thisTaskStatus = READY;
				break;
			}
		}
	}
	return nextTaskToExecute;
}

/*
static inline __attribute__((always_inline)) void myrtUpdateBlockedTasks()
{
	for(uint8_t taskIndex = 0; taskIndex < TASKS_COUNT_WITH_IDLE ; taskIndex++)
	{
		myrtTask * taskToUpdate = &allTasks[taskIndex];
		if(taskToUpdate->thisTaskStatus == MUTEX)
		{
			if(*(taskToUpdate->blockSource.mutex) == RELEASED)
			{
				taskToUpdate->thisTaskStatus = READY;
			}
		}
	}
}*/


// * @note 
// * Os optimization in order to prevent compiler for using stack here. Stack cannot be messed from this point to
// * context switch because otherwise return address will be wrong.
void  __attribute__((optimize("Os"))) myrtSchedule()
{
	/*
	myrtTask * nextTaskToExecute = &allTasks[FIRST_TASK];
	while( nextTaskToExecute->thisTaskStatus != READY)
	{
		nextTaskToExecute++;
	}*/
	myrtTask * nextTaskToExecute = myrtChooseNextTask();
	
	//myrtUpdateChosenTask(nextTaskToExecute);	
	if(nextTaskToExecute != currentTask)
	{
		myrtSwitchContext(nextTaskToExecute);
	}
}

void myrtCreateTask(void * taskFunction, uint8_t thisTaskStackSize)
{
	static uint8_t currentTaskIndex;
	static uint8_t * previousTaskStackEnd = (uint8_t *)(RAMEND - IDLE_TASK_TOTAL_STACK_SIZE);
	
	uint8_t * taskStackPointer = previousTaskStackEnd;
	
	previousTaskStackEnd = previousTaskStackEnd - (TASK_MINIMUM_STACK_SIZE + thisTaskStackSize) - 1;
	
	taskStackPointer[PCL_STACK_INDEX] = (uint16_t)taskFunction & 0xFF;
	taskStackPointer[PCH_STACK_INDEX] = (uint16_t)taskFunction >> 8;
	for(int8_t index=PCH_STACK_INDEX-1 ; index > SREG_STACK_INDEX ; index--)
	{
		taskStackPointer[index] = 0;
	}
	taskStackPointer[SREG_STACK_INDEX] = 1 << SREG_I;
	
	(allTasks[currentTaskIndex]).thisTaskStackPointer = taskStackPointer - TASK_ALL_REGISTERS_SIZE - 1;
	(allTasks[currentTaskIndex]).thisTaskStatus = READY;
	currentTaskIndex++;
}

/*
void myrtDebugSwitchTasksStates()
{
	myrtTask * thisTask;
	for(uint8_t taskIndex=0 ;  taskIndex < TASKS_COUNT_WITH_IDLE ; taskIndex++)
	{
		thisTask = &allTasks[taskIndex];
		if(thisTask == currentTask)
		{
			thisTask->thisTaskStatus = BLOCKED;
		}
		else
		{
			thisTask->thisTaskStatus = READY;
		}
	}
}*/

static inline __attribute__((always_inline)) void myrtSysTick()
{
	systemTime++;
}

void __attribute__ ((naked)) SYSTICK_TIMER_ISR(void)
{
	cli();
	myrtSysTick();
	//myrtDebugSwitchTasksStates();
	myrtWakeupSleepingTasks();
	myrtSchedule();
	reti();
}

void myrtSleep(uint16_t timeInMs)
{
	cli();
	currentTask->thisTaskStatus = SLEEPING;
	currentTask->blockSource.timeWhenUnblocked = systemTime + timeInMs;
	myrtSchedule();
	sei();
}

void myrtStart(void (*idleTaskFunction)(void))
{
	INIT_SYSTICK_TIMER();
	currentTask = &allTasks[IDLE_TASK];
	sei();
	(*idleTaskFunction)();
}