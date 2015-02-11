/*
 * ul4.c
 *
 * Created: 14.04.2014 9:47:54
 *  Author: Oll
 */ 


#include <avr/io.h>

void delayMs();				//function declaration

int main(void)
{
	DDRA = 0x01;			//data direction
	PORTA = 0x01;			//light one LED
	
    while(1)
    {

        shortSignal();
		shortSignal();
		shortSignal();
		
		delayMs(400);
		
		longSignal();
		longSignal();
		longSignal();

		delayMs(400);
		
		shortSignal();
		shortSignal();
		shortSignal();
		
		delayMs(1200);
    }
}


void delayMs(uint16_t milliseconds){				//
	uint32_t cycles = ((uint32_t)milliseconds)*180;	//using 32 bit registers for 32 bit numbers
	for(uint32_t i = 0; i < cycles; i++){
		asm("nop");
	}
}

void shortSignal(){
	PORTA = 0x01;
	delayMs(200);
	PORTA = 0x00;
	delayMs(200);
}

void longSignal(){
	PORTA = 0x01;
	delayMs(600);
	PORTA = 0x00;
	delayMs(200);
}