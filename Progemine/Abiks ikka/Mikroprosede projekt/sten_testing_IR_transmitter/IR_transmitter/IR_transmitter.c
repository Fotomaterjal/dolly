/*
 * IR_transmitter.c
 *
 * Created: 21.06.2014 18:59:03
 *  Author: Oll
 */ 


#include <avr/io.h>
#include <avr/delay.h>
#define F_CPU 2000000


void transmit_zero(void);

int main(void)
{
    while(1)
    {
        transmit_zero();
    }
}


void transmit_zero(){
	for(int i = 0; i < 20; i++){
		//on
		PORTD |= 0x20;
		_delay_us(23);
		//off
		PORTD &= ~(0x20);
		_delay_us(23);
	}	
	for(int i = 0; i < 20; i++){
		asm("nop");
		_delay_us(23);
		asm("nop");
		_delay_us(23);
	}
}

