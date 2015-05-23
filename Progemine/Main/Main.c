/*
 * GccApplication1.c
 *
 * Created: 25.01.2015 12:03:36
 *  Author: Oll
 */ 

// REMEMBER, KIDS:
// int is 16-bit signed
// short is 16-bit signed
// long is 32-bit signed

// Include headers
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
//#include <float.h> 

// Define constants
#define F_CPU 16000000UL
#define CKDIV_16 1024
#define TIM_MAX_16 65536UL
#define STEPDIV 1
#define T1OCA_DIV 1
#define MAX_KEYFRAMES 10
#define DIST_WHEEL_CIRCUMFERENCE 4.8		// in centimeters
#define STEPS_IN_ROTATION_DIV16 16576.1
#define STEPS_IN_DEGREE_DIV16 46.05
#define TIMER_16BIT_ITERATIONS 65536UL
#define TIMER_8BIT_ITERATIONS 256UL
#define TIMER0_DIV 256
#define MAX_MOVE_FREQUECY 5000.0


/////////////////// Distance related /////////////////////
void initDistTimer(uint32_t CTC_value);
void changeDistDir(uint8_t dir);
void halt_Timer1_A();				
uint32_t calcOCR1A(int32_t curTime, int32_t curDist, uint16_t recDistance, uint16_t recTime);
uint32_t calcReqDistSteps(uint16_t recDistance);
uint8_t getDistDirection(int32_t currentPosition, uint16_t futurePosition);
//Define timer additional counters
uint16_t distTimerTOCounterMax = 0;
uint16_t distTimerTOCounter = 0;
uint8_t distTimerTOCounterFlag = 0;
uint8_t distTimerActiveFlag = 0;
//////////////////////////////////////////////////////////


//////////// Horizontal rotation related /////////////////
void initHorTimer(uint32_t CTC_value);
void changeHorRotDir(uint8_t dir);
void halt_Timer3_A();
uint32_t calcOCR3A(int32_t curTime, int32_t curHorDeg, uint16_t recHorDeg, uint16_t recTime);
uint32_t calcReqHorSteps(uint16_t recHorDeg);
uint8_t getHorDirection(int32_t currentDegreeSteps, uint16_t futurePosition);
//Define timer additional counters
uint16_t horTimerTOCounterMax = 0;
uint16_t horTimerTOCounter = 0;
uint8_t horTimerTOCounterFlag = 0;
uint8_t horTimerActiveFlag = 0;
/////////////////////////////////////////////////////////


///////////// Vertical rotation related /////////////////
void initVerTimer(uint32_t CTC_value);
void changeVerRotDir(uint8_t dir);
void halt_timer0_A();
uint32_t calcOCR0A(int32_t curTime, int32_t curVerDeg, uint16_t recVerDeg, uint16_t recTime);
uint32_t calcReqVerSteps(uint16_t recVerDeg);
uint8_t getVerDirection(int32_t currentDegreeSteps, uint16_t futurePosition);
//Define timer additional counters
uint16_t verTimerTOCounterMax = 0;
uint16_t verTimerTOCounter = 0;
uint8_t verTimerTOCounterFlag = 0;
uint8_t verTimerActiveFlag = 0;
/////////////////////////////////////////////////////////


///////// Start position changing related //////////////
uint32_t calcMovementOCRnA(uint8_t speedPercentage);
uint32_t calcMovementOCR0A(uint8_t speedPercentage);
void moveDevice(uint32_t receivedMoveData);
////////////////////////////////////////////////////////


/////////// UART stuff /////////////////////////////////
void init_UART();
uint16_t USARTgetWord();
uint8_t USARTgetLetter();
uint32_t USARTgetDoubleWord();
void sendString(char name[]);
void sendLetter(uint8_t letter);
void sendWord(uint16_t word);
struct moveData readMoveData();	//// TODO: use this function instead of the other one maybe
struct keyFrame readKeyframe();
////////////////////////////////////////////////////////


////////////////// Housekeeping ///////////////////////
void setCurrentLimiter_T4(int ocr_value);
void init_steppers();
void rmCLKDIV8();
///////////////////////////////////////////////////////


//////////////////// Main workers /////////////////////
void receiveAllData();
void drive();
void startLapse();
void haltLapse();
///////////////////////////////////////////////////////


// Define global variables
uint8_t keyFrameReadIndex = 0;
uint8_t curKeyFrameIndex = 0;

int32_t curDist = 0;
int32_t curHorDeg = 0;
int32_t curVerDeg = 0;
int32_t curTime = 0;

uint8_t distDirFlag = 1;
uint8_t horDirFlag = 1;
uint8_t verDirFlag = 1;

uint32_t OCR1A_value = 15625;
uint32_t OCR3A_value = 15625;
uint32_t OCR0A_value = 15625;

struct keyFrame{
	uint16_t distance;
	uint16_t horDeg;
	uint16_t vertDeg;
	uint16_t timeStamp;
};

struct moveData{
	uint8_t whichTimer;
	uint8_t onOff;
	uint8_t moveSpeed;
	uint8_t moveDirection;
};

struct keyFrame keyframes[MAX_KEYFRAMES];


int main(void){
	rmCLKDIV8();
	////////////// Blinking LED freq ///////////////////////////
	int led = 0x20;
	DDRD |= led;	
	////////////// Initialize Stepper CurrentLimit & DDRs //////
	cli();						// cancel all interrupts
	setCurrentLimiter_T4(20);	// 8 = 3,2% duty cycle (I_tripMax = V_ref/0.8)
	init_steppers();			// PORT & DDR stuff for step and direction
	sei();						// allow interrupts	
	////////////// UART testing //////////////////////////
	init_UART();				// settings and RX TX enable stuff
	///////////// Main loop //////////////////////////////	
	while(1){
		receiveAllData();		// halt till all data is received from phone
		drive();				// start the sequence
    }	
}


void startLapse(){
	cli();	
	// initialize all steppers
	OCR1A_value = calcOCR1A(curTime, curDist, keyframes[curKeyFrameIndex].distance, keyframes[curKeyFrameIndex].timeStamp);
	OCR3A_value = calcOCR3A(curTime, curHorDeg, keyframes[curKeyFrameIndex].horDeg, keyframes[curKeyFrameIndex].timeStamp);
	OCR0A_value = calcOCR0A(curTime, curVerDeg, keyframes[curKeyFrameIndex].vertDeg, keyframes[curKeyFrameIndex].timeStamp);

	
	sendString("\n\rOCR1A:");
	char curOCR1A[10];
	sprintf(curOCR1A, "%ld", OCR1A_value);
	sendString(curOCR1A);
	
	sendString("\n\rOCR3A:");
	char curOCR1B[10];
	sprintf(curOCR1B, "%ld", OCR3A_value);
	sendString(curOCR1B);
	
	sendString("\n\rOCR0A:");
	char curOCR1C[10];
	sprintf(curOCR1C, "%ld", OCR0A_value);
	sendString(curOCR1C);
	
	distDirFlag = 1;
	horDirFlag = 1;
	verDirFlag = 1;
	
	changeDistDir(1);
	changeHorRotDir(1);
	changeVerRotDir(1);
	
	
	initDistTimer(OCR1A_value);
	initHorTimer(OCR3A_value);
	initVerTimer(OCR0A_value);
	
	sei();
	/*
	sendString("\n\rcurTime:");
	char curTimes[10];
	sprintf(curTimes, "%d", curTime);
	sendString(curTimes);
	
	sendString("\n\rcurDist:");
	char curDists[10];
	sprintf(curDists, "%d", curDist);
	sendString(curDists);
	
	sendString("\n\rOCR1A:");
	char curOCR1A[10];
	sprintf(curOCR1A, "%d", OCR1A);
	sendString(curOCR1A);
	
	sendString("\n\rcurHorDeg:");
	char curHordegs[10];
	sprintf(curHordegs, "%d", curHorDeg);
	sendString(curHordegs);
	
	sendString("\n\rOCR3A:");
	char curOCR1B[10];
	sprintf(curOCR1B, "%d", OCR3A);
	sendString(curOCR1B);
	
	
	sendString("\n\rcurVerDeg:");
	char curVerdegs[10];
	sprintf(curVerdegs, "%d", curVerDeg);
	sendString(curVerdegs);
	
	sendString("\n\rverTimerPreSc:");
	char curVerdegs1[10];
	sprintf(curVerdegs1, "%d", verTimerTOCounterMax);
	sendString(curVerdegs1);
	
	sendString("\n\rOCR0A:");
	char curOCR1C[10];
	sprintf(curOCR1C, "%d", OCR0A);
	sendString(curOCR1C);
	
	sendString("\n\rB is curKeyFrameIndex: ");
	char bact[10];
	sprintf(bact, "%d ", curKeyFrameIndex);
	sendString(bact);
	
	sendString("\n\rB is curKeyFrameIndex: ");
	char bacta[10];
	sprintf(bacta, "%d ", keyFrameReadIndex);
	sendString(bacta);
	
	sei();*/
	//sendString("\n\rOCR1A:");
	////char curOCR1A[10];
	//sprintf(curOCR1A, "%d", OCR1A);
	//sendString(curOCR1A);
	//
	//sendString("\n\rOCR3C:");
	////char curOCR1B[10];
	//sprintf(curOCR1B, "%d", OCR3A);
	//sendString(curOCR1B);
	//
	//sendString("\n\rOCR0A:");
	////char curOCR1C[10];
	//sprintf(curOCR1C, "%d", OCR0A);
	//sendString(curOCR1C);
	//
	//sendString("\n\rTIMSK1:");
	////char curOCR1C2[10];
	//sprintf(curOCR1C2, "%d", TIMSK1);
	//sendString(curOCR1C2);
	////
	//sendString("\n\r SREG:");
	//char curOCR1C3[10];
	//sprintf(curOCR1C3, "%d", SREG);
	//sendString(curOCR1C3);
}

void haltLapse(){
	cli();
	sendString("\n\rcurTime:");
	char curTimes[10];
	sprintf(curTimes, "%ld", curTime);
	sendString(curTimes);
	
	sendString("\n\rcurDist:");
	char curDists[10];
	sprintf(curDists, "%ld", curDist);
	sendString(curDists);
	
	sendString("\n\rcurHorDeg:");
	char curHordegs[10];
	sprintf(curHordegs, "%ld", curHorDeg);
	sendString(curHordegs);
	
	sendString("\n\rcurVerDeg:");
	char curVerdegs[10];
	sprintf(curVerdegs, "%ld", curVerDeg);
	sendString(curVerdegs);
	
	sei();
	
	/*char dist[10];
	sprintf(dist, "%d", keyframes[curKeyFrameIndex].distance);
	sendString("\n\rStart!!!!!!!!!!!!!!!!!!!!!!!!");
	sendString("\n\rKFDist: ");
	sendString(dist);
	
	sendString("\n\rKFHorDeg:");
	char horDeg[10];
	sprintf(horDeg, "%d", keyframes[curKeyFrameIndex].horDeg);
	sendString(horDeg);
	
	sendString("\n\rKFVerDeg:");
	char verDeg[10];
	sprintf(verDeg, "%d", keyframes[curKeyFrameIndex].vertDeg);
	sendString(verDeg);
	
	sendString("\n\rKFTime:");
	char time[10];
	sprintf(time, "%d", keyframes[curKeyFrameIndex].timeStamp);
	sendString(time);
	sei();*/
	//cli();
	
	halt_Timer1_A();
	halt_Timer3_A();
	halt_timer0_A();
	
	curKeyFrameIndex = 0;
	keyFrameReadIndex = 0;
	curDist = 0;
	curHorDeg = 0;
	curVerDeg = 0;
	curTime = 0;
	
	distTimerTOCounterMax = 0;
	distTimerTOCounter = 0;
	distTimerTOCounterFlag = 0;
	distDirFlag = 1;
	
	horTimerTOCounterMax = 0;
	horTimerTOCounter = 0;
	horTimerTOCounterFlag = 0;
	horDirFlag = 1;
	
	verTimerTOCounterMax = 0;
	verTimerTOCounter = 0;
	verTimerActiveFlag = 0;
	verDirFlag = 1;
	for(int i = 0; i < MAX_KEYFRAMES; i++){
		keyframes[i].distance = 0;
		keyframes[i].horDeg = 0;
		keyframes[i].vertDeg = 0;
		keyframes[i].timeStamp = 0;
	}
}

void receiveAllData(){
	uint8_t firstByte = 0;
	
	while(firstByte != 'z'){			// start command not received		
		firstByte = USARTgetLetter();
		uint8_t secondByte;
		
		if (firstByte == 'k'){	//gonna be receiving a keyFrame, aka four 16bit numbers
			keyframes[keyFrameReadIndex] = readKeyframe();
		}else if(firstByte == 's'){	//receiving some metadata (exactly which keyFrame is coming)
			secondByte = USARTgetLetter();
			keyFrameReadIndex = secondByte;
		}else if(firstByte == 'm'){ //gonna move initial position
			uint32_t receivedMoveData = 0;
			receivedMoveData = USARTgetDoubleWord();
			moveDevice(receivedMoveData);
		}
	}
	
}

uint32_t calcReqDistSteps(uint16_t recDistance){
	double reqRotations = (double)recDistance/(double)DIST_WHEEL_CIRCUMFERENCE;
	uint32_t neededSteps = (uint32_t) ((double)reqRotations*(double)STEPS_IN_ROTATION_DIV16);
	return neededSteps;
}

uint32_t calcReqHorSteps(uint16_t recHorDeg){
	uint32_t neededSteps = (uint32_t) ((double)recHorDeg * (double)STEPS_IN_DEGREE_DIV16);
	return neededSteps;
}

uint32_t calcReqVerSteps(uint16_t recVerDeg){
	uint32_t neededSteps = (uint32_t) ((double)recVerDeg * (double)STEPS_IN_DEGREE_DIV16);
	return neededSteps;
}

uint32_t calcMovementOCRnA(uint8_t speedPercentage){
	float wantedStepFrequency = (float) MAX_MOVE_FREQUECY * 0.01 * (float) speedPercentage;
	uint32_t result = (uint32_t) ((((float) F_CPU)/ ((float) T1OCA_DIV))/wantedStepFrequency);
	if(result >= 0xFFFFFFFF || result == 0x00000000){
		return 0xFFFFFFFF;
	}else{
		return result;
	}
}

uint32_t calcMovementOCR0A(uint8_t speedPercentage){
	float wantedStepFrequency = (float) MAX_MOVE_FREQUECY * 0.01 * (float) speedPercentage;
	uint32_t result = (uint32_t) ((((float) F_CPU)/ ((float) TIMER0_DIV))/wantedStepFrequency);
	if(result >= 0xFFFFFFFF || result == 0x00000000){
		return 0xFFFFFFFF;
	}else{
		return result;
	}
}

//calculates needed OCR1A value to travel needed distance in given time
//linear horizontal movement
//recDistance given in centimeters, recTime in seconds
uint32_t calcOCR1A(int32_t curTime, int32_t curDist, uint16_t recDistance, uint16_t recTime){
    // convert centimeters to steps
	double reqRotationsSinceStart = ((double) recDistance)/((double) DIST_WHEEL_CIRCUMFERENCE);
    uint32_t neededStepsSinceStart = (uint32_t) (reqRotationsSinceStart*STEPS_IN_ROTATION_DIV16);
     
    uint16_t givenTime = abs(recTime-(uint16_t)curTime);
	uint32_t neededSteps;	
	if(curDist > (int32_t)neededStepsSinceStart){
		neededSteps = (uint32_t) (curDist- (int32_t)neededStepsSinceStart);
	}else{
		neededSteps = (uint32_t) ((int32_t) neededStepsSinceStart - curDist);
	}
	double wantedStepFreq = (double) ((double) neededSteps)/(double) givenTime;	
	
	//neededSteps = (uint32_t) abs((int32_t)curDist - (int32_t)neededStepsSinceStart);
    uint32_t result = (uint32_t) ((((double) F_CPU)/ ((double) T1OCA_DIV))/wantedStepFreq);
	
	
		//sendString("\n\rNeedeSteps:");
		//char curocr1c2[10];
		//sprintf(curocr1c2, "%ld", neededStepsSinceStart);
		//sendString(curocr1c2);
		//
		//sendString("\n\rCurDist:");
		//char curocr1c23[10];
		//sprintf(curocr1c23, "%ld", curDist);
		//sendString(curocr1c23);
		//
		//sendString("\n\rNeedeD!:");
		//char curocr1c2311[10];
		//sprintf(curocr1c2311, "%ld", neededSteps);
		//sendString(curocr1c2311);
		//
		//sendString("\n\rGivenTime:");
		//char curocr1c234[10];
		//sprintf(curocr1c234, "%d", givenTime);
		//sendString(curocr1c234);
		//
		//sendString("\n\rResult:");
		//char curocr1c22[10];
		//sprintf(curocr1c22, "%ld", result);
		//sendString(curocr1c22);
		//
		//sendString("\n\rWantedStepFreq:");
		//char curocr1c42[10];
		//sprintf(curocr1c42, "%ld", (uint32_t)wantedStepFreq);
		//sendString(curocr1c42);
	
	
    if(result >= 0xFFFFFFFF || result == 0x00000000){
        return 0xFFFFFFFF;
    }else{
        return result;
    }
}

//calculates needed OCR1B value to travel needed rotation in given time
//horizontal rotational movement
//recHorDeg given in steps, recTime in seconds
uint32_t calcOCR3A(int32_t curTime, int32_t curHorDeg, uint16_t recHorDeg, uint16_t recTime){
	//convert degrees to steps
	double stepsSinceStart = (double)recHorDeg * (double)STEPS_IN_DEGREE_DIV16;
	//uint32_t neededHorSteps = (int32_t)stepsSinceStart - (int32_t)curHorDeg);
	
	
	uint16_t givenTime = (recTime-(uint16_t)curTime);
	uint32_t neededSteps;
	if(curHorDeg > (int32_t) stepsSinceStart){
		neededSteps = (curHorDeg - (int32_t)stepsSinceStart);
	}else{
		neededSteps = (uint32_t) ((int32_t) stepsSinceStart - curHorDeg);
	}
	
	double wantedStepFreq = (double) ((double)neededSteps / (double)givenTime);
	uint32_t result = (uint32_t) ((((double) F_CPU)/ ((double) T1OCA_DIV))/wantedStepFreq);
	if(result >= 0xFFFFFFFF || result == 0x00000000){
		return 0xFFFFFFFF;
	}else{
		return result;
	}
}

//calculates needed OCR1C value to travel needed rotation in given time
//vertical rotational movement
//recVerDeg given in steps, recTime in seconds
uint32_t calcOCR0A(int32_t curTime, int32_t curVerDeg, uint16_t recVerDeg, uint16_t recTime){
	//convert degrees to steps
	double stepsSinceStart = (double)recVerDeg * (double)STEPS_IN_DEGREE_DIV16;
	//uint32_t neededVerSteps = (uint32_t) abs((int32_t)stepsSinceStart - (int32_t)curVerDeg);
	
	
		
	uint16_t givenTime = (recTime-(uint16_t)curTime);
	uint32_t neededSteps;
	if(curVerDeg > (int32_t) stepsSinceStart){
		neededSteps = (uint32_t) (curVerDeg - (int32_t) stepsSinceStart);
	}else{
		neededSteps = (uint32_t) ((int32_t) stepsSinceStart - curVerDeg);
	}
	
	double wantedStepFreq = (double) ((double)neededSteps / (double)givenTime);
	uint32_t result = (uint32_t) roundf(((((float) F_CPU)/ ((float) TIMER0_DIV))/wantedStepFreq));
	

	
	
	
	
	
	if(result >= 0xFFFFFFFF || result == 0x00000000){
		return 0xFFFFFFFF;
	}else{
		return result;
	}
}

//Reads four 16-bit integers into keyframes[]
struct keyFrame readKeyframe(){
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

struct moveData readMoveData(){
	struct moveData receivedData;
	for(int i = 0; i < 4; i++){
		switch(i){
			case 0:
				receivedData.whichTimer = USARTgetLetter();
				break;
			case 1:
				receivedData.onOff = USARTgetLetter();
				break;
			case 2:
				receivedData.moveSpeed = USARTgetLetter();
				break;
			case 3:
				receivedData.moveDirection = USARTgetLetter();
				break;
		}
	}
	return receivedData;
}

uint32_t USARTgetDoubleWord(){
	uint8_t receivedByte;
	uint32_t receivedDoubleWord = 0;		//initialize variable

	while(!(UCSR1A & (1<<RXC1)));	//while receive not complete
	receivedByte = UDR1;
	receivedDoubleWord |= (((uint32_t)receivedByte)<<24);		//fill the high byte
	//sendString("amhere0");
	while(!(UCSR1A & (1<<RXC1)));	//while receive not complete
	receivedByte = UDR1;
	receivedDoubleWord |= (((uint32_t)receivedByte)<<16);		//fill the high byte
	//sendString("amhere1");
	while(!(UCSR1A & (1<<RXC1)));	//while receive not complete
	receivedByte = UDR1;
	receivedDoubleWord |= (((uint32_t)receivedByte)<<8);		//fill the high byte
	//sendString("amhere2");
	while(!(UCSR1A & (1<<RXC1)));	//while receive not complete
	receivedByte = UDR1;
	receivedDoubleWord |= receivedByte;			//fill the low byte
	//sendString("amhere3");
	return receivedDoubleWord;
}

//Reads one 16-bit integer from USART
uint16_t USARTgetWord(){
	uint8_t receivedByte;
	uint16_t receivedWord = 0;			//initialize variable	

	while(!(UCSR1A & (1<<RXC1)));		//while receive not complete
	receivedByte = UDR1;
	receivedWord = (receivedByte<<8);	//fill the high byte	
	while(!(UCSR1A & (1<<RXC1)));		//while receive not complete
	receivedByte = UDR1;
	receivedWord |= receivedByte;		//fill the low byte
	return receivedWord;
}

//Reads one byte from USART
uint8_t USARTgetLetter(){
	uint8_t receivedByte;
	while(!(UCSR1A & (1<<RXC1)));		//while receive not complete
	receivedByte = UDR1;
	return receivedByte;
}

void initDistTimer(uint32_t CTC_value){
	
	TCNT1 = 0x0000;
    TCCR1A = 0x00;
    // setting CPU clock, no preScalers
	// enable CTC
    TCCR1B = (1<<CS10 | 1<<WGM12); 
	
	if(CTC_value == 0xFFFFFFFF){
		halt_Timer1_A();
	}else if(CTC_value > 0xFFFF){
		distTimerTOCounterFlag = 1;
		distTimerTOCounter = 0;
		
		distTimerTOCounterMax = (uint16_t) ceil((float)CTC_value / (float)TIMER_16BIT_ITERATIONS);
		uint16_t tooMuch = (uint16_t) abs((distTimerTOCounterMax*TIMER_16BIT_ITERATIONS) - CTC_value);
		uint16_t toSubtract = tooMuch / distTimerTOCounterMax;	// rounding could improve accuracy	
		OCR1A = 65535-toSubtract;	
		// enable Timer Compare Match Interrupt for COMPA
        TIMSK1 |= (1<<OCIE1A);
		distTimerActiveFlag = 1;
	}else{
		distTimerTOCounterFlag = 0;
        OCR1A = CTC_value & 0xFFFF;
		// enable Timer Compare Match Interrupt for COMPA
        TIMSK1 |= (1<<OCIE1A);
		distTimerActiveFlag = 1;
	}
	//sendString("\n\rOCR1A:");
	//char curOCR1A[10];
	//sprintf(curOCR1A, "%d", OCR1A);
	//sendString(curOCR1A);
	//sendString("\n\rCurDist:");
	//char curocr1c2[10];
	//sprintf(curocr1c2, "%ld", curDist);
	//sendString(curocr1c2);
}

void initHorTimer(uint32_t CTC_value){
	
	//
	
	//

	TCNT3 = 0x0000;
	TCCR3A = 0x00;
	// setting CPU clock, no preScalers
	// enable CTC
	TCCR3B = (1<<CS30 | 1<<WGM32);
		
	if(CTC_value == 0xFFFFFFFF){
		halt_Timer3_A();
	}else if(CTC_value > 0xFFFF){
		horTimerTOCounterFlag = 1;
		horTimerTOCounter = 0;
		// calculate so interrupts will not be very often, but precise enough
		horTimerTOCounterMax = (uint16_t) ceil((float)CTC_value / (float)TIMER_16BIT_ITERATIONS);
		uint16_t tooMuch = (uint16_t) abs(((uint32_t)horTimerTOCounterMax*TIMER_16BIT_ITERATIONS) - CTC_value);
		uint16_t toSubtract = tooMuch / horTimerTOCounterMax;	// rounding could improve accuracy
		OCR3A = 65535-toSubtract;
			
		TIMSK3 |= (1<<OCIE3A);
		horTimerActiveFlag = 1;
	}else{
		horTimerTOCounterFlag = 0;
		OCR3A = CTC_value & 0xFFFF;
		// enable Timer CTC Interrupt for COMPA
		TIMSK3 |= (1<<OCIE3A);
		horTimerActiveFlag = 1;
	}
	//sendString("\n\rOCR3C:");
	//char curOCR1B[10];
	//sprintf(curOCR1B, "%d", OCR3A);
	//sendString(curOCR1B);
	//sendString("\n\rCurHorDeg:");
	//char curocr1c2[10];
	//sprintf(curocr1c2, "%ld", curHorDeg);
	//sendString(curocr1c2);
}

void initVerTimer(uint32_t CTC_value){
	// set CPU clock/256
	TCNT0 = 0;
	TCCR0B |= 1<<CS02;
	//set to CTC mode
	TCCR0A |= (1<<WGM01);
	if(CTC_value == 0xFFFFFFFF){
		halt_timer0_A();
	}else if(CTC_value > (uint32_t)0xFF){
		verTimerTOCounterFlag = 1;
		verTimerTOCounter = 0;		
		
		verTimerTOCounterMax = (uint16_t) ceil((float)CTC_value / (float)TIMER_8BIT_ITERATIONS);
		uint16_t tooMuch = (uint16_t) abs(((uint32_t)verTimerTOCounterMax*TIMER_8BIT_ITERATIONS) - CTC_value);
		uint8_t toSubtract = tooMuch / verTimerTOCounterMax;	// rounding could improve accuracy
		OCR0A = 255-toSubtract;
		
		//allow CTC interrupt
		TIMSK0 |= (1<<OCIE0A);
		verTimerActiveFlag = 1;
	}else{
		verTimerTOCounterFlag = 0;
		OCR0A = CTC_value & 0xFF;
		//allow CTC interrupt
		TIMSK0 |= (1<<OCIE0A);
		verTimerActiveFlag = 1;
	}
	//sendString("\n\rocr0a:");
	//char curocr1c[10];
	//sprintf(curocr1c, "%d", OCR0A);
	//sendString(curocr1c);
	//sendString("\n\rCurVerDeg:");
	//char curocr1c2[10];
	//sprintf(curocr1c2, "%ld", curVerDeg);
	//sendString(curocr1c2);
}


void halt_Timer1_A(){
	// disable Timer Overflow Interrupt for COMPA
	TIMSK1 &= ~(1 << OCIE1A);
	distTimerActiveFlag = 0;
	TCNT1 = 0;	
}

void halt_Timer3_A(){
	// disable Timer Overflow Interrupt for COMPA
	TIMSK3 &= ~(1 << OCIE3A);
	horTimerActiveFlag = 0;
	TCNT3 = 0;
}

void halt_timer0_A(){
	TIMSK0 &= ~(1<<OCIE0A);
	verTimerActiveFlag = 0;
	TCNT0 = 0;
}



uint8_t getDistDirection(int32_t currentPosition, uint16_t futurePosition){
	double reqRotationsSinceStart = ((double)futurePosition/(double)DIST_WHEEL_CIRCUMFERENCE);
    uint32_t neededStepsSinceStart = (uint32_t) ((double)reqRotationsSinceStart*16457.143);
    if((int32_t) neededStepsSinceStart < currentPosition){
        return 0x00;    //moving backwards
    }else{
        return 0x01;    //moving forward
    }
}


uint8_t getHorDirection(int32_t currentDegreeSteps, uint16_t futurePosition){
	uint32_t neededStepsSinceStart = (uint32_t) ((float)futurePosition * (float)STEPS_IN_DEGREE_DIV16);
	if((int32_t) neededStepsSinceStart < currentDegreeSteps){
		return 0x00;    //moving backwards
	}else{
		return 0x01;    //moving forward
	}
}

uint8_t getVerDirection(int32_t currentDegreeSteps, uint16_t futurePosition){
	uint32_t neededStepsSinceStart = (uint32_t) abs((float)futurePosition * (float)STEPS_IN_DEGREE_DIV16);
	if((int32_t) neededStepsSinceStart < currentDegreeSteps){
		return 0x00;    //moving backwards
	}else{
		return 0x01;    //moving forward
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
		//PORTB &= ~(1 << PB4);
		verDirFlag = 1;
	}else{
		// Change upper stepper DIR to BACKWARDS movement
		PORTB &= ~(1 << PB4);
		//PORTB |= (1 << PB4);
		verDirFlag = 0;
	}
}

void moveDevice(uint32_t receivedMoveData){
	struct moveData myMoveData;
	myMoveData.whichTimer =		(receivedMoveData >> 24);
	myMoveData.onOff =			(receivedMoveData >> 16);
	myMoveData.moveSpeed =		(receivedMoveData >> 8);
	myMoveData.moveDirection =	receivedMoveData;			
	switch (myMoveData.whichTimer){
		case 1:
		if(myMoveData.onOff == 0x01){
			uint32_t CRC_value = calcMovementOCRnA(myMoveData.moveSpeed);					
			if(myMoveData.moveDirection){
				changeDistDir(1);
			}else{
				changeDistDir(0);
			}
			initDistTimer(CRC_value);
		}else{
			halt_Timer1_A();
			distDirFlag = 1;
			curDist=0;
		}
		break;
	case 2:
		if(myMoveData.onOff == 0x01){
			uint32_t CRC_value = calcMovementOCRnA(myMoveData.moveSpeed);
			if(myMoveData.moveDirection){
				changeHorRotDir(1);
			}else{
				changeHorRotDir(0);
			}
			initHorTimer(CRC_value);
		}else{
			halt_Timer3_A();
			horDirFlag = 1;
			curHorDeg= 0;
		}
		break;
	case 3:
		if(myMoveData.onOff == 0x01){
			uint32_t CRC_value = calcMovementOCR0A(myMoveData.moveSpeed);
			if(myMoveData.moveDirection){
				changeVerRotDir(1);
			}else{
				changeVerRotDir(0);
			}
			initVerTimer(CRC_value);
		}else{
			halt_timer0_A();
			verDirFlag = 1;
			curVerDeg= 0;
		}
		break;
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
	TCNT4 = 0x000;						// initialize count
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
	UCSR1B |= (1<<TXEN1 | 1 << RXEN1);	
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

void drive(){
	sendLetter('p');
	startLapse();
	
	while(1){
			//if(curDist%1000 == 0){
				//sendString("\n\rCURDist: ");
				//char curintdist[10];
				//sprintf(curintdist, "%ld ", curDist);
				//sendString(curintdist);
			//}
			
			//if(curHorDeg%1000 == 0){
				//sendString("\n\rrCURHorDeg: ");
				//char curintdist5513[10];
				//sprintf(curintdist5513, "%ld ", curHorDeg);
				//sendString(curintdist5513);
			//}
			
			if(curVerDeg%1000 == 0){
				sendString("\n\rCURVERDEG: ");
				char curintver[10];
				sprintf(curintver, "%ld ", curVerDeg);
				sendString(curintver);
			}
			
			
		if(curKeyFrameIndex <= keyFrameReadIndex){
			// we are not out of keyframes yet
			if(((curDist > (int32_t) calcReqDistSteps(keyframes[curKeyFrameIndex].distance)
				&& distTimerActiveFlag && distDirFlag) ||
				(curDist < (int32_t) calcReqDistSteps(keyframes[curKeyFrameIndex].distance)
				&& distTimerActiveFlag && !distDirFlag)) ||
				((curHorDeg > (int32_t) calcReqHorSteps(keyframes[curKeyFrameIndex].horDeg)
				&& horTimerActiveFlag && horDirFlag) ||
				(curHorDeg < (int32_t) calcReqHorSteps(keyframes[curKeyFrameIndex].horDeg)
				&& horTimerActiveFlag && !horDirFlag)) ||
				((curVerDeg > (int32_t) calcReqVerSteps(keyframes[curKeyFrameIndex].vertDeg)
				&& verTimerActiveFlag && verDirFlag) ||
				(curVerDeg < (int32_t) calcReqVerSteps(keyframes[curKeyFrameIndex].vertDeg)
				&& verTimerActiveFlag && !verDirFlag))){
		
	
			
				curTime = keyframes[curKeyFrameIndex].timeStamp;
				//sendString("\n\rCURTIME: ");
				//char curocr1c2[10];
				//sprintf(curocr1c2, "%ld", curTime);
				//sendString(curocr1c2);
				curKeyFrameIndex ++;
				cli();
				// calculate new CTC values for timers
				OCR1A_value = calcOCR1A(curTime, curDist,
				keyframes[curKeyFrameIndex].distance, keyframes[curKeyFrameIndex].timeStamp);
				OCR3A_value = calcOCR3A(curTime, curHorDeg,
				keyframes[curKeyFrameIndex].horDeg, keyframes[curKeyFrameIndex].timeStamp);
				OCR0A_value = calcOCR0A(curTime, curVerDeg,
				keyframes[curKeyFrameIndex].vertDeg, keyframes[curKeyFrameIndex].timeStamp);
				// check whether user insisted on stopping any motors
				// if not then re-initialize stepper step interrupts
				if(curKeyFrameIndex != 0){
					// check distance axis
					if(keyframes[curKeyFrameIndex].distance == keyframes[curKeyFrameIndex-1].distance){
						halt_Timer1_A();
					}else{
						if(getDistDirection(curDist, keyframes[curKeyFrameIndex].distance) == 0x01){
							//we have to move forward
							changeDistDir(1);		//now we are moving forward
						}else{
							changeDistDir(0);		//move backwards
						}
						initDistTimer(OCR1A_value);
					}
					// check horizontal rotation axis
					if(keyframes[curKeyFrameIndex].horDeg == keyframes[curKeyFrameIndex-1].horDeg){
						halt_Timer3_A();
					}else{
						if(getHorDirection(curHorDeg, keyframes[curKeyFrameIndex].horDeg) == 0x01){
							//we have to move forward
							changeHorRotDir(1);		//now we are moving forward
						}else{
							changeHorRotDir(0);		//move backward
						}
						initHorTimer(OCR3A_value);
					}
					// check vertical rotation axis
					if(keyframes[curKeyFrameIndex].vertDeg == keyframes[curKeyFrameIndex-1].vertDeg){
						halt_timer0_A();
					}else{
						if(getVerDirection(curVerDeg, keyframes[curKeyFrameIndex].vertDeg) == 0x01){
							//we have to move forward
							changeVerRotDir(1);		//now we are moving forward
						}else{
							changeVerRotDir(0);		//move backward
						}
						initVerTimer(OCR0A_value);
					}				
				}
				sei();		
			}	
		}else{
			haltLapse();
			return;
		}
		
	}
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

///////////////////////////////////////////////// STEP INTERRUPTS /////////////////////////////////////////////
// Upper (distance) stepper interrupt
SIGNAL(TIMER1_COMPA_vect){
    //Step on step2 (PB5), upper stepper
    //TEST WITH POWER SUPPLY      
	if(distTimerTOCounterFlag){
		if(distTimerTOCounter < (distTimerTOCounterMax-1)){
			distTimerTOCounter++;
		}else{
			distTimerTOCounter = 0;
			// step on that, man
			PORTB = PORTB^(1<<PB6);
			PORTB = PORTB^(1<<PB6);
			// where am I going?
			if(distDirFlag){
				curDist++;
			}else{
				curDist --; 
			}			
			//sendString("\n\rCURDist: ");
			//char curintdist55[10];
			//sprintf(curintdist55, "%ld ", curDist);
			//sendString(curintdist55);
		}
	}else{
		// step on that, man. not using preScalers, man
		PORTB = PORTB^(1<<PB6);
		PORTB = PORTB^(1<<PB6);
		// where am I going?
		if(distDirFlag){
			curDist++;
		}else{
			curDist --; 
		}
	}
	
	//if(curDist%1000 == 0){
		//sendString("\n\rCURDist: ");
		//char curintdist[10];
		//sprintf(curintdist, "%ld ", curDist);
		//sendString(curintdist);
	//}
}

// Middle (horizontal rotation) stepper
SIGNAL(TIMER3_COMPA_vect){
	//TEST WITHH POWER SUPPLY
	//PC6 = STEP3
	PORTD = PORTD^(1<<PD5);	// invert LED value	
    //Step on step2 (PB5), upper stepper
    //TEST WITH POWER SUPPLY
    if(horTimerTOCounterFlag){
	    if(horTimerTOCounter < (horTimerTOCounterMax-1)){
			horTimerTOCounter++;
		}else{
		    horTimerTOCounter = 0;
		    // step on that, man
		    PORTC = PORTC^(1<<PC6);
		    PORTC = PORTC^(1<<PC6);
		    // where am I going?
		    if(horDirFlag){
			    curHorDeg++;
			}else{
			    curHorDeg --;
		    }
			//sendString("\n\rrCURHorDeg: ");
			//char curintdist551[10];
			//sprintf(curintdist551, "%ld ", curHorDeg);
			//sendString(curintdist551);
	    }
	}else{
	    // step on that, man. not using preScalers, man
	    PORTC = PORTC^(1<<PC6);
	    PORTC = PORTC^(1<<PC6);
	    // where am I going?
	    if(horDirFlag){
		    curHorDeg++;
		}else{
		    curHorDeg --;
	    }
		
		//if(curHorDeg%1000 == 0){
			//sendString("\n\rrCURHorDeg: ");
			//char curintdist5513[10];
			//sprintf(curintdist5513, "%ld ", curHorDeg);
			//sendString(curintdist5513);
		//}
    }
}

// vertical rotation
SIGNAL(TIMER0_COMPA_vect){
	if(verTimerTOCounterFlag){
		if(verTimerTOCounter < (verTimerTOCounterMax-1)){
			verTimerTOCounter ++;
		}else{
			verTimerTOCounter = 0;
			// do the stepping action
			//TEST WITH POWER SUPPLY
			//PB5 = STEP2
			PORTB = PORTB^(1<<PB5);
			PORTB = PORTB^(1<<PB5);
			if(verDirFlag){
				curVerDeg ++;
			}else{
				curVerDeg --;
			}		
			//if(curVerDeg%1000 == 0){
				//sendString("\n\rCURVERDEG: ");
				//char curintver[10];
				//sprintf(curintver, "%ld ", curVerDeg);
				//sendString(curintver);
			//}	
		}
	}else{
		// do the stepping action
		//TEST WITH POWER SUPPLY
		//PB5 = STEP2
		PORTB = PORTB^(1<<PB5);
		PORTB = PORTB^(1<<PB5);
		if(verDirFlag){
			curVerDeg ++;
		}else{
			curVerDeg --;
		}
	}
	
	//if(curVerDeg%1000 == 0){
		//sendString("\n\rCURVERDEG: ");
		//char curintver[10];
		//sprintf(curintver, "%ld ", curVerDeg);
		//sendString(curintver);
	//}
}




