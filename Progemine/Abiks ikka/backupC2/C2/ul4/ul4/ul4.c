/*
 * ul4.c
 *
 * Created: 21.04.2014 11:25:43
 *  Author: Oll
 */ 


#include <avr/io.h>
#include "avr/interrupt.h"

#define F_CPU 2000000
#define PRESCALER 1024

void write_values(unsigned int compA);

int main(void)
{
	//UART
	//setting baud rate to 9600
	UBRR1 = 25;
	UCSR1A = (1 << U2X1);
	//setting up 8 data bits
	UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);
	//enabling transmitter, receiver, receive complete interrupt
	UCSR1B = (1 << TXEN1 | 1 << RXEN1 | 1 << RXCIE1);
	
	//TIMER-COUNTER
	//setting clock to 1/1024 CPU clock, setting timer to compare match mode
	TCCR1B = (1<<CS10) | (1<<CS12)| (1 << WGM12);
	//initializing timer values
	TCCR1A = 0;
	TCCR1C = 0;
	//timer 1 compare match A interrupt enable
	TIMSK1 = (1 << OCIE1A);
	//load number into compare match, 0.5 Hz
	OCR1A = 1953;
	
	//LED INITIALISATION
	//setting PORTA pin 0 to output
	DDRA = 0x01;
	//lighting LED 0
	PORTA = 0x01;

	//INTERRUPTS
	//global interrupt enable
	sei();
	
    while(1)
    {
        asm("nop");
    }
}



void write_values(unsigned int compA){
	OCR1A = compA;
	/*TCCR1A = 0;
	TCCR1C = 0;*/
	TCNT1 = 0;
}

//!use the right letter!
SIGNAL(TIMER1_COMPA_vect){
	PORTA ^= 0xFF;
}

SIGNAL(USART1_RX_vect){
	// Enable timer interrupts
	TIMSK1 = (1<<OCIE1A);
	// Read UART
	uint8_t a = UDR1;
	// do stuff
	if(a == '0') {			// LED OFF
		TIMSK1 = 0;
		PORTA = 0;
	} else if (a >= '1' && a <= '9'){
		uint8_t num = a - '0';
		uint16_t oncePerSecond = F_CPU/PRESCALER/2;
		write_values(oncePerSecond / num);
	} else {					// LED ON
		TIMSK1 = 0;
		PORTA = 0x01;
	}
}