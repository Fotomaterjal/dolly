/*
 * GccApplication1.c
 *
 * Created: 22.06.2014 23:39:25
 *  Author: Oll
 */ 


#include <avr/io.h>
#include "avr/interrupt.h"
#include <util/delay.h>

#define F_CPU		2000000
#define DD_MISO		DDB3
#define DD_MOSI		DDB2
#define DD_SCK		DDB1
#define DD_SS		DDB0
#define DDR_SPI		DDRB

void SPI_MasterTransmit(char accReg, char cData);
void SPI_MasterInit(void);
int16_t measureX(void);
int16_t measureY(void);
void send_zero();
void send_one();

uint8_t whatToDo = 0x00;

int main(void)
{
	
	
	DDRD = 0x20;
	// Initialize SPI as master
	SPI_MasterInit();
	// Configure accelerometer in CTRL_REG1
	// Enable X and Y, disable Z, set measurement frequency to 400 Hz
	SPI_MasterTransmit(0x20, 0x73);
	// global interrupt enable
	sei();
	
	
	DDRA = 0xFF;		//for testing
	
    while(1)
    {	
		send_zero();
        
			// is the first bit '1' ?
			if(whatToDo & 0x01){
				send_zero();
			}else{
				send_one();
			}
			// is the second bit '1' ?
			if((whatToDo >> 1) & 0x01){
				send_zero();
			}else{
				send_one();
			}
			// is the third bit '1' ?
			if((whatToDo >> 2) & 0x01){
				send_zero();
			}else{
				send_one();
			}
			// is the fourth bit '1' ?
			if((whatToDo >> 3) & 0x01){
				send_zero();
			}else{
				send_one();
			}
			PORTA = whatToDo;
			// wait and chill
			
			
			
			_delay_ms(300);
			measureX();
			measureY();			
		
    }
}


// signal that generates '0' in the receiver
void send_zero(){
	for(uint8_t i=0; i<20; i++){
		// keep output HIGH for ~27 us
		PORTD |= 0x20;
		_delay_us(23);
		// keep output LOW for ~27 us
		PORTD &= ~(0x20);
		_delay_us(23);
	}
}

// signal that generates '1' in the receiver
void send_one(){
	for(uint8_t i=0; i<20; i++){
		// keep output LOW for ~27 us
		PORTD &= ~(0x20);
		_delay_us(23);
		// keep output LOW for ~27 us
		PORTD &= ~(0x20);
		_delay_us(23);
	}
}

// ACCELEROMETER STUFF
void SPI_MasterInit(void)
{
	/* Set MOSI and SCK and SS output, all others input */
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_SS);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void SPI_MasterTransmit(char accReg, char cData)
{
	/* Start transmission, set SS low */
	PORTB = (0<<PB0);
	/* Start transmission, select register*/
	SPDR = accReg;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	/* Send real data */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	/* Set SS high again, end transmission */
	PORTB = (1<<PB0);
}

int16_t measureX(void){
	int16_t xValue;
	//measure value of X axis, register 28 (OUT_X_L)
	SPI_MasterTransmit(0xA8, 0xFF);
	xValue |= SPDR;
	//measure value of X axis, register 29 (OUT_X_H)
	SPI_MasterTransmit(0xA9, 0xFF);
	xValue |= (SPDR << 8);
	

	xValue += 0x1FF;
	// reducing jumping
	if(xValue < 0x0AFF && xValue > 0){
		whatToDo = 0x00;
	}else if(xValue > 0x00F0){
		whatToDo = 0x05;
	}else{
		whatToDo = 0x0A;
	}
	return xValue;
}

int16_t measureY(void){
	int16_t yValue;
	//measure value of X axis, register 28 (OUT_X_L)
	SPI_MasterTransmit(0xAA, 0xFF);
	yValue |= SPDR;
	//measure value of X axis, register 29 (OUT_X_H)
	SPI_MasterTransmit(0xAB, 0xFF);
	yValue |= (SPDR << 8);
	

	yValue += 0x1FF;
	// reducing jumping
	if(yValue < 0x0AFF && yValue > 0){
		asm("nop");
	}else if(yValue > 0x00F0 && whatToDo == 0x00){
		whatToDo = 0x09;
	}else if(whatToDo == 0x00){
		whatToDo = 0x06;
	}
	return yValue;
}