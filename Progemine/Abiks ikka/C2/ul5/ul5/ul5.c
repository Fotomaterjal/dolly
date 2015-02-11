/*
 * ul5.c
 *
 * Created: 28.04.2014 9:27:19
 *  Author: Oll
 */ 

#define F_CPU 2000000
#define PRESCALER1 1024
#define PRESCALER2 128
#include <avr/io.h>
#include <util/delay.h>
#include "avr/interrupt.h"
#include <stdlib.h>


#define NUM0 0b11111100
#define NUM1 0b01100000
#define NUM2 0b11011010
#define NUM3 0b11110010
#define NUM4 0b01100110
#define NUM5 0b10110110
#define NUM6 0b10111110
#define NUM7 0b11100000
#define NUM8 0b11111110
#define NUM9 0b11110110

#define ARRAY_LENGTH 10

uint8_t leftData = 0;
uint8_t rightData = 0;
uint8_t numberArray[ARRAY_LENGTH] = {NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9};

void shift_out(uint8_t data, uint8_t position);


int main(void){
	
	
	
    //TIMER-COUNTER 1 for right display
    //setting clock to clk/64 CPU clock, setting timer to compare match mode
    TCCR1B = (1<<CS10) | (1<<CS11) | (1 << WGM12);
    //initializing timer values
    TCCR1A = 0;
    TCCR1C = 0;
    //timer 1 compare match A interrupt enable
    TIMSK1 = (1 << OCIE1A);
    //load number into compare match, 100 Hz
    OCR1A = 31250;
	
	//TIMER-COUNTER 2 for left display, 10 times slower
	//setting clock to clk/1024 CPU clock, setting timer to compare match mode
	TCCR0B = (1<<CS22) | (1<<CS20) | (1 << WGM22);
	//initializing timer values
	TCCR0A = 0;
	//TCCR = 0;
	//timer 1 compare match A interrupt enable
	TIMSK2 = (1 << OCIE2A);
	//load number into compare match
	OCR2A = 19531;
    	
    //LED INITIALISATION
    //setting PORTA pin 0 to output
    DDRA = 0x01;
    //lighting LED 0
    PORTA = 0x01;

    //INTERRUPTS
    //global interrupt enable
    sei();
	
	// 7 segment display control
	// 7-segment data connected to PE4
	DDRE = 0xFF;
	PORTE = 0x00;
	
	DDRB = 0x80;	//latch to output
	DDRD = 0x10;	
	
		
    while(1){
		shift_out(numberArray[rightData], 0x01);	// refresh right display
		shift_out(numberArray[leftData], 0x10);		// refresh left display
    }
    
	
}

void shift_out(uint8_t data, uint8_t position){
	//TODO
	// Siin on mingi loogikaviga sees. Kuvab korraga ainult kas paremat
	// või vasakut displayd. võiks kuvada korraga mõlemat.
	
	
	if(position == 0x10){
		PORTD = PORTD & ~(1 << PD4);					//PD4 to 0
	}else{
		PORTD = PORTD | (1 << PD4);						//PD4 to 1
	}	
	
	volatile uint8_t dataBit_i = 0;
	for(int i = 0; i < 8; i++){
		PORTE = (PORTE & ~(1 << PE3));	// clock signal to 0
		// data pin = PE4
		dataBit_i  = data & (1 << i);
		dataBit_i = dataBit_i >> i;		// now data is on position 0
		dataBit_i = dataBit_i << 4;		// now data is on position 4
		
		PORTE = (PORTE & ~(1 << PE4));	// 7seg data now 0
		PORTE |=  dataBit_i;	// 7seg data written
		
		PORTE = (PORTE | (1 << PE3));	// clock signal to 1
	}
		
	// latch
	//latch is PB7
	PORTB = PORTB | (1 << PB7);			// latch is "1"
	asm("nop");
	PORTB = PORTB & ~(1 << PB7);		// latch is "0"
	
}

//!use the right letter!
SIGNAL(TIMER1_COMPA_vect){	//right display
	if(rightData != ){
		rightData += 1;
	}else{
		rightData = 0;
	}
}

SIGNAL(TIMER2_COMPA_vect){	//left display
	if(leftData != 9){
		leftData += 1;
	}else{
		leftData = 0;
	}
}