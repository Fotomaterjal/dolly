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
void startLapse();
// USART functions
void sendString(char name[]);
void sendLetter(uint8_t letter);
void sendWord(uint16_t word);
struct keyFrame readKeyframe(uint8_t bits);
uint16_t USARTgetWord();
uint8_t USARTgetLetter();
uint16_t calcOCR1A(uint16_t recDistance, uint16_t recTime);

// Define global variables
int freq1 = 1;
uint8_t keyFramePointer = 0;
uint8_t keyFrameReadPointer = 0;
uint8_t sendPointer = 0;
int OCR1A_value;
uint16_t curDist = 0;
uint16_t curHorDeg = 0;
uint16_t curVerDeg = 0;
uint16_t curTime = 0;

uint8_t usartReceivePrevByte = 0;
uint8_t usartReceiveNowByte = 1;


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
		}
		// Debugging
		sendWord(keyframes[sendPointer].distance);
		sendWord(keyframes[sendPointer].horDeg);
		sendWord(keyframes[sendPointer].vertDeg);
		sendWord(keyframes[sendPointer].timeStamp);
		
		if (sendPointer < 9){
			sendPointer++;
		}else{
			sendPointer = 0;
			sendLetter(0xFF);
			sendLetter(0xFF);
			sendLetter(0xFF);
			sendLetter(0xFF);
		}
		
    }
	
}


//calculates needed OCR1A value to travel needed distance in given time
//linear horizontal movement
//recDistance given in steps, recTime in seconds
uint16_t calcOCR1A(uint16_t recDistance, uint16_t recTime){
	float wantedStepFreq = (recDistance*STEPDIV)/recTime;
	TCNT1 = 0x00;
	uint16_t wat = (F_CPU/T1OCA_DIV)/wantedStepFreq;
	sendLetter('u');
	sendWord(wat);
	return wat;	
}


//Reads four 16-bit integers into keyframes[]
struct keyFrame readKeyframe(uint8_t bytes){
	struct keyFrame receivedKeyframe;
	for(int i=0; i<4; i++){	

		switch(i){
			case 0:
				//sendLetter('b');
				receivedKeyframe.distance = USARTgetWord();
				sendWord(receivedKeyframe.distance);
				break;
			case 1:
				//sendLetter('c');
				receivedKeyframe.horDeg = USARTgetWord();
				sendWord(receivedKeyframe.horDeg);
				break;
			case 2:
				//sendLetter('d');				
				receivedKeyframe.vertDeg = USARTgetWord();
				sendWord(receivedKeyframe.vertDeg);
				break;
			case 3:
				//sendLetter('e');				
				receivedKeyframe.timeStamp = USARTgetWord();				
				sendWord(receivedKeyframe.timeStamp);
				break;
		}
	}
	//sendLetter('f');	
	return receivedKeyframe;
}


//Reads one 16-bit integer from USART
uint16_t USARTgetWord(){
	uint8_t receivedByte;
	uint16_t receivedWord = 0;		//initialize variable	

	while(!(UCSR1A & (1<<RXC1)));	//while receive not complete
	receivedByte = UDR1;
	receivedWord = (receivedByte<<8);		//fill the high byte	
	while(!(UCSR1A & (1<<RXC1)));	//while receive not complete
	receivedByte = UDR1;
	receivedWord |= receivedByte;			//fill the low byte
	return receivedWord;
}


//Reads one byte from USART
uint8_t USARTgetLetter(){
	uint8_t receivedByte;
	while(!(UCSR1A & (1<<RXC1)));	//while receive not complete
	receivedByte = UDR1;
	return receivedByte;
}

//Initialize Timer1
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

//Initialize USART
void init_UART(){
	// UART
	// setting baud rate to 9600
	UBRR1 = 207;
	// setting up double speed mode
	UCSR1A = (1 << U2X1);	
	// set up 8 data bits
	UCSR1C |= (1<<UCSZ10) | (1<<UCSZ11);
	// enable transmitter, receiver, receive complete interrupt
	UCSR1B |= (1<<TXEN1 | 1 << RXEN1 | 1 << RXCIE1);	
}

//Send a byte over USART
void sendLetter(uint8_t letter){
	// while last value not yet cleared
	while ((UCSR1A & (1 << UDRE1)) == 0) ;
	// else load value to UART sending register
	UDR1 = letter;
}


//Send a word over USART
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

//Send a string over USART
void sendString(char name[]){
	for( int i = 0; i < strlen(name); i++){
		sendLetter(name[i]);
	}
}

// Remove CLKDIV8
void rmCLKDIV8(){	
	CLKPR = 0x80;
	CLKPR = 0x00;
}


//Initialize stepper motor outputs (step&direction&mode)
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

//Interrupt, got data from HC-06 module
SIGNAL(USART1_RX_vect){
	uint8_t firstByte = USARTgetLetter();
	uint8_t secondByte;
	
	if (firstByte == 'k'){	//gonna be receiving a keyframe, aka four 16bit numbers
		
		keyframes[keyFrameReadPointer] = readKeyframe(2);
	
	}else if(firstByte == 's'){	//receiving some metadata (exactly which keyFrame is coming)
		secondByte = USARTgetLetter();
		keyFrameReadPointer = secondByte;
	}else if(firstByte == 'z'){
		startLapse();
	}
}