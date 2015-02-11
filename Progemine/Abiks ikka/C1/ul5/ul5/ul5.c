/*
 * ul5.c
 *
 * Created: 14.04.2014 10:41:58
 *  Author: Oll
 */ 



#include <avr/io.h>


int main(void)
{
	MCUCR |= (1<<JTD);
	MCUCR |= (1<<JTD);
	DDRA = 0x01;			//data direction
	
	
	PORTF = (1 << PF5 );
    while(1)
    {
        if((~PINF)&(1 << PF5)){
			PORTA = 0x01;
		}
    }
}
