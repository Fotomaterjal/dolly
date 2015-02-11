/*
 * ul1.c
 *
 * Created: 21.04.2014 8:19:19
 *  Author: Oll
 */ 


#include <avr/io.h>

void measureVoltage();

int main(void)
{
	//UART
	//setting baud rate to 9600
	UBRR1 = 25;
	UCSR1A |= (1 << U2X1);	
	//setting up 8 data bits
	UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);
	//enabling transmitter
	UCSR1B |= (1 << TXEN1);
	
	//ADC
	//selecting reference voltage VCC, measure voltage on pin ADC2
	ADMUX |= (1 << REFS0 | 1 << MUX1);
	
	//PORT A to output
	DDRA = 0xFF;
	
    while(1)
    {
        uint16_t adc = measureVoltage();
		uint8_t tosend = adc / 100;
		if (tosend > 9) {
			tosend = 9;
		}
		
		sendVoltage('0'+tosend);
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

void sendVoltage(uint8_t number){
	//load value to UART sending register
	UDR1 = number;
	//wait loop for send to be completed
	while(~UCSR1A&(1 << TXC1)){
		asm("nop");
	}
	//clearing TX complete flag
	UCSR1A |= (1 << TXC1);
}
