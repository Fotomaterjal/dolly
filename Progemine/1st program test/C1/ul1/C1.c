/*
 * C1.c
 *
 * Created: 14.04.2014 8:23:48
 *  Author: Oll
 */ 


#include <avr/io.h>

int main(void)
{
	int led = 0x20;

	DDRD = led;			//data direction
	PORTD = led;		//light one LED
	
	int empty = 0xFF;
	
    while(1)
    {
		for(int i = 0; i < 200000; i++){
			asm("NOP");
		}
		PORTD = PORTD^empty;
		
    }
}