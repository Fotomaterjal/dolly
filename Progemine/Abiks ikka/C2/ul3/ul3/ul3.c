/*
 * ul3.c
 *
 * Created: 21.04.2014 10:09:23
 *  Author: Oll
 */ 


#include <avr/io.h>
#include "avr/interrupt.h"

void sendLetter(uint8_t letter);
void sendName(char name[]);
//SIGNAL(TIMER0_OVF_vect);

int main(void)
{
	//UART
	//setting baud rate to 9600
	UBRR1 = 25;
	UCSR1A = (1 << U2X1);
	//setting up 8 data bits
	UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);
	//enabling transmitter
	UCSR1B = (1 << TXEN1);
	
	//LED initialization
	DDRA = 0xFF;
	PORTA = 0xFF;
	
	//TIMER
	//initializing timer values
	TCCR1A = 0;
	TCCR1C = 0;
	//setting default clock (CPU clock)
	TCCR1B = (1 << CS10);
	////loading value to compare match
	//OCR1A = 2000;
	
	//INTERRUPTS
	//global interrupt enable
	sei();
	//Timer/Counter1 Interrupt Mask Register, Compare match A interrupt enable			
	TIMSK1 = (1 << TOIE1);
	
		
	
	char name[] = "Sten-Oliver";
	
    while(1)
    {
		sendName(name);
    }
}


void sendLetter(uint8_t letter){
	while ((UCSR1A & (1 << UDRE1)) == 0) ;
	//load value to UART sending register
	UDR1 = letter;
}

void sendName(char name[]){
	for( int i = 0; i < strlen(name); i++){
		sendLetter(name[i]);
	}
}

SIGNAL(TIMER1_OVF_vect){
	PORTA ^= 0xFF;
}