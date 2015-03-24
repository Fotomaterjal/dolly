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
void init_Timer1_A(uint16_t CTC_value);
void init_Timer1_B(uint16_t CTC_value);
void init_Timer1_C(uint16_t CTC_value);
void changeDistDir(uint8_t dir);
void changeHorRotDir(uint8_t dir);
void changeVerRotDir(uint8_t dir);
void init_Distance_Axis(uint16_t OCR1A_intake, uint8_t direction);
void init_Timer0();
void halt_Timer1_A();
void halt_Timer1_B();
void halt_Timer1_C();
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
uint8_t getDistDir(uint16_t curDist, uint16_t futureDist);
uint8_t getHorRotDir(uint16_t curHorDeg, uint16_t futureHorDeg);
uint16_t calcOCR1A(uint16_t curTime, uint16_t curDist, uint16_t recDistance, uint16_t recTime);
uint16_t calcOCR1B(uint16_t curTime, uint16_t curHorDeg, uint16_t recHorDeg, uint16_t recTime);
uint16_t calcOCR1C(uint16_t curTime, uint16_t curVerDeg, uint16_t recVerDeg, uint16_t recTime);

// Define global variables
int freq1 = 1;
uint8_t keyFramePointer = 0;
uint8_t keyFrameReadPointer = 0;
uint8_t curKeyFramePointer = 0;
uint8_t sendPointer = 0;
uint8_t startedFlag = 0;

uint16_t curDist = 0;
uint16_t curHorDeg = 0;
uint16_t curVerDeg = 0;
uint16_t curTime = 0;

uint8_t usartReceivePrevByte = 0;
uint8_t usartReceiveNowByte = 1;

uint8_t TimerCounter0PreSc = 0;

uint16_t OCR1A_value = 15625;
uint16_t OCR1B_value = 15625;
uint16_t OCR1C_value = 15625;

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
	
	////////////// Initialize Stepper CurrentLimit & DDRs //////
	cli();
	
	init_Timer0();
	// !!!TEST WITH POWER SUPPLY!!!
	setCurrentLimiter_T4(8);	// 3,2% duty cycle
	init_steppers();			// PORT & DDR stuff for step and direction
	sei();
	
	////////////// UART testing //////////////////////////
	init_UART();		// settings and RX TX enable stuff
	
	
	// initialize timer
	OCR1A = OCR1A_value;
	// initialize 1-second CTC
	//OCR1C = 15625;
	///////////// Main loop //////////////////////////////
    while(1)
    {
		
		//if(curTime >= keyframes[curKeyFramePointer].timeStamp){
			//curKeyFramePointer++;
		//}
		//if(curKeyFramePointer >= 9){
			//stopSequence();
		//}
		
		
		
		//TODO: problem here:
		//somehow doesn't go into this if clause
		//for(uint32_t k = 0; k<1000000; k++){
			//asm("nop");
		//}
		//sendLetter(startedFlag);
		//if(startedFlag == 0x01){
			//sendLetter(0x02);
			//sendWord(keyframes[curKeyFramePointer].distance);
			//sendWord(keyframes[curKeyFramePointer].horDeg);
			//sendWord(keyframes[curKeyFramePointer].vertDeg);
			//sendWord(keyframes[curKeyFramePointer].timeStamp);
			//
			//
			//
			//sendWord(OCR1A);
			//if(OCR1A != OCR1A_value){
			//
			//}
		//}
		//
		
		
		
		
		
		//for(uint32_t k = 0; k<2000000; k++){
			//asm("nop");	
		//}
		//// Debugging
		//sendWord(keyframes[sendPointer].distance);
		//sendWord(keyframes[sendPointer].horDeg);
		//sendWord(keyframes[sendPointer].vertDeg);
		//sendWord(keyframes[sendPointer].timeStamp);
		//
		//if (sendPointer < 9){
			//sendPointer++;
		//}else{
			//sendPointer = 0;
			//sendLetter(0xFF);
			//sendLetter(0xFF);
			//sendLetter(0xFF);
			//sendLetter(0xFF);
		//}
		
    }
	
}


void startLapse(){
	startedFlag = 0x01;
	// init linear motion timer
	
	OCR1A_value = calcOCR1A(curTime, curDist, keyframes[curKeyFramePointer].distance, keyframes[curKeyFramePointer].timeStamp);
	//OCR1A = OCR1A_value;
	init_Timer1_A(OCR1A_value);
}


//calculates needed OCR1A value to travel needed distance in given time
//linear horizontal movement
//recDistance given in steps, recTime in seconds
uint16_t calcOCR1A(uint16_t curTime, uint16_t curDist, uint16_t recDistance, uint16_t recTime){
	float wantedStepFreq = ((recDistance-curDist)*STEPDIV)/(recTime-curTime);
	TCNT1 = 0x00;
	uint16_t wat = (F_CPU/T1OCA_DIV)/wantedStepFreq;
	return wat;	
}

//calculates needed OCR1B value to travel needed rotation in given time
//horizontal rotational movement
//recHorDeg given in steps, recTime in seconds
uint16_t calcOCR1B(uint16_t curTime, uint16_t curHorDeg, uint16_t recHorDeg, uint16_t recTime){
	float wantedStepFreq = ((recHorDeg-curHorDeg)*STEPDIV)/(recTime-curTime);
	TCNT1 = 0x00;
	uint16_t wat = (F_CPU/T1OCA_DIV)/wantedStepFreq;
	return wat;
}

//calculates needed OCR1C value to travel needed rotation in given time
//vertical rotational movement
//recVerDeg given in steps, recTime in seconds
uint16_t calcOCR1C(uint16_t curTime, uint16_t curVerDeg, uint16_t recVerDeg, uint16_t recTime){
	float wantedStepFreq = ((recVerDeg-curVerDeg)*STEPDIV)/(recTime-curTime);
	TCNT1 = 0x00;
	uint16_t wat = (F_CPU/T1OCA_DIV)/wantedStepFreq;
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
				//sendWord(receivedKeyframe.distance);
				break;
			case 1:
				//sendLetter('c');
				receivedKeyframe.horDeg = USARTgetWord();
				//sendWord(receivedKeyframe.horDeg);
				break;
			case 2:
				//sendLetter('d');				
				receivedKeyframe.vertDeg = USARTgetWord();
				//sendWord(receivedKeyframe.vertDeg);
				break;
			case 3:
				//sendLetter('e');				
				receivedKeyframe.timeStamp = USARTgetWord();				
				//sendWord(receivedKeyframe.timeStamp);
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
void init_Timer1_A(uint16_t CTC_value){	
	TCNT1 = 0x0000;
	TCCR1A = 0x00;
	// setting CPU clock/1024
	// 1s = 15625 cycles
	TCCR1B = (1<<CS12 | 1<<CS10 | 1<<WGM12); // enable CTC
	// set CTC value
	OCR1A = CTC_value;	
	// enable Timer Overflow Interrupt for COMPA
	TIMSK1 |= (1<<OCIE1A);
	// Global Interrupt enable
	SREG |= (1<<7);	
}

void init_Timer1_B(uint16_t CTC_value){
	TCNT1 = 0x0000;
	TCCR1A = 0x00;
	// setting CPU clock/1024
	// 1s = 15625 cycles
	TCCR1B = (1<<CS12 | 1<<CS10 | 1<<WGM12); // enable CTC
	// set CTC value
	OCR1B = CTC_value;
	// enable Timer Overflow Interrupt for COMPA
	TIMSK1 |= (1<<OCIE1B);
	// Global Interrupt enable
	SREG |= (1<<7);
}

void init_Timer1_C(uint16_t CTC_value){
	TCNT1 = 0x0000;
	TCCR1A = 0x00;
	// setting CPU clock/1024
	// 1s = 15625 cycles
	TCCR1B = (1<<CS12 | 1<<CS10 | 1<<WGM12); // enable CTC
	// set CTC value
	OCR1C = CTC_value;
	// enable Timer Overflow Interrupt for COMPA
	TIMSK1 |= (1<<OCIE1C);
	// Global Interrupt enable
	SREG |= (1<<7);
}

void halt_Timer1_A(){
	//TCNT1 = 0x0000;
	// disable Timer Overflow Interrupt for COMPA
	TIMSK1 |= (0<<OCIE1A);
}

void halt_Timer1_B(){
	//TCNT1 = 0x0000;
	// disable Timer Overflow Interrupt for COMPB
	TIMSK1 |= (0<<OCIE1B);
}

void halt_Timer1_C(){
	//TCNT1 = 0x0000;
	// disable Timer Overflow Interrupt for COMPA
	TIMSK1 |= (0<<OCIE1C);
}

void init_Timer0(){
	//set mode CTC
	//TCCR0A = (1<<WGM01);
	//set preScaler to 1024
	TCCR0B = (1<<CS02 | 1<<CS00);
	//allow OVF interrupt
	TIMSK0 = (1<<TOIE0);	
}

// get OSCR1A value for required frequency
int getOSCR1A(int freq){
	return (F_CPU/CKDIV_16/freq);	
}

//returns future direction
uint8_t getDistDir(uint16_t curDist, uint16_t futureDist){
	if(futureDist < curDist){
		return 0x00;	//moving backwards
	}else{
		return 0x01;	//moving forward
	}
}

//returns future direction
uint8_t getHorRotDir(uint16_t curHorDeg, uint16_t futureHorDeg){
	if(futureHorDeg < curHorDeg){
		return 0x00;	//moving backwards
	}else{
		return 0x01;	//moving forward
	}
}


void changeDistDir(uint8_t dir){
	if(dir){
		// Change upper stepper DIR to FORWARD movement
		DDRD |= (1 << PD1);
	}else{
		// Change upper stepper DIR to BACKWARD movement
		DDRD &= ~(1 << PD1);
	}	
}

void changeHorRotDir(uint8_t dir){
	if(dir){
		// Change middle stepper DIR to FORWARD movement
		DDRC |= (1 << PD7);
		}else{
		// Change middle stepper DIR to BACKWARD movement
		DDRC &= ~(1 << PD7);
	}
}

void changeVerRotDir(uint8_t dir){
	if(dir){
		// Change upper stepper DIR to FORWARD movement
		DDRB |= (1 << PB4);
		}else{
		// Change upper stepper DIR to BACKWARD movement
		DDRB &= ~(1 << PB4);
	}
}

void init_Distance_Axis(uint16_t OCR1A_intake, uint8_t direction){
	changeDistDir(direction);
	init_Timer1_A(OCR1A_intake);
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
	curDist ++;
	//upper stepper
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
	//sendLetter(firstByte);
	uint8_t secondByte;
	
	if (firstByte == 'k'){	//gonna be receiving a keyFrame, aka four 16bit numbers		
		keyframes[keyFrameReadPointer] = readKeyframe(2);	
	}else if(firstByte == 's'){	//receiving some metadata (exactly which keyFrame is coming)
		secondByte = USARTgetLetter();
		keyFrameReadPointer = secondByte;
	}else if(firstByte == 'z'){
		startLapse();
	}
}

SIGNAL(TIMER0_OVF_vect){			
	if(TimerCounter0PreSc != 61){
		TimerCounter0PreSc++;
	}else{							// happens every second
		//PORTD = PORTD^(1<<PD5);	// invert LED value	
		TimerCounter0PreSc = 0;
		//sendLetter(startedFlag);
		if(startedFlag == 0x01){
			curTime++;
			sendLetter(0x02);
			sendWord(keyframes[curKeyFramePointer].distance);
			sendWord(keyframes[curKeyFramePointer].horDeg);
			sendWord(keyframes[curKeyFramePointer].vertDeg);
			sendWord(keyframes[curKeyFramePointer].timeStamp);
			sendLetter(0xFF);
			sendWord(OCR1A);
			// time to change to next keyFrame
			if(curTime >= keyframes[curKeyFramePointer].timeStamp){
				curKeyFramePointer++;	//go to next keyframe
				// OCR1A = linear movement
				OCR1A_value = calcOCR1A(curTime, curDist, keyframes[curKeyFramePointer].distance, keyframes[curKeyFramePointer].timeStamp);
				// OCR1B = rotational movement
				OCR1B_value = calcOCR1B(curTime, curHorDeg, keyframes[curKeyFramePointer].horDeg, keyframes[curKeyFramePointer].timeStamp);
				// OCR1C = vertical rotational movemnt
				OCR1B_value = calcOCR1C(curTime, curVerDeg, keyframes[curKeyFramePointer].vertDeg, keyframes[curKeyFramePointer].timeStamp);
				
				////////////////////////////////////////////////// LINEAR MOTION STUFF /////////////////////////////////////////////////
				if(OCR1A_value == 0xFFFF){		// divide by 0 aka stepper has to stop
					halt_Timer1_A();
				}else{
					if(getDistDir(curDist, keyframes[curKeyFramePointer].distance) != 0x01){
						//we have to move backwards
						changeDistDir(0);		//now we are moving backwards
					}else{
						changeDistDir(1);		//move forward
					}
					init_Timer1_A(OCR1A_value);	//initialize timer with new OCR1A value
				}
				
				///////////////////////////////////////////////// HORIZONTAL ROTATE STUFF //////////////////////////////////////////////
				if(OCR1B_value == 0xFFFF){		// divide by 0 aka stepper has to stop
					halt_Timer1_B();
				}else{
					if(getHorRotDir(curHorDeg, keyframes[curKeyFramePointer].horDeg) != 0x01){
						//we have to move backwards
						changeHorRotDir(0);		//now we are moving backwards
					}else{
						changeHorRotDir(1);		//move forward
					}
					init_Timer1_B(OCR1B_value);	//initialize timer with new OCR1A value
				}
				///////////////////////////////////////////////// VERTICAL ROTATE STUFF ////////////////////////////////////////////////
				if(OCR1B_value == 0xFFFF){		// divide by 0 aka stepper has to stop
					halt_Timer1_C();
				}else{
					if(getHorRotDir(curVerDeg, keyframes[curKeyFramePointer].vertDeg) != 0x01){
						//we have to move backwards
						changeVerRotDir(0);		//now we are moving backwards
					}else{
						changeVerRotDir(1);		//move forward
					}
					init_Timer1_C(OCR1C_value);	//initialize timer with new OCR1A value
				}
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////							
			}						
		}		
	}	
	PORTD = PORTD^(1<<PD5);	// invert LED value	every second
}