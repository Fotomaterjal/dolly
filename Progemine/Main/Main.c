/*
 * GccApplication1.c
 *
 * Created: 25.01.2015 12:03:36
 *  Author: Oll
 */ 
// Include headers
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Define constants
#define F_CPU 16000000
#define CKDIV_16 1024
#define TIM_MAX_16 65536
#define STEPDIV 1
#define T1OCA_DIV 1024

// Declare functions in this file
void init_Timer1();
void setCurrentLimiter_T4(int ocr_value);
int getOSCR1A(int freq);
void init_UART();
void init_steppers();
void rmCLKDIV8();
// USART functions
void sendString(char name[]);
void sendLetter(uint8_t letter);
void sendWord(uint16_t word);
struct keyFrame readKeyframe(uint8_t bits);
uint16_t USARTgetWord();
uint16_t calcOCR1A(uint16_t recDistance, uint16_t recTime);

// Define global variables
int freq1 = 1;
uint8_t keyFramePointer = 0;
uint8_t keyFrameReadPointer = 0;
int OCR1A_value;
uint16_t curDist = 0;
uint16_t curHorDeg = 0;
uint16_t curVerDeg = 0;
uint16_t curTime = 0;


struct keyFrame{
	uint16_t distance;
	uint16_t horDeg;
	uint16_t vertDeg;
	uint16_t timeStamp;
};

struct keyFrame keyframes[10];


int main(void){

	rmCLKDIV8();
	////////////// Blinking LED freq /////////////////////
	// get compare match value for timer 1
	OCR1A_value = getOSCR1A(freq1);
	// debug LED to output
	int led = 0x20;
	DDRD |= led;
	
	////////////// Init Stepper CurrentLimit & DDRs //////
	cli();
	init_Timer1();
	// !!!TEST WITH POWER SUPPLY!!!
	setCurrentLimiter_T4(8);	// 3,2% duty cycle
	init_steppers();			// PORT & DDR stuff for step and direction
	sei();
	
	////////////// UART testing //////////////////////////
	char str[] = "JOUJOU";
	init_UART();		// settings and RX TX enable stuff

	OCR1A = 15625;
	///////////// Main loop //////////////////////////////
    while(1)
    {
		for(uint32_t k = 0; k<2000000; k++){
			asm("nop");
			//sendLetter('k');
			
		}
		sendLetter(0xFF);
		//sendWord(OCR1A);
		sendWord(keyframes[keyFrameReadPointer].distance);
		sendWord(keyframes[keyFrameReadPointer].horDeg);
		sendWord(keyframes[keyFrameReadPointer].vertDeg);
		sendWord(keyframes[keyFrameReadPointer].timeStamp);
		
		
		if (keyFrameReadPointer < 10){
			keyFrameReadPointer++;
		}else{
			keyFrameReadPointer = 0;
			sendLetter(0xFF);
			sendLetter(0xFF);
			sendLetter(0xFF);
			sendLetter(0xFF);
		}
		
		//
		//sendWord(keyframes[0].distance);	// send confirmation
		////sendWord(keyframes[0].horDeg);	// send confirmation
		////sendWord(keyframes[0].vertDeg);	// send confirmation
		//sendWord(keyframes[0].timeStamp);	// send confirmation
		////sendLetter('o');
		//
		//if(keyframes[0].distance != '\0\0'){
			//OCR1A = calcOCR1A(keyframes[0].distance, keyframes[0].timeStamp);
			//TCNT1 = 0x00;
		//}
    }
	
}

uint16_t calcOCR1A(uint16_t recDistance, uint16_t recTime){
	float wantedStepFreq = (recDistance*STEPDIV)/recTime;
	TCNT1 = 0x00;
	uint16_t wat = (F_CPU/T1OCA_DIV)/wantedStepFreq;
	sendLetter('u');
	sendWord(wat);
	return wat;	
}

struct keyFrame readKeyframe(uint8_t bytes){
	struct keyFrame receivedKeyframe;
	for(int i=0; i<4; i++){	
		//sendLetter('a');	
		
		switch(i){
			case 0:
				sendLetter('b');
				receivedKeyframe.distance = USARTgetWord();
				sendWord(receivedKeyframe.distance);
				break;
			case 1:
				sendLetter('c');
				receivedKeyframe.horDeg = USARTgetWord();
				sendWord(receivedKeyframe.horDeg);
				break;
			case 2:
				sendLetter('d');				
				receivedKeyframe.vertDeg = USARTgetWord();
				sendWord(receivedKeyframe.vertDeg);
				break;
			case 3:
				sendLetter('e');				
				receivedKeyframe.timeStamp = USARTgetWord();				
				sendWord(receivedKeyframe.timeStamp);
				break;
		}
	}
	//sendLetter('f');	
	return receivedKeyframe;
}


uint16_t USARTgetWord(){
	uint8_t receivedByte;
	uint16_t receivedWord = 0;		//init variable	

	while(!(UCSR1A & (1<<RXC1)));	//while receive not complete
	receivedByte = UDR1;
	receivedWord = (receivedByte<<8);		//fill the high byte	
	while(!(UCSR1A & (1<<RXC1)));	//while receive not complete
	receivedByte = UDR1;
	receivedWord |= receivedByte;			//fill the low byte
	return receivedWord;
}

void init_Timer1(){	
	TCCR1A = 0x00;
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


// current limit set, TIMER4 PWM
// I_tripMax = V_ref/(8*R_s), R_s = 0.1 Ohm
// 200 mA = V_ref/0.8 -> V_ref = 0.16V = 3.2% uptime -> OCR4D = 8
void setCurrentLimiter_T4(int ocr_value){
	DDRD |= (1<<PD7);					//OCR4D to output
	OCR4D = ocr_value;					// set count limit
	TCCR4C |= (1<<COM4D1)|(1<<PWM4D);	// set fast PWM mode - clear on CM, enable PWM @ OCR4D
	TCCR4B |= (1<<CS40);				// set CPU clock for timer clock
	TCNT4 = 0x000;						// init count
}

void init_UART(){
	// UART
	// setting baud rate to 9600
	UBRR1 = 207;
	// setting up double speed mode
	UCSR1A = (1 << U2X1);	
	// set up 8 data bits
	UCSR1C |= (1<<UCSZ10) | (1<<UCSZ11);
	// enable transmitter, receiever, receive complete interrupt
	UCSR1B |= (1<<TXEN1 | 1 << RXEN1 | 1 << RXCIE1);
	
}

void sendLetter(uint8_t letter){
	// while last value not yet cleared
	while ((UCSR1A & (1 << UDRE1)) == 0) ;
	// else load value to UART sending register
	UDR1 = letter;
}

void sendWord(uint16_t word){
	// while last value not yet cleared
	while ((UCSR1A & (1 << UDRE1)) == 0) ;
	// else load value to UART sending register
	UDR1 = (word>>8) & 0xFF;
	// while last value not yet cleared
	while ((UCSR1A & (1 << UDRE1)) == 0) ;
	// else load value to UART sending register
	UDR1 = word;
}

void sendString(char name[]){
	for( int i = 0; i < strlen(name); i++){
		sendLetter(name[i]);
	}
}

void rmCLKDIV8(){
	// Remove CLKDIV8
	CLKPR = 0x80;
	CLKPR = 0x00;
}

void init_steppers(){
	// STEP2 (bottom stepper) is PB5
	DDRB |= (1 << PB5);
	// DIR2
	DDRB |= (1 << PB4);
	PORTB |= (1 << PB4);
	// STEP3 (middle stepper) is PC6
	DDRC |= (1 << PC6);
	// STEP1 (upper stepper) is PB6
	DDRB |= (1 << PB6);
	// Change upper stepper DIR
	DDRD |= (1 << PD1);
	
	//// Change step mode
	//DDRB |= (1 << PB0) | (1 << PB1);
	//PORTB |= (1 << PB0) | (1 << PB1);
	
	
}

SIGNAL(TIMER1_COMPA_vect){
	//TCNT1 = 0x0000;
	//PD5 = DEBUG_LED
	PORTD = PORTD^(1<<PD5);	// invert LED value
	//Step on step2 (PB5)
	//TEST WITHH POWER SUPPLY
	//PB5 = STEP2
	//PORTB = PORTB^(1<<PB5);
	//asm("NOP");
	//asm("NOP");
	//asm("NOP");
	//PORTB = PORTB^(1<<PB5);
	//asm("NOP");
	//asm("NOP");
	//asm("NOP");
	
	//TEST WITHH POWER SUPPLY
	//PC6 = STEP3
	//PORTC = PORTC^(1<<PC6);
	//asm("NOP");
	//asm("NOP");
	//asm("NOP");
	//PORTC = PORTC^(1<<PC6);
	//asm("NOP");
	//asm("NOP");
	//asm("NOP");

	//TEST WITHH POWER SUPPLY
	//PB6 = STEP1
	//PORTD ^= (1 << PD1);
	PORTB = PORTB^(1<<PB6);
	asm("NOP");
	asm("NOP");
	asm("NOP");
	PORTB = PORTB^(1<<PB6);
	asm("NOP");
	asm("NOP");
	asm("NOP");
}

SIGNAL(USART1_RX_vect){
	uint8_t firstBit = UDR1;	
	uint16_t OCR1A_value_here;
	if(firstBit == 0x6B){	//gonna be receiving a keyframe, aka four 16bit numbers
		//sendLetter('s');
		keyframes[keyFramePointer] = readKeyframe(2);
		if(keyFramePointer < 10){
			keyFramePointer ++;	
		}else{
			keyFramePointer = 0;
		}
		
		
		OCR1A_value_here = calcOCR1A(keyframes[0].distance, keyframes[0].timeStamp);
		OCR1A = OCR1A_value_here;
		//	sendWord(OCR1A_value_here);
		//sendWord(keyframes[0].distance);
		//sendWord(keyframes[0].horDeg);
		//sendWord(keyframes[0].vertDeg);
		//sendWord(keyframes[0].timeStamp);
	}
	//sendLetter('s');
	//sendLetter(firstBit);
	//OCR1A = firstBit * 100;
}