/*
 * sten_testing_pwm.c
 *
 * Created: 9.06.2014 10:43:42
 *  Author: Oll
 */ 

#define high(x)   ((x)>>2)
#define low(x)   ((x) & 0xFF)

#include "avr/interrupt.h"
#include <avr/io.h>

uint16_t measureVoltage();
void WriteTCNT(uint16_t i, uint8_t which);


int main(void)
{
		//setting port b pin5 to output
		DDRB = (1 << PB5 | 1 << PB4);
		//PORTB = 0x30;
		
		//ADC
		//selecting reference voltage VCC, measure voltage on pin ADC2
		ADMUX |= (1 << REFS0 | 1 << MUX1);
		
		//TIMER
		//initializing timer values
		TCNT1 = 0;
		TCCR1A = 0;
		TCCR1B = 0;
		TCCR1C = 0;
		//setting 1/8 CPU clock, set normal mode for TCCR1B
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
		//setting 1/8 CPU clock, set normal mode for TCCR1B
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
		//TODO::
    }
}



void WriteTCNT(uint16_t i , uint8_t which)
{
	unsigned char sreg;
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

uint16_t measureVoltage(){
	//ADC enable, ADC start conversion
	ADCSRA = (1 << ADEN);
	ADCSRA |= (1 << ADSC);
	//loop for checking ADIF flag
	while(~ADCSRA&(1 << ADIF)){
		asm("nop");
	}
	//if conversion complete, reset ADIF flag
	ADCSRA = (1 << ADIF);
	return ADC;
}

SIGNAL(TIMER1_COMPA_vect){
	PORTB = PORTB & ~(0x20);	//PB5
}

SIGNAL(TIMER3_COMPA_vect){
	PORTB = PORTB & ~(0x10);	//PB4
}

SIGNAL(TIMER1_OVF_vect){
	PORTB = 0x20;
	uint16_t pinge = measureVoltage();			//PB5
	WriteTCNT(22136 + pinge*3.3, 1);		//vahemik 1,3 kuni 1,7 ms impulsi tegemiseks
}

SIGNAL(TIMER3_OVF_vect){
	PORTB = 0x10;
	uint16_t pinge = measureVoltage();			//PB4
	WriteTCNT(22136 + pinge*3.3, 3);		//vahemik 1,3 kuni 1,7 ms impulsi tegemiseks
}