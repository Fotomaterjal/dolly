/*
 * C1.c
 *
 * Created: 14.04.2014 8:23:48
 *  Author: Oll
 */ 


#include <avr/io.h>

void delayHalfSecond();
void longSignal();
void shortSignal();


int main(void)
{
	int led = 0x01;

	DDRA = led;			//data direction
	PORTA = led;		//light one LED
		
    while(1)
    {
		shortSignal();
		shortSignal();
		shortSignal();
		
		delayHalfSecond();
		delayHalfSecond();
		
		longSignal();
		longSignal();
		longSignal();

		delayHalfSecond();
		delayHalfSecond();
		
		shortSignal();
		shortSignal();
		shortSignal();
		
		delayHalfSecond();
		delayHalfSecond();
		delayHalfSecond();
		delayHalfSecond();
		delayHalfSecond();
		delayHalfSecond();
    }
	
}

void delayHalfSecond(){
	for(uint16_t i = 0; i < 50000; i++){
		asm("nop");
	}
}

void shortSignal(){
	PORTA = 0x01;
	delayHalfSecond();
	PORTA = 0x00;
	delayHalfSecond();
}

void longSignal(){
	PORTA = 0x01;
	delayHalfSecond();
	delayHalfSecond();
	delayHalfSecond();
	PORTA = 0x00;
	delayHalfSecond();
}