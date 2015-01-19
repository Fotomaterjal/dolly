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
	DDRA = 0xFF;			//data direction
	
	PORTF = (1 << PF5 );
	
	uint8_t which_led = 1;
	
    while(1)
    {
        if((~PINF)&(1 << PF5)){
			which_led = which_led << 1;
			if(which_led == 0){
				which_led = 1;
			}
			PORTA = which_led;
			checkButton();
		}
    }
}

void checkButton(){
	while((~PINF)&(1 << PF5)){
		asm("nop");
	}
}