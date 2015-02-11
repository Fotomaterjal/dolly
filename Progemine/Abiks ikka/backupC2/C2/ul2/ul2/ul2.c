/*
 * ul2.c
 *
 * Created: 21.04.2014 9:36:26
 *  Author: Oll
 */ 


#include <avr/io.h>

uint16_t measureVoltage();

int main(void)
{
	//ADC
	//selecting reference voltage VCC, measure voltage on pin ADC2
	ADMUX |= (1 << REFS0 | 1 << MUX1);
	
	//setting portb pin5 to output
	DDRB = (1 << PB5);
	
	//TIMER
	//initializing timer values
	TCCR1A = 0;
	TCCR1C = 0;
	//setting default clock (CPU clock), set compare match mode for TCCR1B (CTC)
	TCCR1B = (1 << CS10 | 1 << WGM12);
	//setting TCCR1 to "Fast PWM 10bit", setting OC1A low after compare match
	TCCR1A = (1<<WGM11 | 1<<WGM10 | 1<<COM1A1);
	
	
		
    while(1)
    {
		OCR1A = measureVoltage();
    }
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