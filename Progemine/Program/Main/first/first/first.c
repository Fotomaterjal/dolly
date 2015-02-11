/*
 * GccApplication1.c
 *
 * Created: 25.01.2015 12:03:36
 *  Author: Oll
 */ 
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000
#define CKDIV_16 1024
#define TIM_MAX_16 65536



void init_Timer1();
void setCurrentLimiter_T4(int ocr_value);
int getOSCR1A(int freq);
void init_UART();
//void setPWMForFun();

void sendString(char name[]);
void sendLetter(uint8_t letter);

int freq1 = 2;
int TCNT1_value;
int OCR1A_value;



int main(void){
	// Remove CLKDIV8
	CLKPR = 0x80;
	CLKPR = 0x00;
	// get compare match value for timer 1
	OCR1A_value = getOSCR1A(freq1);
	
	cli();
	init_Timer1();
	//TEST WITH POWER SUPPLY
	setCurrentLimiter_T4(32);	// 3,2% duty cycle
	sei();
	
	char str[] = "JOUJOU";
	
	// debug LED to output
	int led = 0x20;
	DDRD |= led;	
	//init_UART();		

	//setPWMForFun();
	
	
    while(1)
    {
        //TODO:: Please write your application code	
		//sendString(str);		
    }
}



void init_Timer1(){	
	// setting CPU clock/1024
	// 1s = 15625 cycles
	TCCR1B = (1<<CS12 | 1<<CS10 | 1<<WGM12); // enable CTC
	// set CTC value
	OCR1A = OCR1A_value;	
	
	// enable Timer Overflow Interrupt
	TIMSK1 |= (1<<OCIE1A);
	// Global Interrupt enable
	SREG |= (1<<7);	
}

// get OSCR1A value for required frequency
int getOSCR1A(int freq){
	return (F_CPU/CKDIV_16/freq);	
}

SIGNAL(TIMER1_COMPA_vect){
	TCNT1 = 0x0000;
	//PD5 = DEBUG_LED
	PORTD = PORTD^(1<<PD5);	// invert LED value
	//Step on step2 (PB5)
	//TEST WITHH POWER SUPPLY
	//PB5 = STEP2
	PORTB = PORTB^(1<<PB5);
	asm("NOP");
	PORTB = PORTB^(1<<PB5);	
	asm("NOP");
}

// current limit set
// I_tripMax = V_ref/(8*R_s), R_s = 0.1 Ohm
// 200 mA = V_ref/0.8 -> V_ref = 0.16V = 3.2% uptime -> OCR4D = 8
void setCurrentLimiter_T4(int ocr_value){
	DDRD |= (1<<PD7);			//OCR4D to output
	OCR4D = ocr_value;
	//TCCR4A = 0x00;
	//TCCR4B = 0x00;
	//TCCR4C = 0x00;
	
	TCCR4C |= (1<<COM4D1)|(1<<PWM4D);	// set fast PWM mode - clear on CM, enable PWM @ OCR4D
	TCCR4B |= (1<<CS40);
	//TCCR4D = 0x00;
	TCNT4 = 0x000;	
}

//void setCurrentLimiter_T4_2(){
	//DDRD |= (1<<PD7);
	//OCR4D = 255;
	//
//}

//void setPWMForFun(){
	////PC6 OC3A
	////WGM: 8-bit fast pwm
	//// CS: no prescale
	//// com3b1: clear oc3a on compare match
	//TCCR3B |= (1<<CS30) | (1<<WGM32);
	//TCCR3A |= (1<<WGM30) | (1<<COM3A1);
	//DDRC |= (1<<PC6);
	//OCR3A = 32000;
	//
//}


void init_UART(){
	// UART
	// setting baud rate to 9600
	UBRR1 = 207;
	// setting up double speed mode
	UCSR1A = (1 << U2X1);
	
	// set up 8 data bits
	UCSR1C |= (1<<UCSZ10) | (1<<UCSZ11);
	// put DDR UART to output
	//DDRD |= (1<<PD3);
	// enable transmitter
	UCSR1B |= (1<<TXEN1);
	
}

void sendLetter(uint8_t letter){
	while ((UCSR1A & (1 << UDRE1)) == 0) ;
	//load value to UART sending register
	UDR1 = letter;
}

void sendString(char name[]){
	for( int i = 0; i < strlen(name); i++){
		sendLetter(name[i]);
	}
}

