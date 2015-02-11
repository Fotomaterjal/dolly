/*
 * acc2.c
 *
 * Created: 9.06.2014 12:36:31
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
void sendLetter(uint8_t letter);
int16_t measureX(void);
int16_t measureY(void);




int main(void)
{
	
	//UART
	//setting baud rate to 9600
	UBRR1 = 25;
	UCSR1A |= (1 << U2X1);
	//setting up 8 data bits
	UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);
	//enabling transmitter
	//enabling transmitter, receiver, receive complete interrupt
	UCSR1B = (1 << TXEN1 | 1 << RXEN1 | 1 << RXCIE1);
	sei();
	
	// Init SPI as master
	SPI_MasterInit();
	// Configure accelerometer in CTRL_REG1
	// Enable X and Y, disable Z, set measurement frequency to 400 Hz
	SPI_MasterTransmit(0x20, 0x73);
	
		
	
	
	
    while(1)
    {
		//SPI_MasterTransmit(SPICongfigData);
		measureX();
		//sendLetter(measureX()>>8);
		//sendLetter(measureX());
		//sendLetter('\n');
		_delay_ms(500);
		measureY();
		_delay_ms(500);
		
		
        //TODO:: Please write your application code 
    }
}



void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
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

void sendLetter(uint8_t letter){
	while ((UCSR1A & (1 << UDRE1)) == 0) ;
	//load value to UART sending register
	UDR1 = letter;
	//wait loop for send to be completed
	while(~UCSR1A&(1 << TXC1)){
		asm("nop");
	}
	//clearing TX complete flag
	UCSR1A |= (1 << TXC1);
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
		sendLetter('0');
	}else if(xValue > 0x00F0){
		sendLetter('a');
	}else{
		sendLetter('b');
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
		sendLetter('1');
	}else if(yValue > 0x00F0){
		sendLetter('c');
	}else{
		sendLetter('d');
	}
	return yValue;
}

//char SPI_OneMoreByte(void){
	///* Start transmission, set SS low */
	//PORTB = (0<<PB0);
	//SPDR = 0x00;
	///* Wait for transmission complete */
	//while(!(SPSR & (1<<SPIF)));
	//return SPDR;
//}

//uint16_t SPI_GetDoubleByte(char accReg, char cData){
	//uint16_t result;
	//
//}