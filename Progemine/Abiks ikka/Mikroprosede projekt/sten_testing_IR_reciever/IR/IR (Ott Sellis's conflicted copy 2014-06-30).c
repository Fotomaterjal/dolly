/*
 * IR.c
 *
 * Created: 15.06.2014 20:28:37
 *  Author: Oll
 */ 

#include "avr/interrupt.h"
#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 2000000
#define STOP 0b00000000
#define OTSE 0b00000101
#define TAGASI 0b00001010
#define PAREM 0b00000110
#define VASAK 0b00001001
#define fixation_delay 200
#define pulsipikkus 2175
//PB4 is RIGHT MOTOR and 2180PB5 is LEFT MOTOR!

#define high(x)   ((x)>>2)
#define low(x)   ((x) & 0xFF)

uint16_t measureVoltage();
void WriteTCNT(uint16_t i, uint8_t which);
void drive(uint8_t state);

uint8_t whatToDo = STOP, state;

uint16_t whatLeft(uint8_t whatToDo);
uint16_t whatRight(uint8_t whatToDo);

int main(void)
{
	//setting port b pin5 and LED-s (testing) to output
	DDRB = (1 << PB5 | 1 << PB4);
	DDRA = 0xFF;
	
	//ADC
	//selecting reference voltage VCC, measure voltage on pin ADC2
	ADMUX |= (1 << REFS0 | 1 << MUX1);
	
	//TIMER1
	//initializing timer values
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1C = 0;
	//setting 1 CPU clock, set normal mode for TCCR1B
	TCCR1B = (1 << CS10);
	//loading OCR1A with 40000 to have 20ms delay between pwm pulses
	OCR1A = 25536;
	//OCR1B = 25536;
	//setting TCCR1 to "non-PWM", setting OC1A low after compare match
	//TCCR1A = (1<<COM1A1);
	TIMSK1 = (1 << TOIE1) | (1 << OCIE1A);
	//WriteTCNT1(0xD6D8);
	
	// To unsync the timers
	for(int i = 0; i < 3000; i++){
		asm("nop");
	}
	
	//TIMER3
	//initializing timer values
	TCNT3 = 0;
	TCCR3A = 0;
	TCCR3B = 0;
	TCCR3C = 0;
	//setting CPU clock, set normal mode for TCCR1B
	TCCR3B = (1 << CS30);
	//loading OCR1A with 40000 to have 20ms delay between pwm pulses
	OCR3A = 25536;
	//OCR1B = 25536;
	//setting TCCR1 to "non-PWM", setting OC1A low after compare match
	//TCCR1A = (1<<COM1A1);
	TIMSK3 = (1 << TOIE3)| (1 << OCIE1A);
	//WriteTCNT1(0xD6D8);

	sei();
	
	while((PINE & 0x20) == 0x20){
		whatToDo = STOP;
	}
	
    while(1)
    {
		if((PINE & 0x20) == 0x00){
			//cli();
			_delay_us(fixation_delay);
			
			_delay_us(pulsipikkus);
			if((PINE & 0x20) == 0x00){
				state += 1;
			}			
			_delay_us(pulsipikkus);
			if((PINE & 0x20) == 0x00){
				state += 2;
			}				
			_delay_us(pulsipikkus);
			if((PINE & 0x20) == 0x00){
				state += 4;
			}				
			_delay_us(pulsipikkus);
			if((PINE & 0x20) == 0x00){
				state += 8;
			}			
			
			drive(state);
			state = 0;
			//sei();
			
			_delay_ms(300);
			
		}			
    }
}


void drive(uint8_t state){
	if(state == OTSE){
		whatToDo = OTSE;
	}
	else if(state == VASAK){
		whatToDo = VASAK;
	}
	else if(state == PAREM){
		whatToDo = PAREM;
	}
	else if(state == TAGASI){
		whatToDo = TAGASI;
	}
	else{
		whatToDo = STOP;
	}
	PORTA = state;
}

void WriteTCNT(uint16_t i , uint8_t which)
{
	unsigned char sreg;
	//TCNT3 = 0;
	//TCNT1 = 0;
	//uint16_t i;
	/* Save global interrupt flag */
	sreg = SREG;
	/* Disable interrupts */
	cli();
	/* Set TCNTn to i */
	if(which == 3){
		TCNT3 = i;
	}else{
		TCNT1 = i;
	}
	/* Restore global interrupt flag */
	SREG = sreg;
}

uint16_t whatRight(uint8_t whatToDo){
	uint16_t result;
	
	if(whatToDo == 0x05 || whatToDo == 0x09){
		result = 22940;	// pulse length less than 1,3 ms aka forward rotation
	}else if(whatToDo == 0x06 || whatToDo == 0x0A){
		result = 22136;	// pulse length more than 1,7 ms aka backward rotation
	}else{
		result = 22639;	// pulse length exactly 1,5 ms aka no motor rotation
	}
	return result;
}

uint16_t whatLeft(uint8_t whatToDo){//must think backwards because motor is upside down
uint16_t result2;
if(whatToDo == 0x0A || whatToDo == 0x09){
	//return 22940;
	result2 = 22945;	// pulse length less than 1,3 ms aka forward rotation
}else if(whatToDo == 0x06 || whatToDo == 0x05){
	//return 22136;
	result2 = 22141;	// pulse length more than 1,7 ms aka backward rotation
}else{
	//return 22536;
	result2 = 22725;	// pulse length exactly 1,5 ms aka no motor rotation
}
return result2;
}

SIGNAL(TIMER1_COMPA_vect){
	PORTB = PORTB & ~(0x20);	//PB5
}

SIGNAL(TIMER3_COMPA_vect){
	PORTB = PORTB & ~(0x10);	//PB4
}

SIGNAL(TIMER1_OVF_vect){
	PORTB = 0x20;
	uint16_t reading = whatLeft(whatToDo);			//PB5
	WriteTCNT(reading, 1);		//vahemik 1,3 kuni 1,7 ms impulsi tegemiseks
}

SIGNAL(TIMER3_OVF_vect){
	PORTB = 0x10;
	uint16_t reading = whatRight(whatToDo);			//PB4
	WriteTCNT(reading, 3);		//vahemik 1,3 kuni 1,7 ms impulsi tegemiseks
}