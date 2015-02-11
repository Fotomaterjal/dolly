/*
 * C1.c
 *
 * Created: 14.04.2014 8:23:48
 *  Author: Oll
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void TIM16_WriteTCNT1(UINT16_C i);
void init_CPUClockTC1_256();


int main(){
	cli();
	init_DefaultCPUClockTC1();
	sei();
	
	
	
	
}

void TIM16_WriteTCNT1(UINT16_C i){
	unsigned char sreg;
}

void init_CPUClockTC1_256(){
	// setting CPU clock/256
	// 1s = 62500 cycles
	TCCR1B = (1 << CS12); 
}

void init_Timer1(){
	// setting CPU clock/256
	// 1s = 62500 cycles
	TCCR1B = (1 << CS12);
	// enable Timer Overflow Interrupt
	TIMSK1 |= (1<<TOIE1);
	// Global Interrupt enable
	SREG |= (1<<I);
	
}
