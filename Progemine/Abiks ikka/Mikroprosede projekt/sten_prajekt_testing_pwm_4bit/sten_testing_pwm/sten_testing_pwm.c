/*
 * sten_testing_pwm.c
 *
 * Created: 9.06.2014 10:43:42
 *  Author: Oll
 */ 

//PB4 is RIGHT MOTOR and PB5 is LEFT MOTOR!

#define high(x)   ((x)>>2)
#define low(x)   ((x) & 0xFF)

#include "avr/interrupt.h"
#include <avr/io.h>
#include <avr/delay.h>

uint16_t measureVoltage();
void WriteTCNT(uint16_t i, uint8_t which);

uint8_t whatToDo = 0x0A;

uint16_t whatLeft(uint8_t whatToDo);
uint16_t whatRight(uint8_t whatToDo);

int main(void)
{
	

		//setting port b pin5 to output
		DDRB = (1 << PB5 | 1 << PB4);
		
		//ADC
		//selecting reference voltage VCC, measure voltage on pin ADC2
		ADMUX |= (1 << REFS0 | 1 << MUX1);
		
		//TIMER1
		//initializing timer values
		TCNT1 = 0;
		TCCR1A = 0;
		TCCR1B = 0;
		TCCR1C = 0;
		//setting 1 CPU clock, set normal mode for TCCR1B
		TCCR1B = (1 << CS10);
		//loading OCR1A with 40000 to have 20ms delay between pwm pulses
		OCR1A = 25536;
		//OCR1B = 25536;
		//setting TCCR1 to "non-PWM", setting OC1A low after compare match
		//TCCR1A = (1<<COM1A1);
		TIMSK1 = (1 << TOIE1) | (1 << OCIE1A);
		//WriteTCNT1(0xD6D8);
		
		for(int i = 0; i < 1000; i++){
			asm("nop");
		}
		
		//TIMER3
		//initializing timer values
		TCNT3 = 0;
		TCCR3A = 0;
		TCCR3B = 0;
		TCCR3C = 0;
		//setting CPU clock, set normal mode for TCCR1B
		TCCR3B = (1 << CS30);
		//loading OCR1A with 40000 to have 20ms delay between pwm pulses
		OCR3A = 25536;
		//OCR1B = 25536;
		//setting TCCR1 to "non-PWM", setting OC1A low after compare match
		//TCCR1A = (1<<COM1A1);
		TIMSK3 = (1 << TOIE3)| (1 << OCIE1A);
		//WriteTCNT1(0xD6D8);

		sei();
	
	
	
	
	
    while(1)
    {
		//whatToDo = 0x00;
		//_delay_ms(1000);
		//whatToDo = 0x0A;
		//_delay_ms(1000);
		//whatToDo = 0x05;
		//_delay_ms(1000);
		//whatToDo = 0x06;
		//_delay_ms(1000);
		//whatToDo = 0x09;
		//_delay_ms(1000);
    }
}



void WriteTCNT(uint16_t i , uint8_t which)
{
	unsigned char sreg;
	//TCNT3 = 0;
	//TCNT1 = 0;
	//uint16_t i;
	/* Save global interrupt flag */
	sreg = SREG;
	/* Disable interrupts */
	cli();
	/* Set TCNTn to i */
	if(which == 3){
		TCNT3 = i;
	}else{
		TCNT1 = i;	
	}	
	/* Restore global interrupt flag */
	SREG = sreg;
}


uint16_t whatRight(uint8_t whatToDo){
	uint16_t result;
	
	if(whatToDo == 0x05 || whatToDo == 0x09){
		result = 22940;	// pulse length less than 1,3 ms aka forward rotation
	}else if(whatToDo == 0x06 || whatToDo == 0x0A){
		result = 22136;	// pulse length more than 1,7 ms aka backward rotation
	}else{
		result = 22700;	// pulse length exactly 1,5 ms aka no motor rotation
	}
	return result;
}

uint16_t whatLeft(uint8_t whatToDo){//must think backwards because motor is upside down
	uint16_t result2;
	if(whatToDo == 0x0A || whatToDo == 0x09){
		//return 22940;
		result2 = 22945;	// pulse length less than 1,3 ms aka forward rotation
	}else if(whatToDo == 0x06 || whatToDo == 0x05){
		//return 22136;
		result2 = 22141;	// pulse length more than 1,7 ms aka backward rotation
	}else{
		//return 22536;
		result2 = 22725;	// pulse length exactly 1,5 ms aka no motor rotation
	}
	return result2;
}


SIGNAL(TIMER1_COMPA_vect){
	PORTB = PORTB & ~(0x20);	//PB5
}

SIGNAL(TIMER3_COMPA_vect){
	PORTB = PORTB & ~(0x10);	//PB4
}

SIGNAL(TIMER1_OVF_vect){
	PORTB = 0x20;
	uint16_t reading = whatLeft(whatToDo);			//PB5
	WriteTCNT(reading, 1);		//vahemik 1,3 kuni 1,7 ms impulsi tegemiseks
}

SIGNAL(TIMER3_OVF_vect){
	PORTB = 0x10;
	uint16_t reading = whatRight(whatToDo);			//PB4
	WriteTCNT(reading, 3);		//vahemik 1,3 kuni 1,7 ms impulsi tegemiseks
}