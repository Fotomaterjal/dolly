/*
 * GccApplication1.c
 *
 * Created: 22.06.2014 23:39:25
 *  Author: Oll
 */ 


#include <avr/io.h>
#include <avr/delay.h>
#define F_CPU 2000000;

void send_zero();
void send_one();

uint8_t whatToDo = 0b00001001;
uint8_t testingLED = 0x00;
char whatToDoArray[4];


int main(void)
{
	DDRA = 0xFF;
	
    while(1)
    {	
		send_zero();
			// is the first bit '1' ?
			if(whatToDo & 0x01){
				send_zero();
			}else{
				send_one();
			}
			// is the second bit '1' ?
			if((whatToDo >> 1) & 0x01){
				send_zero();
			}else{
				send_one();
			}
			//PORTA = 0x04;
			// is the third bit '1' ?
			if((whatToDo >> 2) & 0x01){
				send_zero();
			}else{
				send_one();
			}
			// is the fourth bit '1' ?
			if((whatToDo >> 3) & 0x01){
				send_zero();
			}else{
				send_one();
			}
			// wait and chill
			PORTA ^= whatToDo;
			_delay_ms(2000);	
			
    }
}


// signal that generates '0' in the receiver
void send_zero(){
	uint8_t k = 0;
	while(k<20){
		// keep output HIGH for ~27 us
		PORTD |= 0x20;
		_delay_us(23);
		// keep output LOW for ~27 us
		PORTD &= ~(0x20);
		_delay_us(23);
		k++;
	}
}

// signal that generates '1' in the receiver
void send_one(){
	uint8_t k = 0;
	while(k<20){
		// keep output LOW for ~27 us
		asm("nop");
		_delay_us(23);
		// keep output LOW for ~27 us
		asm("nop");
		_delay_us(23);
		k++;
	}
}

