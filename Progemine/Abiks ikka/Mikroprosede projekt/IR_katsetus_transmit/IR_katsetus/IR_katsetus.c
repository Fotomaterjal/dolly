/*
 * IR_katsetus.c
 *
 * Created: 16.06.2014 17:09:25
 *  Author: Ottk6uk
 */ 


#include <avr/io.h>
#include <avr/delay.h>
#define F_CPU 2000000;

void delay(uint8_t x);

void Burst();
void Pause();

int main(void)
{
	
	DDRD = 0x20;
	DDRA = 0xFF;
	
    while(1)
    {
		
		PORTA = 1;
		Burst();
		Pause();
		
		Burst();
		Pause();
		Burst();
		Pause();
		Burst();
		Pause();
		Burst();
		Pause();
		
		
		_delay_ms(1000);
		
		PORTA = 2;
		
		Burst();
		Pause();
		
		Pause();
		Pause();
		Pause();
		Pause();
		Pause();
		Pause();
		Pause();
		Pause();
		
		
		_delay_ms(1000);
		
    }
	
}

void Burst(){
	uint8_t i = 0;
	while(i<20){
		PORTD ^= 0x20;
		_delay_us(23);
		
		PORTD &= ~(0x20);
		_delay_us(23);
		i++;
	}
}

void Pause(){
	uint8_t i = 0;
	while(i<20){
		asm("nop");
		_delay_us(23);
		
		asm("nop");
		_delay_us(23);
		i++;
	}
}