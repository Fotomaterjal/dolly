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
#define F_CPU 16000000
#define CKDIV_16 1024
#define TIM_MAX_16 65536
#define STEPDIV 1
#define T1OCA_DIV 1
#define MAX_KEYFRAMES 10
#define DIST_WHEEL_CIRCUMFERENCE 4.8		// in centimeters
#define STEPS_IN_ROTATION_DIV16 16457.143
#define STEPS_IN_DEGREE_DIV16 45.714
#define TIMER_16BIT_ITERATIONS 65536
#define TIMER_8BIT_ITERATIONS 256
#define TIMER0_DIV 256


/////////////////// Distance related /////////////////////
void initDistTimer(uint32_t CTC_value);
void changeDistDir(uint8_t dir);
void halt_Timer1_A();				
uint32_t calcOCR1A(long curTime, long curDist, uint16_t recDistance, uint16_t recTime);
uint32_t calcReqDistSteps(uint16_t recDistance);
uint8_t getDistDirection(long currentPosition, uint16_t futurePosition);
//Define timer preScalers
uint16_t distTimerPreSc = 0;
uint16_t distTimerPreScCounter = 0;
uint8_t distTimerPreScFlag = 0;
uint8_t distTimerActiveFlag = 0;
//////////////////////////////////////////////////////////


//////////// Horizontal rotation related /////////////////
void initHorTimer(uint32_t CTC_value);
void changeHorRotDir(uint8_t dir);
void halt_Timer3_A();
uint32_t calcOCR3A(long curTime, long curHorDeg, uint16_t recHorDeg, uint16_t recTime);
uint32_t calcReqHorSteps(uint16_t recHorDeg);
uint8_t getHorDirection(long currentDegreeSteps, uint16_t futurePosition);
//Define timer preScalers
uint16_t horTimerPreSc = 0;
uint16_t horTimerPreScCounter = 0;
uint8_t horTimerPreScFlag = 0;
uint8_t horTimerActiveFlag = 0;
/////////////////////////////////////////////////////////



///////////// Vertical rotation related /////////////////
void initVerTimer(uint32_t CTC_value);
void changeVerRotDir(uint8_t dir);
void halt_timer0_A();
uint32_t calcOCR0A(long curTime, long curVerDeg, uint16_t recVerDeg, uint16_t recTime);
uint32_t calcReqVerSteps(uint16_t recVerDeg);
uint8_t getVerDirection(long currentDegreeSteps, uint16_t futurePosition);
//Define timer preScalers
uint16_t verTimerPreSc = 0;
uint16_t verTimerPreScCounter = 0;
uint8_t verTimerPreScFlag = 0;
uint8_t verTimerActiveFlag = 0;






void init_Timer1(char OCR, uint16_t CTC_value);










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




// Define global variables
int freq1 = 1;
uint8_t keyFramePointer = 0;
uint8_t keyFrameReadPointer = 0;
uint8_t curKeyFramePointer = 0;
uint8_t sendPointer = 0;
uint8_t startedFlag = 0;
uint8_t timer0PreScFlag = 0;

long curDist = 0;
long curHorDeg = 0;
long curVerDeg = 0;
long curTime = 0;

uint8_t distDirFlag = 1;
uint8_t horDirFlag = 1;
uint8_t verDirFlag = 1;

uint8_t usartReceivePrevByte = 0;
uint8_t usartReceiveNowByte = 1;

uint32_t OCR1A_value = 15625;
uint32_t OCR3A_value = 15625;
uint32_t OCR0A_value = 15625;

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
		char curDists4[10];
		sprintf(curDists4, "%d", distDirFlag);
		
		// Here we plan to check when to change to the next keyFrame
		if(startedFlag == 0x01){
			//sendString("AHHHHAHHAHAHA:");
			if(curKeyFramePointer <= keyFrameReadPointer){ 
				// we are not out of keyframes yet				
				if(((curDist > (long) calcReqDistSteps(keyframes[curKeyFramePointer].distance)
					&& distTimerActiveFlag && distDirFlag) ||
					(curDist < (long) calcReqDistSteps(keyframes[curKeyFramePointer].distance)
					 && distTimerActiveFlag && !distDirFlag)) ||
					((curHorDeg > (long) calcReqHorSteps(keyframes[curKeyFramePointer].horDeg) 
					&& horTimerActiveFlag && horDirFlag) ||
					(curHorDeg < (long) calcReqHorSteps(keyframes[curKeyFramePointer].horDeg)
					&& horTimerActiveFlag && !horDirFlag)) ||
					((curVerDeg > (long) calcReqVerSteps(keyframes[curKeyFramePointer].vertDeg) 
					&& verTimerActiveFlag && verDirFlag) ||
					(curVerDeg < (long) calcReqVerSteps(keyframes[curKeyFramePointer].vertDeg) 
					&& verTimerActiveFlag && !verDirFlag))){
					// time to change to the next keyFrame
					//sendString("\n\r3:");
					
					//if((curDist > (long) calcReqDistSteps(keyframes[curKeyFramePointer].distance)
					//&& distTimerActiveFlag && distDirFlag) ||
					//(curDist < (long) calcReqDistSteps(keyframes[curKeyFramePointer].distance)
					//&& distTimerActiveFlag && !distDirFlag)){
						//sendString("AYYLMAO1");
					//}
					//
					//if(((curHorDeg > (long) calcReqHorSteps(keyframes[curKeyFramePointer].horDeg)
					//&& horTimerActiveFlag && horDirFlag) ||
					//(curHorDeg < (long) calcReqHorSteps(keyframes[curKeyFramePointer].horDeg)
					//&& horTimerActiveFlag && !horDirFlag))){
						//sendString("AYYLMAO2");
					//}
					//
					//if((curVerDeg > (long) calcReqVerSteps(keyframes[curKeyFramePointer].vertDeg)
					//&& verTimerActiveFlag && verDirFlag) ||
					//(curVerDeg < (long) calcReqVerSteps(keyframes[curKeyFramePointer].vertDeg)
					//&& verTimerActiveFlag && !verDirFlag)){
						//sendString("AYYLMAO3");
					//}
					//
					cli();
					
					
					
					
					
					
					
					
					
					
					curTime = keyframes[curKeyFramePointer].timeStamp;
					curKeyFramePointer ++;
					
					////////////////	DEBUG	/////////////////
					
					//sendString("\n\rCUrrent NEEDEDSTEPS: ");
					//char curinthor[10];
					//sprintf(curinthor, "%d ", calcReqDistSteps(keyframes[curKeyFramePointer].distance));
					//sendString(curinthor);
					/*char dist[10];
					sprintf(dist, "%d", keyframes[curKeyFramePointer].distance);
					sendString("\n\rStart!!!!!!!!!!!!!!!!!!!!!!!!");
					sendString("\n\rKFDist: ");
					sendString(dist);
					
					sendString("\n\rKFHorDeg:");
					char horDeg[10];
					sprintf(horDeg, "%d", keyframes[curKeyFramePointer].horDeg);
					sendString(horDeg);
					
					sendString("\n\rKFVerDeg:");
					char verDeg[10];
					sprintf(verDeg, "%d", keyframes[curKeyFramePointer].vertDeg);
					sendString(verDeg);
					
					sendString("\n\rKFTime:");
					char time[10];
					sprintf(time, "%d", keyframes[curKeyFramePointer].timeStamp);
					sendString(time);*/
					/*
					//print out mcu data
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
					sprintf(curOCR1B, "%u", OCR3A);
					sendString(curOCR1B);
					
					
					sendString("\n\rcurVerDeg:");
					char curVerdegs[10];
					sprintf(curVerdegs, "%d", curVerDeg);
					sendString(curVerdegs);
					
					sendString("\n\rverTimerPreSc:");
					char curVerdegs1[10];
					sprintf(curVerdegs1, "%d", verTimerPreSc);
					sendString(curVerdegs1);
					
					sendString("\n\rOCR0A:");
					char curOCR1C[10];
					sprintf(curOCR1C, "%d", OCR0A);
					sendString(curOCR1C);
					
					sendString("\n\rB is curKeyFramePointer: ");
					char bact[10];
					sprintf(bact, "%d ", curKeyFramePointer);
					sendString(bact);
					
					sendString("\n\rB is curKeyFramePointer: ");
					char bacta[10];
					sprintf(bacta, "%d ", keyFrameReadPointer);
					sendString(bacta);
					*/
					
					/*
					sendString("\n\rTIMSK1: ");
					char reg[10];
					sprintf(reg, "%d ", TIMSK1);
					sendString(reg);
					
					sendString("\n\rSREG: ");
					char reg1[10];
					sprintf(reg1, "%d ", SREG);
					sendString(reg1);
					*/
					
					
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
						//sendString("\n\r5:");
						// check distance axis
						if(keyframes[curKeyFramePointer].distance == keyframes[curKeyFramePointer-1].distance){
							halt_Timer1_A();
							//sendString("\n\r6:");
						}else{							
							if(getDistDirection(curDist, keyframes[curKeyFramePointer].distance) == 0x01){
								//we have to move forward
								//sendString("\n\r....1....:");
								changeDistDir(1);		//now we are moving forward
							}else{
								//sendString("\n\r....2....:");
								changeDistDir(0);		//move backwards
							}
							initDistTimer(OCR1A_value);							
						}
						// check horizontal rotation axis
						if(keyframes[curKeyFramePointer].horDeg == keyframes[curKeyFramePointer-1].horDeg){
							halt_Timer3_A();
						}else{
							if(getHorDirection(curHorDeg, keyframes[curKeyFramePointer].horDeg) == 0x01){
								//we have to move forward
								//sendString("\n\r....1 HOR....:");
								changeHorRotDir(1);		//now we are moving forward
							}else{
								//sendString("\n\r....2 HOR....:");
								changeHorRotDir(0);		//move backward
							}
							initHorTimer(OCR3A_value);
						}
						// check vertical rotation axis
						if(keyframes[curKeyFramePointer].vertDeg == keyframes[curKeyFramePointer-1].vertDeg){
							halt_timer0_A();
						}else{
							if(getVerDirection(curVerDeg, keyframes[curKeyFramePointer].vertDeg) == 0x01){
								//we have to move forward
								//sendString("\n\r....1 VER....:");
								changeVerRotDir(1);		//now we are moving forward
							}else{
								//sendString("\n\r....2 VER....:");
								changeVerRotDir(0);		//move backward
							}
							initVerTimer(OCR0A_value);
						}
						
					}
					sei();
					
						
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
				//sendString("\n\rFINAL CURDIST: ");
				//char curintdist[10];
				//sprintf(curintdist, "%d ", curDist);
				//sendString(curintdist);
				////
				//sendString("\n\rFINAL NEEDEDSTEPS: ");
				//char curinthor[10];
				//sprintf(curinthor, "%d ", calcReqDistSteps(keyframes[curKeyFramePointer].distance));
				//sendString(curinthor);
				////
				//sendString("\n\rFINAL CURKEYFRAMEPOINTER: ");
				//char curintver[10];
				//sprintf(curintver, "%d ", (int) curKeyFramePointer);
				//sendString(curintver);
				haltLapse();
				
				
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
	
	//sei();
	
	
	
	
	//sendString("\n\rOCR1A:");
	//char curOCR1A[10];
	//sprintf(curOCR1A, "%d", OCR1A);
	//sendString(curOCR1A);
	//
	//sendString("\n\rOCR3A:");
	//char curOCR1B[10];
	//sprintf(curOCR1B, "%d", OCR3A);
	//sendString(curOCR1B);
	//
	//sendString("\n\rOCR0A:");
	//char curOCR1C[10];
	//sprintf(curOCR1C, "%d", OCR0A);
	//sendString(curOCR1C);
	
	//sendString("\n\rTIMSK1:");
	//char curOCR1C2[10];
	//sprintf(curOCR1C2, "%d", TIMSK1);
	//sendString(curOCR1C2);
	
	//init_Timer1('A', OCR1A_value);
	//init_Timer1('B', OCR3A_value);
	//init_Timer1('C', OCR1C_value);
	
	initDistTimer(OCR1A_value);
	initHorTimer(OCR3A_value);
	initVerTimer(OCR0A_value);
	
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
	sprintf(curVerdegs1, "%d", verTimerPreSc);
	sendString(curVerdegs1);
	
	sendString("\n\rOCR0A:");
	char curOCR1C[10];
	sprintf(curOCR1C, "%d", OCR0A);
	sendString(curOCR1C);
	
	sendString("\n\rB is curKeyFramePointer: ");
	char bact[10];
	sprintf(bact, "%d ", curKeyFramePointer);
	sendString(bact);
	
	sendString("\n\rB is curKeyFramePointer: ");
	char bacta[10];
	sprintf(bacta, "%d ", keyFrameReadPointer);
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
	//startedFlag = 0x01;
	
}

void haltLapse(){
	/*cli();
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
	
	char dist[10];
	sprintf(dist, "%d", keyframes[curKeyFramePointer].distance);
	sendString("\n\rStart!!!!!!!!!!!!!!!!!!!!!!!!");
	sendString("\n\rKFDist: ");
	sendString(dist);
	
	sendString("\n\rKFHorDeg:");
	char horDeg[10];
	sprintf(horDeg, "%d", keyframes[curKeyFramePointer].horDeg);
	sendString(horDeg);
	
	sendString("\n\rKFVerDeg:");
	char verDeg[10];
	sprintf(verDeg, "%d", keyframes[curKeyFramePointer].vertDeg);
	sendString(verDeg);
	
	sendString("\n\rKFTime:");
	char time[10];
	sprintf(time, "%d", keyframes[curKeyFramePointer].timeStamp);
	sendString(time);
	sei();*/
	//cli();
	startedFlag = 0x00;
	curKeyFramePointer = 0;
	keyFrameReadPointer = 0;
	curDist = 0;
	curHorDeg = 0;
	curVerDeg = 0;
	distTimerPreSc = 0;
	distTimerPreScCounter = 0;
	distTimerPreScFlag = 0;
	for(int i = 0; i < MAX_KEYFRAMES; i++){
		keyframes[i].distance = 0;
		keyframes[i].horDeg = 0;
		keyframes[i].vertDeg = 0;
		keyframes[i].timeStamp = 0;
	}	
	halt_Timer1_A();	//dist
	halt_Timer3_A();	//hor deg
	halt_timer0_A();
	//halt_Timer1_C();
	//sei();
	//sendString("\n\rTIMELAPSE OVER, BYEBYE");
	
	//print out mcu data
	
}

uint32_t calcReqDistSteps(uint16_t recDistance){
	float reqRotations = (float)recDistance/DIST_WHEEL_CIRCUMFERENCE;
	uint32_t neededSteps = (uint32_t) abs(reqRotations*STEPS_IN_ROTATION_DIV16);
	return neededSteps;
}

uint32_t calcReqHorSteps(uint16_t recHorDeg){
	uint32_t neededSteps = (uint32_t) abs((float)recHorDeg * (float)STEPS_IN_DEGREE_DIV16);
	return neededSteps;
}

uint32_t calcReqVerSteps(uint16_t recVerDeg){
	uint32_t neededSteps = (uint32_t) abs((float)recVerDeg * (float)STEPS_IN_DEGREE_DIV16);
	return neededSteps;
}

//calculates needed OCR1A value to travel needed distance in given time
//linear horizontal movement
//recDistance given in centimeters, recTime in seconds
uint32_t calcOCR1A(long curTime, long curDist, uint16_t recDistance, uint16_t recTime){
    // convert centimeters to steps
	float reqRotationsSinceStart = ((float) recDistance)/((float) DIST_WHEEL_CIRCUMFERENCE);
    long neededStepsSinceStart = (long) (reqRotationsSinceStart*STEPS_IN_ROTATION_DIV16);
     
    uint16_t givenTime = abs(recTime-(uint16_t)curTime);
    float wantedStepFreq = (float) (abs(curDist - neededStepsSinceStart))/(float) givenTime;	
    uint32_t result = (uint32_t) ((((float) F_CPU)/ ((float) T1OCA_DIV))/wantedStepFreq);
    if(result >= 0xFFFFFFFF || result == 0x00000000){
        return 0xFFFFFFFF;
    }else{
        return result;
    }
}

//calculates needed OCR1B value to travel needed rotation in given time
//horizontal rotational movement
//recHorDeg given in steps, recTime in seconds
uint32_t calcOCR3A(long curTime, long curHorDeg, uint16_t recHorDeg, uint16_t recTime){
	//convert degrees to steps
	float stepsSinceStart = (float)recHorDeg * (float)STEPS_IN_DEGREE_DIV16;
	uint32_t neededHorSteps = abs((uint32_t)stepsSinceStart - (uint32_t)curHorDeg);
	
	uint16_t givenTime = (recTime-(uint16_t)curTime);
	float wantedStepFreq = (float)neededHorSteps / (float)givenTime;
	uint32_t result = (uint32_t) ((((float) F_CPU)/ ((float) T1OCA_DIV))/wantedStepFreq);
	if(result >= 0xFFFFFFFF || result == 0x00000000){
		return 0xFFFFFFFF;
	}else{
		return result;
	}
}

//calculates needed OCR1C value to travel needed rotation in given time
//vertical rotational movement
//recVerDeg given in steps, recTime in seconds
uint32_t calcOCR0A(long curTime, long curVerDeg, uint16_t recVerDeg, uint16_t recTime){
	/*cli();
			sendString("\n\rcurTime: ");
			char curintdist1[10];
			sprintf(curintdist1, "%ld ", curTime);
			sendString(curintdist1);
			
					sendString("\n\rcurVerDeg: ");
					char curintdist11[10];
					sprintf(curintdist11, "%ld ", curVerDeg);
					sendString(curintdist11);
					
							sendString("\n\rrecVerDeg: ");
							char curintdist111[10];
							sprintf(curintdist111, "%d ", recVerDeg);
							sendString(curintdist111);
							
									sendString("\n\rrecTime: ");
									char curintdist1112[10];
									sprintf(curintdist1112, "%d ", recTime);
									sendString(curintdist1112);
									
	sei();*/
	//convert degrees to steps
	float stepsSinceStart = (float)recVerDeg * (float)STEPS_IN_DEGREE_DIV16;
	uint32_t neededVerSteps = (uint32_t) abs((long)stepsSinceStart - curVerDeg);
		
	uint16_t givenTime = (recTime-(uint16_t)curTime);
	float wantedStepFreq = (float)neededVerSteps / (float)givenTime;
	uint32_t result = (uint32_t) roundf(((((float) F_CPU)/ ((float) TIMER0_DIV))/wantedStepFreq));
	if(result >= 0xFFFFFFFF || result == 0x00000000){
		return 0xFFFFFFFF;
	}else{
		return result;
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

void initDistTimer(uint32_t CTC_value){
	//sendString("initdisttimer");
	//sendString("\n\rCTC_value: ");
	//char curintver332[10];
	//sprintf(curintver332, "%ld ", CTC_value);
	//sendString(curintver332);
    TCNT1 = 0x0000;
    TCCR1A = 0x00;
    // setting CPU clock, no preScalers
	// enable CTC
    TCCR1B = (1<<CS10 | 1<<WGM12); 
	
	if(CTC_value == 0xFFFFFFFF){
		halt_Timer1_A();
		//sendString("AM HERE1");
		//sendString("\n\rCTCValue: ");
		//char curintdist[10];
		//sprintf(curintdist, "%d ", (int)CTC_value);
		//sendString(curintdist);
	}else if(CTC_value > 0xFFFF){
		distTimerPreScFlag = 1;
		distTimerPreScCounter = 0;
		
		distTimerPreSc = (uint16_t) ceil((float)CTC_value / (float)TIMER_16BIT_ITERATIONS);
		uint16_t tooMuch = (uint16_t) abs((distTimerPreSc*TIMER_16BIT_ITERATIONS) - CTC_value);
		uint16_t toSubtract = tooMuch / distTimerPreSc;	// rounding could improve accuracy	
		OCR1A = 65535-toSubtract;	
			
		// enable Timer Overflow Interrupt for COMPA
        TIMSK1 |= (1<<OCIE1A);
		distTimerActiveFlag = 1;
		//sendString("AM HERE2");	
	}else{
		distTimerPreScFlag = 0;
        OCR1A = CTC_value & 0xFFFF;
		// enable Timer Overflow Interrupt for COMPA
        TIMSK1 |= (1<<OCIE1A);
		distTimerActiveFlag = 1;
		//sendString("AM HERE3");
	}
}

void initHorTimer(uint32_t CTC_value){
	//sendString("inithortimer");
	//sendString("\n\rCTC_value: ");
	//char curintver332[10];
	//sprintf(curintver332, "%ld ", CTC_value);
	//sendString(curintver332);
		TCNT3 = 0x0000;
		TCCR3A = 0x00;
		// setting CPU clock, no preScalers
		// enable CTC
		TCCR3B = (1<<CS30 | 1<<WGM32);
		
		if(CTC_value == 0xFFFFFFFF){
			halt_Timer3_A();
		}else if(CTC_value > 0xFFFF){
			horTimerPreScFlag = 1;
			horTimerPreScCounter = 0;
			// calculate so interrupts will not be very often, but precise enough
			horTimerPreSc = (uint16_t) ceil((float)CTC_value / (float)TIMER_16BIT_ITERATIONS);
			uint16_t tooMuch = (uint16_t) abs(((uint32_t)horTimerPreSc*TIMER_16BIT_ITERATIONS) - CTC_value);
			uint16_t toSubtract = tooMuch / horTimerPreSc;	// rounding could improve accuracy
			OCR3A = 65535-toSubtract;
			
			TIMSK3 |= (1<<OCIE3A);
			horTimerActiveFlag = 1;
		}else{
			horTimerPreScFlag = 0;
			OCR3A = CTC_value & 0xFFFF;
			// enable Timer CTC Interrupt for COMPA
			TIMSK3 |= (1<<OCIE3A);
			horTimerActiveFlag = 1;
		}
}

void initVerTimer(uint32_t CTC_value){
	// set CPU clock/256
	//sendString("initvertimer");
	//sendString("\n\rCTC_value: ");
	//char curintver332[10];
	//sprintf(curintver332, "%ld ", CTC_value);
	//sendString(curintver332);
	TCNT0 = 0;
	TCCR0B |= 1<<CS02;
	//set to CTC mode
	TCCR0A |= (1<<WGM01);
	if(CTC_value == 0xFFFFFFFF){
		halt_timer0_A();
	}else if(CTC_value > (uint32_t)0xFF){
		verTimerPreScFlag = 1;
		verTimerPreScCounter = 0;		
		
		verTimerPreSc = (uint16_t) ceil((float)CTC_value / (float)TIMER_8BIT_ITERATIONS);
		uint16_t tooMuch = (uint16_t) abs(((uint32_t)verTimerPreSc*TIMER_8BIT_ITERATIONS) - CTC_value);
		uint8_t toSubtract = tooMuch / verTimerPreSc;	// rounding could improve accuracy
		OCR0A = 255-toSubtract;
		
		//allow CTC interrupt
		TIMSK0 |= (1<<OCIE0A);
		verTimerActiveFlag = 1;
	}else{
		verTimerPreScFlag = 0;
		OCR0A = CTC_value & 0xFF;
		//allow CTC interrupt
		TIMSK0 |= (1<<OCIE0A);
		verTimerActiveFlag = 1;
	}
	/*cli();
	sendString("\n\rCURVERDEG: ");
	char curintver[10];
	sprintf(curintver, "%ld ", curVerDeg);
	sendString(curintver);
	
	sendString("\n\rOCR0A: ");
	char curintver33[10];
	sprintf(curintver33, "%d ", OCR0A);
	sendString(curintver33);
	
	sendString("\n\rCTC_value: ");
	char curintver332[10];
	sprintf(curintver332, "%ld ", CTC_value);
	sendString(curintver332);
	sei();*/
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
	distTimerActiveFlag = 0;
	TCNT1 = 0;	
}

void halt_Timer3_A(){
	// disable Timer Overflow Interrupt for COMPA
	TIMSK3 &= ~(1 << OCIE3A);
	horTimerActiveFlag = 0;
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

// get OSCR1A value for required frequency
int getOSCR1A(int freq){
	return (F_CPU/CKDIV_16/freq);	
}

uint8_t getDistDirection(long currentPosition, uint16_t futurePosition){
	float reqRotationsSinceStart = ((float)futurePosition/DIST_WHEEL_CIRCUMFERENCE);
    long neededStepsSinceStart = (long) abs(reqRotationsSinceStart*16457.143);
    if(neededStepsSinceStart < currentPosition){
        return 0x00;    //moving backwards
    }else{
        return 0x01;    //moving forward
    }
}


uint8_t getHorDirection(long currentDegreeSteps, uint16_t futurePosition){
	long neededStepsSinceStart = (long) abs((float)futurePosition * (float)STEPS_IN_DEGREE_DIV16);
	if(neededStepsSinceStart < currentDegreeSteps){
		return 0x00;    //moving backwards
	}else{
		return 0x01;    //moving forward
	}
}

uint8_t getVerDirection(long currentDegreeSteps, uint16_t futurePosition){
	long neededStepsSinceStart = (long) abs((float)futurePosition * (float)STEPS_IN_DEGREE_DIV16);
	if(neededStepsSinceStart < currentDegreeSteps){
		return 0x00;    //moving backwards
	}else{
		return 0x01;    //moving forward
	}
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

///////////////////////////////////////////////// STEP INTERRUPTS /////////////////////////////////////////////
// Upper (distance) stepper interrupt
SIGNAL(TIMER1_COMPA_vect){
    //Step on step2 (PB5), upper stepper
    //TEST WITH POWER SUPPLY      
	if(distTimerPreScFlag){
		if(distTimerPreScCounter < (distTimerPreSc-1)){
			distTimerPreScCounter++;
		}else{
			distTimerPreScCounter = 0;
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
    if(horTimerPreScFlag){
	    if(horTimerPreScCounter < (horTimerPreSc-1)){
			horTimerPreScCounter++;
		}else{
		    horTimerPreScCounter = 0;
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
	if(verTimerPreScFlag){
		if(verTimerPreScCounter < (verTimerPreSc-1)){
			verTimerPreScCounter ++;
		}else{
			verTimerPreScCounter = 0;
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
		//sendString("\n\rCURVERDEG: ");
		//char curintver[10];
		//sprintf(curintver, "%ld ", curVerDeg);
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
		sendString("\n\rStarting...");
		startLapse();
	}
}



