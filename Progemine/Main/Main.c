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
#include <stdlib.h>
#include <stdio.h>

// Define constants
#define F_CPU 16000000
#define CKDIV_16 1024
#define TIM_MAX_16 65536
#define STEPDIV 1
#define T1OCA_DIV 1024
#define MAX_KEYFRAMES 10


// Declare functions in this file
void init_Timer1(char OCR, uint16_t CTC_value);
void changeDistDir(uint8_t dir);
void changeHorRotDir(uint8_t dir);
void changeVerRotDir(uint8_t dir);

void initDistTimer(uint16_t CTC_value);	// move
void initHorTimer(uint16_t CTC_value);	// horizontal rotation
void initVerTimer(uint16_t CTC_value);	// vertical rotation
void halt_Timer1_A();					// move
void halt_Timer3_A();					// horizontal rotation
void halt_timer0_A();					// vertical rotation

void init_Timer0();
void halt_Timer0();

void setCurrentLimiter_T4(int ocr_value);
int getOSCR1A(int freq);
void init_UART();
void init_steppers();
void rmCLKDIV8();
void startLapse();
void haltLapse();
// USART functions
void sendString(char name[]);
void sendLetter(uint8_t letter);
void sendWord(uint16_t word);
struct keyFrame readKeyframe(uint8_t bits);
uint16_t USARTgetWord();
uint8_t USARTgetLetter();
uint8_t getDirection(int currentPosition, uint16_t futurePosition);
uint16_t calcOCR1A(uint16_t curTime, uint16_t curDist, uint16_t recDistance, uint16_t recTime);
uint16_t calcOCR3A(uint16_t curTime, uint16_t curHorDeg, uint16_t recHorDeg, uint16_t recTime);
uint16_t calcOCR0A(uint16_t curTime, uint16_t curVerDeg, uint16_t recVerDeg, uint16_t recTime);

// Define global variables
int freq1 = 1;
uint8_t keyFramePointer = 0;
uint8_t keyFrameReadPointer = 0;
uint8_t curKeyFramePointer = 0;
uint8_t sendPointer = 0;
uint8_t startedFlag = 0;
uint8_t timer0PreScFlag = 0;

int curDist = 0;
int curHorDeg = 0;
int curVerDeg = 0;
int curTime = 0;

uint8_t distDirFlag = 1;
uint8_t horDirFlag = 1;
uint8_t verDirFlag = 1;

uint8_t usartReceivePrevByte = 0;
uint8_t usartReceiveNowByte = 1;

uint8_t Timer0PreSc = 0;
uint8_t Timer0PreScCheck = 0;
uint8_t VerTimerPreSc = 0;

uint16_t OCR1A_value = 15625;
uint16_t OCR3A_value = 15625;
uint16_t OCR0A_value = 15625;

struct keyFrame{
	uint16_t distance;
	uint16_t horDeg;
	uint16_t vertDeg;
	uint16_t timeStamp;
};

struct keyFrame keyframes[MAX_KEYFRAMES];


int main(void){
	rmCLKDIV8();
	////////////// Blinking LED freq ///////////////////////////
	int led = 0x20;
	DDRD |= led;	
	////////////// Initialize Stepper CurrentLimit & DDRs //////
	cli();	// cancel all interrupts
	// !!!TEST WITH POWER SUPPLY!!! //
	setCurrentLimiter_T4(8);	// 8 = 3,2% duty cycle (I_tripMax = V_ref/0.8)
	init_steppers();			// PORT & DDR stuff for step and direction
	sei();	// allow interrupts	
	////////////// UART testing //////////////////////////
	init_UART();		// settings and RX TX enable stuff
	///////////// Main loop //////////////////////////////
	
    while(1){
		//sendString("joukijou");
		
		//sendString("\nStartedflag:");
		char curDists4[10];
		sprintf(curDists4, "%d", distDirFlag);
		
		// Here we plan to check when to change to the next keyFrame
		if(startedFlag == 0x01){
			//sendString("AHHHHAHHAHAHA:");
			if(curKeyFramePointer <= keyFrameReadPointer){ 
				// we are not out of keyframes yet				
				if(((curDist > (int) keyframes[curKeyFramePointer].distance && distDirFlag) ||
					(curDist < (int) keyframes[curKeyFramePointer].distance && !distDirFlag)) ||
					((curHorDeg > (int) keyframes[curKeyFramePointer].horDeg && horDirFlag) ||
					(curHorDeg < (int) keyframes[curKeyFramePointer].horDeg && !horDirFlag)) ||
					((curVerDeg > (int) keyframes[curKeyFramePointer].vertDeg && verDirFlag) ||
					(curVerDeg < (int) keyframes[curKeyFramePointer].vertDeg && !verDirFlag))){
					// time to change to the next keyFrame
					//sendString("\n3:");
					curTime = keyframes[curKeyFramePointer].timeStamp;
					curKeyFramePointer ++;
					
					////////////////	DEBUG	/////////////////
					
					
					/*char dist[10];
					sprintf(dist, "%d", keyframes[curKeyFramePointer].distance);
					sendString("Start");
					sendString("\nKFDist: ");
					sendString(dist);
					
					sendString("\nKFHorDeg:");
					char horDeg[10];
					sprintf(horDeg, "%d", keyframes[curKeyFramePointer].horDeg);
					sendString(horDeg);
					
					sendString("\nKFVerDeg:");
					char verDeg[10];
					sprintf(verDeg, "%d", keyframes[curKeyFramePointer].vertDeg);
					sendString(verDeg);
					
					sendString("\nKFTime:");
					char time[10];
					sprintf(time, "%d", keyframes[curKeyFramePointer].timeStamp);
					sendString(time);
					
					//print out mcu data
					sendString("\ncurTime:");
					char curTimes[10];
					sprintf(curTimes, "%d", curTime);
					sendString(curTimes);
					
					sendString("\ncurDist:");
					char curDists[10];
					sprintf(curDists, "%d", curDist);
					sendString(curDists);
					
					sendString("\nOCR1A:");
					char curOCR1A[10];
					sprintf(curOCR1A, "%d", OCR1A);
					sendString(curOCR1A);
					
					sendString("\ncurHorDeg:");
					char curHordegs[10];
					sprintf(curHordegs, "%d", curHorDeg);
					sendString(curHordegs);
					
					sendString("\nOCR1B:");
					char curOCR1B[10];
					sprintf(curOCR1B, "%d", OCR1B);
					sendString(curOCR1B);
					
					
					sendString("\ncurVerDeg:");
					char curVerdegs[10];
					sprintf(curVerdegs, "%d", curVerDeg);
					sendString(curVerdegs);
					
					sendString("\nOCR1C:");
					char curOCR1C[10];
					sprintf(curOCR1C, "%d", OCR1C);
					sendString(curOCR1C);
					
					sendString("\nB is curKeyFramePointer: ");
					char bact[10];
					sprintf(bact, "%d ", curKeyFramePointer);
					sendString(bact);
					
					sendString("\nB is curKeyFramePointer: ");
					char bacta[10];
					sprintf(bacta, "%d ", keyFrameReadPointer);
					sendString(bacta);
					
					//sei();
					sendString("\nTIMSK1: ");
					char reg[10];
					sprintf(reg, "%d ", TIMSK1);
					sendString(reg);
					
					sendString("\nSREG: ");
					char reg1[10];
					sprintf(reg1, "%d ", SREG);
					sendString(reg1);*/
					
					
					////////////////	DEBUG	/////////////////
					
					
					// calculate new CTC values for timers
					OCR1A_value = calcOCR1A(curTime, curDist, 
					keyframes[curKeyFramePointer].distance, keyframes[curKeyFramePointer].timeStamp);
					OCR3A_value = calcOCR3A(curTime, curHorDeg,
					keyframes[curKeyFramePointer].horDeg, keyframes[curKeyFramePointer].timeStamp);
					OCR0A_value = calcOCR0A(curTime, curVerDeg,
					keyframes[curKeyFramePointer].vertDeg, keyframes[curKeyFramePointer].timeStamp);
					// check whether user insisted on stopping any motors
					// if not then re-initialize stepper step interrupts
					if(curKeyFramePointer != 0){
						//sendString("\n5:");
						// check distance axis
						if(keyframes[curKeyFramePointer].distance == keyframes[curKeyFramePointer-1].distance){
							halt_Timer1_A();
							//sendString("\n6:");
						}else{
							if(getDirection(curDist, keyframes[curKeyFramePointer].distance) == 0x01){
								//we have to move forward
								changeDistDir(1);		//now we are moving forward
							}else{
								changeDistDir(0);		//move backwards
							}
							//sendString("\n7:");
							initDistTimer(OCR1A_value);							
						}
						// check horizontal rotation axis
						if(keyframes[curKeyFramePointer].horDeg == keyframes[curKeyFramePointer-1].horDeg){
							halt_Timer3_A();
						}else{
							if(getDirection(curHorDeg, keyframes[curKeyFramePointer].horDeg) == 0x01){
								//we have to move forward
								changeHorRotDir(1);		//now we are moving forward
							}else{
								changeHorRotDir(0);		//move backward
							}
							initHorTimer(OCR3A_value);
						}
						// check vertical rotation axis
						if(keyframes[curKeyFramePointer].vertDeg == keyframes[curKeyFramePointer-1].vertDeg){
							halt_timer0_A();
						}else{
							if(getDirection(curVerDeg, keyframes[curKeyFramePointer].vertDeg) == 0x01){
								//we have to move forward
								changeVerRotDir(1);		//now we are moving forward
							}else{
								changeVerRotDir(0);		//move backward
							}
							initVerTimer(OCR0A_value);
						}
						
					}
					
						
						/*
						if(keyframes[curKeyFramePointer].horDeg == keyframes[curKeyFramePointer-1].horDeg){
							halt_Timer3_A();
						}
						if(keyframes[curKeyFramePointer].vertDeg == keyframes[curKeyFramePointer-1].vertDeg){
							halt_timer0_A();
						}
						*/
						
				//}else{
					//haltLapse();
				//}
				}				
			}else{
				haltLapse();
				
				//sendString("\nFINAL CURDIST: ");
				//char curintdist[10];
				//sprintf(curintdist, "%d ", curDist);
				//sendString(curintdist);
				//
				//sendString("\nFINAL CURHORDEG: ");
				//char curinthor[10];
				//sprintf(curinthor, "%d ", curHorDeg);
				//sendString(curinthor);
				//
				//sendString("\nFINAL CURVERDEG: ");
				//char curintver[10];
				//sprintf(curintver, "%d ", curVerDeg);
				//sendString(curintver);
			}	
		}			
    }	
}


void startLapse(){
	cli();
	startedFlag = 0x01;
	//startedFlag = 0x01;
	
	
	// initialize all steppers
	OCR1A_value = calcOCR1A(curTime, curDist, keyframes[curKeyFramePointer].distance, keyframes[curKeyFramePointer].timeStamp);
	OCR3A_value = calcOCR3A(curTime, curHorDeg, keyframes[curKeyFramePointer].horDeg, keyframes[curKeyFramePointer].timeStamp);
	OCR0A_value = calcOCR0A(curTime, curVerDeg, keyframes[curKeyFramePointer].vertDeg, keyframes[curKeyFramePointer].timeStamp);
	
	sei();
	
	
	//sendString("\nOCR1A:");
	//char curOCR1A[10];
	//sprintf(curOCR1A, "%d", OCR1A);
	//sendString(curOCR1A);
	//
	//sendString("\nOCR3A:");
	//char curOCR1B[10];
	//sprintf(curOCR1B, "%d", OCR3A);
	//sendString(curOCR1B);
	//
	//sendString("\nOCR0A:");
	//char curOCR1C[10];
	//sprintf(curOCR1C, "%d", OCR0A);
	//sendString(curOCR1C);
	
	//sendString("\nTIMSK1:");
	//char curOCR1C2[10];
	//sprintf(curOCR1C2, "%d", TIMSK1);
	//sendString(curOCR1C2);
	
	//init_Timer1('A', OCR1A_value);
	//init_Timer1('B', OCR3A_value);
	//init_Timer1('C', OCR1C_value);
	
	initDistTimer(OCR1A_value);
	initHorTimer(OCR3A_value);
	initVerTimer(OCR0A_value);
	
	
	
	//sendString("\nOCR1A:");
	////char curOCR1A[10];
	//sprintf(curOCR1A, "%d", OCR1A);
	//sendString(curOCR1A);
	//
	//sendString("\nOCR3C:");
	////char curOCR1B[10];
	//sprintf(curOCR1B, "%d", OCR3A);
	//sendString(curOCR1B);
	//
	//sendString("\nOCR0A:");
	////char curOCR1C[10];
	//sprintf(curOCR1C, "%d", OCR0A);
	//sendString(curOCR1C);
	//
	//sendString("\nTIMSK1:");
	//char curOCR1C2[10];
	//sprintf(curOCR1C2, "%d", TIMSK1);
	//sendString(curOCR1C2);
	//
	//sendString("\n SREG:");
	//char curOCR1C3[10];
	//sprintf(curOCR1C3, "%d", SREG);
	//sendString(curOCR1C3);
	//startedFlag = 0x01;
	
}

void haltLapse(){
	//cli();
	startedFlag = 0x00;
	curKeyFramePointer = 0;
	keyFrameReadPointer = 0;	
	halt_Timer1_A();	//dist
	halt_Timer3_A();	//hor deg
	halt_timer0_A();
	//halt_Timer1_C();
	//sei();
}


//calculates needed OCR1A value to travel needed distance in given time
//linear horizontal movement
//recDistance given in steps, recTime in seconds
uint16_t calcOCR1A(uint16_t curTime, uint16_t curDist, uint16_t recDistance, uint16_t recTime){
	uint32_t absDist = (abs(recDistance - curDist))*STEPDIV;
	uint32_t absTime = (recTime-curTime);
	float wantedStepFreq = absDist/absTime;
	uint32_t result = (F_CPU/T1OCA_DIV)/wantedStepFreq;
	if(result >= 0xFFFF || result == 0x0000){
		return 0xFFFF;
	}else{
		return (uint16_t) result;
	}
}

//calculates needed OCR1B value to travel needed rotation in given time
//horizontal rotational movement
//recHorDeg given in steps, recTime in seconds
uint16_t calcOCR3A(uint16_t curTime, uint16_t curHorDeg, uint16_t recHorDeg, uint16_t recTime){
	uint32_t absHorDeg = (abs(recHorDeg - curHorDeg))*STEPDIV;
	uint32_t absTime = (recTime-curTime);
	float wantedStepFreq = absHorDeg/absTime;
	uint32_t result = (F_CPU/T1OCA_DIV)/wantedStepFreq;
	if(result >= 0xFFFF || result == 0x0000){
		return 0xFFFF;
	}else{
		return (uint16_t) result;
	}
}

//calculates needed OCR1C value to travel needed rotation in given time
//vertical rotational movement
//recVerDeg given in steps, recTime in seconds
uint16_t calcOCR0A(uint16_t curTime, uint16_t curVerDeg, uint16_t recVerDeg, uint16_t recTime){
	uint32_t absVerDeg = (abs(recVerDeg - curVerDeg))*STEPDIV;
	uint32_t absTime = (recTime-curTime);
	float wantedStepFreq = absVerDeg/absTime;
	uint32_t result = (F_CPU/T1OCA_DIV)/wantedStepFreq;
	if(result >= 0xFFFF || result == 0x0000){
		return 0xFFFF;
	}else{
		return (uint16_t) result;
	}
}

//Reads four 16-bit integers into keyframes[]
struct keyFrame readKeyframe(uint8_t bytes){
	struct keyFrame receivedKeyframe;
	for(int i=0; i<4; i++){	
		switch(i){
			case 0:
				receivedKeyframe.distance = USARTgetWord();
				break;
			case 1:
				receivedKeyframe.horDeg = USARTgetWord();
				break;
			case 2:
				receivedKeyframe.vertDeg = USARTgetWord();
				break;
			case 3:
				receivedKeyframe.timeStamp = USARTgetWord();				
				break;
		}
	}
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

void initDistTimer(uint16_t CTC_value){
	TCNT1 = 0x0000;
	TCCR1A = 0x00;
	// setting CPU clock/1024
	// 1s = 15625 cycles
	TCCR1B = (1<<CS12 | 1<<CS10 | 1<<WGM12); // enable CTC
	
	if(CTC_value != 0xFFFF){
		// set CTC value
		OCR1A = CTC_value;
		// enable Timer Overflow Interrupt for COMPA
		TIMSK1 |= (1<<OCIE1A);
	}else{
		halt_Timer1_A();
	}
}

void initHorTimer(uint16_t CTC_value){
		TCNT3 = 0x0000;
		TCCR3A = 0x00;
		// setting CPU clock/1024
		// 1s = 15625 cycles
		TCCR3B = (1<<CS32 | 1<<CS30 | 1<<WGM32); // enable CTC
		
		if(CTC_value != 0xFFFF){
			// set CTC value
			OCR3A = CTC_value;
			// enable Timer Overflow Interrupt for COMPA
			TIMSK3 |= (1<<OCIE3A);
		}else{
			halt_Timer3_A();
		}
}

void initVerTimer(uint16_t CTC_value){
	//set preScaler to 1024
	TCCR0B |= (1<<CS02 | 1<<CS00);
	//set to CTC mode
	TCCR0A |= (1<<WGM01);
	//allow CTC interrupt
	TIMSK0 |= (1<<OCIE0A);
	Timer0PreScCheck = 0x00;
	if(CTC_value == 0xFFFF){
		halt_timer0_A();
	}else if(CTC_value > 0xFF){
		timer0PreScFlag = 1;
		OCR0A = 0xFF;
		Timer0PreSc = CTC_value % 0xFF;
	}else{
		timer0PreScFlag = 0;
		OCR0A = CTC_value & 0xFF;
	}
}

void init_Timer1(char OCR, uint16_t CTC_value){
	//cli();
	TCNT1 = 0x0000;
	TCCR1A = 0x00;
	// setting CPU clock/1024
	// 1s = 15625 cycles
	TCCR1B = (1<<CS12 | 1<<CS10 | 1<<WGM12); // enable CTC
	
	
	if(CTC_value != 0xFFFF){
		switch(OCR){
			case 'A':			
				// set CTC value
				OCR1A = CTC_value;
				// enable Timer Overflow Interrupt for COMPA
				TIMSK1 |= (1<<OCIE1A);		
				break;
			case 'B':
				// set CTC value
				OCR1B = CTC_value;
				// enable Timer Overflow Interrupt for COMPA
				TIMSK1 |= (1<<OCIE1B);
				break;
			case 'C':
				// set CTC value
				OCR1C = CTC_value;
				// enable Timer Overflow Interrupt for COMPA
				TIMSK1 |= (1<<OCIE1C);		
				break;
		}	
	}
	// Global Interrupt enable
	//sei();	
}

void halt_Timer1_A(){
	// disable Timer Overflow Interrupt for COMPA
	TIMSK1 &= ~(1 << OCIE1A);
	TCNT1 = 0;	
}

void halt_Timer3_A(){
	// disable Timer Overflow Interrupt for COMPA
	TIMSK3 &= ~(1 << OCIE3A);
	TCNT3 = 0;
}

void halt_Timer1_B(){
	// disable Timer Overflow Interrupt for COMPB
	TIMSK1 &= ~(1<<OCIE1B);
	TCNT1 = 0;
	//OCR1B = 0;
}

void halt_Timer1_C(){
	// disable Timer Overflow Interrupt for COMPC
	TIMSK1 &= ~(1<<OCIE1C);
	TCNT1 = 0;
	//OCR1C = 0;
}

void halt_timer0_A(){
	TIMSK0 &= ~(1<<OCIE0A);
	TCNT0 = 0;
}

//void init_Timer0(){
	////set preScaler to 1024
	//TCCR0B = (1<<CS02 | 1<<CS00);
	////allow OVF interrupt
	//TIMSK0 = (1<<TOIE0);	
//}



// get OSCR1A value for required frequency
int getOSCR1A(int freq){
	return (F_CPU/CKDIV_16/freq);	
}

uint8_t getDirection(int currentPosition, uint16_t futurePosition){
	if((int) futurePosition < currentPosition){
		return 0x00;	//moving backwards
	}else{
		return 0x01;	//moving forward
	}
}

void changeDistDir(uint8_t dir){
	if(dir){
		// Change upper stepper DIR to FORWARD movement
		PORTD |= (1 << PD1);
		distDirFlag = 1;
	}else{
		// Change upper stepper DIR to BACKWARDS movement
		PORTD &= ~(1 << PD1);
		distDirFlag = 0;
	}	
}

void changeHorRotDir(uint8_t dir){
	if(dir){
		// Change middle stepper DIR to FORWARD movement
		PORTC |= (1 << PC7);
		horDirFlag = 1;
	}else{
		// Change middle stepper DIR to BACKWARDS movement
		PORTC &= ~(1 << PC7);
		horDirFlag = 0;
	}
}

void changeVerRotDir(uint8_t dir){
	if(dir){
		// Change upper stepper DIR to FORWARD movement
		PORTB |= (1 << PB4);
		verDirFlag = 1;
	}else{
		// Change upper stepper DIR to BACKWARDS movement
		PORTB &= ~(1 << PB4);
		verDirFlag = 0;
	}
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
	// DIR3 middle (distance) stepper
	DDRC |= (1 << PC7);
	PORTC |= (1 << PC7);
	
	// STEP1 (upper stepper) is PB6
	DDRB |= (1 << PB6);
	// DIR1 upper (distance) stepper
	DDRD |= (1 << PD1);
	PORTD |= (1 << PD1);
		
	//// Change step mode
	DDRB |= (1 << PB0) | (1 << PB1);
	PORTB |= (1 << PB0) | (1 << PB1);
}

// Bottom (vertical rotation) stepper
/*SIGNAL(TIMER1_COMPC_vect){
	//TEST WITHH POWER SUPPLY
	//PB5 = STEP2
	PORTB = PORTB^(1<<PB5);
	PORTB = PORTB^(1<<PB5);
	
	if(verDirFlag){
		curVerDeg ++;	
	}else{
		curVerDeg --;
	}
	
}*/



///////////////////////////////////////////////// STEP INTERRUPTS /////////////////////////////////////////////
// Upper (distance) stepper interrupt
SIGNAL(TIMER1_COMPA_vect){
	//Step on step2 (PB5)
	//TEST WITHH POWER SUPPLY		
	//upper stepper
	//sendString(" DistStep ");
	PORTB = PORTB^(1<<PB6);
	PORTB = PORTB^(1<<PB6);
	
	if(distDirFlag == 1){
		curDist++;
	}else{
		curDist --;	
	}
	/*if(curDist%100 == 0){
		sendString("\nCURDIST: ");
		char curintdist[10];
		sprintf(curintdist, "%d ", curDist);
		sendString(curintdist);
		
		sendString("\nDISTDIRFLAG: ");
		char curintdists[10];
		sprintf(curintdists, "%d ", distDirFlag);
		sendString(curintdists);
		
		sendString("\nCURKeyFramePointer: ");
		char curintdista[10];
		sprintf(curintdista, "%d ", curKeyFramePointer);
		sendString(curintdista);
		
		sendString("\nReadPointer: ");
		char curintdistar[10];
		sprintf(curintdistar, "%d ", keyFrameReadPointer);
		sendString(curintdistar);
				
		sendString("\nCURKeyFramePointerDistance: ");
		char curintdistas[10];
		sprintf(curintdistas, "%d ", keyframes[curKeyFramePointer].distance);
		sendString(curintdistas);
	}*/
}

// Middle (horizontal rotation) stepper
SIGNAL(TIMER3_COMPA_vect){
	//TEST WITHH POWER SUPPLY
	//PC6 = STEP3
	//sendString(" HorStep ");
	PORTC = PORTC^(1<<PC6);
	PORTC = PORTC^(1<<PC6);	
	PORTD = PORTD^(1<<PD5);	// invert LED value
	
	if(horDirFlag == 1){
		curHorDeg ++;
	}else{
		curHorDeg --;
	}
	
	//if(curHorDeg%100 == 0){
		//sendString("\nCURHORDEG: ");
		//char curinthor[10];
		//sprintf(curinthor, "%d ", curHorDeg);
		//sendString(curinthor);
	//}
}

// vertical rotation
SIGNAL(TIMER0_COMPA_vect){
	if(timer0PreScFlag){
		if(Timer0PreScCheck < Timer0PreSc){
			Timer0PreScCheck ++;
		}else{
			Timer0PreScCheck = 0;
			// do the stepping action
			//TEST WITH POWER SUPPLY
			//PB5 = STEP2
			//sendString(" VerStep ");
			PORTB = PORTB^(1<<PB5);
			PORTB = PORTB^(1<<PB5);
			if(verDirFlag){
				curVerDeg ++;
			}else{
				curVerDeg --;
			}
		}
	}else{
		// do the stepping action
		//TEST WITH POWER SUPPLY
		//PB5 = STEP2
		//sendString(" VerStep ");
		PORTB = PORTB^(1<<PB5);
		PORTB = PORTB^(1<<PB5);
		if(verDirFlag){
			curVerDeg ++;
		}else{
			curVerDeg --;
		}
	}
	
	//if(curVerDeg%100 == 0){
		//sendString("\nCURVERDEG: ");
		//char curintver[10];
		//sprintf(curintver, "%d ", curVerDeg);
		//sendString(curintver);
	//}
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

/*SIGNAL(TIMER0_OVF_vect){			
	if(TimerCounter0PreSc != 61){
		TimerCounter0PreSc++;
		//PORTD = PORTD^(1<<PD5);	// invert LED value
	}else{							// happens every second
		//PORTD = PORTD^(1<<PD5);	// invert LED value	
		
		TimerCounter0PreSc = 0;
		
		//for(int i = 0; i <= keyFrameReadPointer; i++){
			//sendString("\nKeyframe: ");
			//char curkeyFrame1[10];
			//char curkeyFrame2[10];
			//char curkeyFrame3[10];
			//char curkeyFrame4[10];
			//sprintf(curkeyFrame1, "%d ", keyframes[i].distance);
			//sprintf(curkeyFrame2, "%d ", keyframes[i].horDeg);
			//sprintf(curkeyFrame3, "%d ", keyframes[i].vertDeg);
			//sprintf(curkeyFrame4, "%d ", keyframes[i].timeStamp);
			//sendString(curkeyFrame1);
			//sendString(curkeyFrame2);
			//sendString(curkeyFrame3);
			//sendString(curkeyFrame4);
		//}
		
		//sendLetter(startedFlag);
		if(startedFlag == 0x01){
			curTime++;
			
			// time to change to next keyFrame
				char dist[10];
				sprintf(dist, "%d", keyframes[curKeyFramePointer].distance);
				sendString("Start");
				sendString("\nKFDist: ");
				sendString(dist);
				
				sendString("\nKFHorDeg:");
				char horDeg[10];
				sprintf(horDeg, "%d", keyframes[curKeyFramePointer].horDeg);
				sendString(horDeg);
				
				sendString("\nKFVerDeg:");
				char verDeg[10];
				sprintf(verDeg, "%d", keyframes[curKeyFramePointer].vertDeg);
				sendString(verDeg);
				
				sendString("\nKFTime:");
				char time[10];
				sprintf(time, "%d", keyframes[curKeyFramePointer].timeStamp);
				sendString(time);
				
				//print out mcu data
				sendString("\ncurTime:");
				char curTimes[10];
				sprintf(curTimes, "%d", curTime);
				sendString(curTimes);
				
				sendString("\ncurDist:");
				char curDists[10];
				sprintf(curDists, "%d", curDist);
				sendString(curDists);
				
				sendString("\nOCR1A:");
				char curOCR1A[10];
				sprintf(curOCR1A, "%d", OCR1A);
				sendString(curOCR1A);
				
				sendString("\ncurHorDeg:");
				char curHordegs[10];
				sprintf(curHordegs, "%d", curHorDeg);
				sendString(curHordegs);
				
				sendString("\nOCR1B:");
				char curOCR1B[10];
				sprintf(curOCR1B, "%d", OCR1B);
				sendString(curOCR1B);
				
				
				sendString("\ncurVerDeg:");
				char curVerdegs[10];
				sprintf(curVerdegs, "%d", curVerDeg);
				sendString(curVerdegs);
				
				sendString("\nOCR1C:");
				char curOCR1C[10];
				sprintf(curOCR1C, "%d", OCR1C);
				sendString(curOCR1C);
				
				sendString("\nB is curKeyFramePointer: ");
				char bact[10];
				sprintf(bact, "%d ", curKeyFramePointer);
				sendString(bact);
				
				sendString("\nB is curKeyFramePointer: ");
				char bacta[10];
				sprintf(bacta, "%d ", keyFrameReadPointer);
				sendString(bacta);
				
				//sei();
				sendString("\nTIMSK1: ");
				char reg[10];
				sprintf(reg, "%d ", TIMSK1);
				sendString(reg);
				
				sendString("\nSREG: ");
				char reg1[10];
				sprintf(reg1, "%d ", SREG);
				sendString(reg1);
				
				
				
			if(curTime >= keyframes[curKeyFramePointer].timeStamp){
				curKeyFramePointer++;	//go to next keyFrame
				// check whether we are out of keyframes
				if(curKeyFramePointer > (keyFrameReadPointer)){
					// party over, let's reset everything to its initial state
					haltLapse();
					//curKeyFramePointer = 0;
					//keyFrameReadPointer = 0;
					horDirFlag = 1;
					verDirFlag = 1;
					distDirFlag = 1;
					return;
				}
				
				//check whether user insisted on stopping any motors
				if(curKeyFramePointer != 0){
					if(keyframes[curKeyFramePointer].distance == keyframes[curKeyFramePointer-1].distance){
						halt_Timer1_A();
					}
					if(keyframes[curKeyFramePointer].horDeg == keyframes[curKeyFramePointer-1].horDeg){
						//halt_Timer1_B();
						halt_Timer3_A();
					}
					if(keyframes[curKeyFramePointer].vertDeg == keyframes[curKeyFramePointer-1].vertDeg){
						//halt_Timer1_C();
					}
				}
				 
				
				// OCR1A = linear movement
				OCR1A_value = calcOCR1A(curTime, curDist, keyframes[curKeyFramePointer].distance, keyframes[curKeyFramePointer].timeStamp);
				// OCR1B = horizontal rotational movement
				OCR3A_value = calcOCR3A(curTime, curHorDeg, keyframes[curKeyFramePointer].horDeg, keyframes[curKeyFramePointer].timeStamp);
				// OCR1C = vertical rotational movement
				OCR1C_value = calcOCR1C(curTime, curVerDeg, keyframes[curKeyFramePointer].vertDeg, keyframes[curKeyFramePointer].timeStamp);
				
				////////////////////////////////////////////////// LINEAR MOTION STUFF /////////////////////////////////////////////////
				if(OCR1A_value == 0xFFFF){		// divide by 0 aka stepper has to stop
					halt_Timer1_A();
				}else if(keyframes[curKeyFramePointer].distance == keyframes[curKeyFramePointer-1].distance && curKeyFramePointer != 0){
					// horizontal movement keyFrame has the same value, please stop					
					halt_Timer1_A();
				}else{
					if(getDirection(curDist, keyframes[curKeyFramePointer].distance) == 0x01){
						//we have to move forward
						changeDistDir(1);		//now we are moving forward
					}else{
						changeDistDir(0);		//move backwards
					}					
					initDistTimer(OCR1A_value);		//initialize timer with new OCR1A value
				}
				
				/////////////////////////////////////////////////// HORIZONTAL ROTATE STUFF //////////////////////////////////////////////
				if((OCR3A_value == 0xFFFF)){		// divide by 0 aka stepper has to stop
					halt_Timer3_A();	
				}else if(keyframes[curKeyFramePointer].horDeg == keyframes[curKeyFramePointer-1].horDeg && curKeyFramePointer != 0){
					// horizontal rotation keyFrame has the same value, please stop					
					halt_Timer1_B();
				}else{
					if(getDirection(curHorDeg, keyframes[curKeyFramePointer].horDeg) == 0x01){
						//we have to move forward
						changeHorRotDir(1);		//now we are moving forward
					}else{
						changeHorRotDir(0);		//move backward
					}
					//init_Timer1('B', OCR1B_value);	//initialize timer with new OCR1B value
					initHorTimer(OCR3A_value);
				}
				/////////////////////////////////////////////////// VERTICAL ROTATE STUFF ////////////////////////////////////////////////
				if((OCR1C_value == 0xFFFF)){		// divide by 0 aka stepper has to stop
					halt_Timer1_C();
				}else if(keyframes[curKeyFramePointer].vertDeg == keyframes[curKeyFramePointer-1].vertDeg && curKeyFramePointer != 0){
					// vertical rotation keyFrame has the same value, please stop					
					halt_Timer1_C();
				}else{
					if(getDirection(curVerDeg, keyframes[curKeyFramePointer].vertDeg) == 0x01){
						//we have to move forward
						changeVerRotDir(1);		//now we are moving forward
					}else{
						changeVerRotDir(0);		//move backward
					}
					//init_Timer1('C', OCR1C_value);	//initialize timer with new OCR1C value
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
				
			
				
				///////////////////////////////////////////////	DEBUG  ///////////////////////////////////////////////////////////////////	
				
				

					
				
							
			}	
			
			
			//if(horDirFlag){
				//sendString("  Direction is forward   ");
				//
			//}else{
				//sendString("  Direction is backward   ");
			//}
			
			
			
			///////////////////////////////////////////////	DEBUG  ///////////////////////////////////////////////////////////////////
		}		
	}
}*/



