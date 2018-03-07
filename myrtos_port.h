/*
 * myrtos_port.h
 *
 * Created: 2018-03-06 00:01:49
 *  Author: Nikodem Kastelik
 */ 


#ifndef MYRTOS_PORT_H_
#define MYRTOS_PORT_H_

#include <avr/interrupt.h>

#define INIT_SYSTICK_TIMER() {TIMSK = (1<<TOIE0); TCCR0 = (1<<CS01);}

#define ENABLE_INTERRUPTS() sei()
#define DISABLE_INTERRUPTS() cli()




#endif /* MYRTOS_PORT_H_ */