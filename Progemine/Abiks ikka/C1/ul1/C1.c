/*
 * C1.c
 *
 * Created: 14.04.2014 8:23:48
 *  Author: Oll
 */ 


#include <avr/io.h>

int main(void)
{
	int led = 0x01;

	DDRA = led;			//data direction
	PORTA = led;		//light one LED
	
	int empty = 0xFF;
	
    while(1)
    {
		for(int i = 0; i < 20000; i++){
			asm("NOP");
		}
		PORTA = PORTA^empty;
		
    }
}