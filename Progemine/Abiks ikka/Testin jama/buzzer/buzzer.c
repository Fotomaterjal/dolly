/*
 * buzzer.c
 *
 * Created: 8.06.2014 13:28:36
 *  Author: Oll
 */ 


#include <avr/io.h>

int main(void)
{
	DDRB = 0b01000000;		//light one LED
	PORTB = 0b01000000;
	
    while(1)
		for(int i = 0; i < 20000; i++);
		PORTB ^= 0xFF;
	{
        //TODO:: Please write your application code 
    }
}