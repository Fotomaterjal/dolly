/*
 * sten_prajekt.c
 *
 * Created: 3.06.2014 11:27:30
 *  Author: Oll
 */ 
/*http://www.avrfreaks.net/index.php?name=PNphpBB2&file=printview&t=97409&start=0 */

#define high(x)   ((x)>>2)
#define low(x)   ((x) & 0xFF)

#include <avr/io.h>

uint16_t measureVoltage();
#include "avr/interrupt.h"

void WriteTCNTn(uint16_t i );

int main(void)
{
	////UART
	////setting baud rate to 9600
	//UBRR1 = 25;
	//UCSR1A |= (1 << U2X1);
	////setting up 8 data bits
	//UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);
	////enabling transmitter
	////enabling transmitter, receiver, receive complete interrupt
	//UCSR1B = (1 << TXEN1 | 1 << RXEN1 | 1 << RXCIE1);
	sei();
	
	
	//ADC
	//selecting reference voltage VCC, measure voltage on pin ADC2
	ADMUX |= (1 << REFS0 | 1 << MUX1);
	
	//setting port b pin5 to output
	DDRB = (1 << PB5 | 1 << PB4);
	
	//TIMER
	//initializing timer values
	TCCR1A = 0;
	TCCR1C = 0;
	//setting default clock (CPU clock), set compare match mode for TCCR1B (CTC)
	TCCR1B = (1 << CS10 | 1 << WGM12);
	//setting TCCR1 to "Fast PWM 10bit", setting OC1A low after compare match
	TCCR1A = (1<<WGM11 | 1<<WGM10 | 1<<COM1A1);	
	
	//TIMER2
	//initializing timer values
	TCCR2A = 0;
	TCCR2B = 0;
	//setting default clock (CPU clock), set compare match mode for TCCR1B (CTC)
	TCCR2B = (1 << CS20 );
	//setting TCCR1 to "Fast PWM", setting OC2A low after compare match
	TCCR2A = (1<<WGM21 | 1<<WGM20 | 1<<COM2A1);	
	
	
	
	
	
	while(1)
	{
		// output compare register 1A (for PWM)
		OCR1A = measureVoltage();			//PB5
		//UDR1 = OCR1A;
		OCR2A =	high(measureVoltage());		//PB4
		//UDR1 = OCR2A;
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

//SIGNAL(USART1_RX_vect){
	//// Enable timer interrupts
	//TIMSK1 = (1<<OCIE1A);
	//// Read UART
	//uint8_t a = UDR1;
	//sendLetter(a);
//}
//
//void sendLetter(uint8_t letter){
	//while ((UCSR1A & (1 << UDRE1)) == 0) ;
	////load value to UART sending register
	//UDR1 = letter;
	////wait loop for send to be completed
	//while(~UCSR1A&(1 << TXC1)){
		//asm("nop");
	//}
	////clearing TX complete flag
	//UCSR1A |= (1 << TXC1);
//}


void WriteTCNT1(uint16_t i )
{
	unsigned char sreg;
	//uint16_t i;
	/* Save global interrupt flag */
	sreg = SREG;
	/* Disable interrupts */
	cli();
	/* Set TCNTn to i */
	TCNT1 = i;
	/* Restore global interrupt flag */
	SREG = sreg;
}

