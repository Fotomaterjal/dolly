/*
 * accelerometer.c
 *
 * Created: 6.06.2014 13:48:00
 *  Author: Oll
 */ 


#include <avr/io.h>
#include "avr/interrupt.h"

#define DD_MISO		DDB3
#define DD_MOSI		DDB2
#define DD_SCK		DDB1
#define DD_SS		DDB0
#define DDR_SPI		DDRB

void SPI_MasterInit(void);
uint8_t SPI_MasterTransmit(uint8_t cData);
void SPI_SlaveInit(void);
char SPI_SlaveReceive(void);
void sendCharToPC(uint8_t number);

void sendLetter(uint8_t letter);
void sendName(char name[]);


int main(void)
{
	char name[] = "Sten-Oliver";
	unsigned char recievedData;
	unsigned char SPICongfigData;
	
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
	
	
	SPI_MasterInit();
	//TODO initialize accelerometer
	//configdata is 7 downto 0
	//bit0: '1' write bit 8 to 15; '0' read data from 1 to 7
	//bit1: '1' address is auto-incremented; '0' address remains unchanged
	//bit2 to bit 7: address field of the indexed register (register to be modified)
	//bit8 to 15: data to be written in the specified register (read if bit0 is '1')
	
	////Configure ADC register
	//SPICongfigData = 0b01111100;
	//SPI_MasterTransmit(SPICongfigData);
	////Enable ADC
	//SPICongfigData = 0b10000000;
	//SPI_MasterTransmit(SPICongfigData);
	////Configure axis/frequency register
	//SPICongfigData = 0b10000000;
	//SPI_MasterTransmit(SPICongfigData);
	////enable all axises using 50Hz frequency
	//SPICongfigData = 0b01000111;
	//SPI_MasterTransmit(SPICongfigData);
	////configure FIFO register
	//SPICongfigData = 0b10010000;
	//SPI_MasterTransmit(SPICongfigData);
	////FIFO enable
	//SPICongfigData = 0b01000000;
	//SPI_MasterTransmit(SPICongfigData);
	
	
	
	//TESTING
	//SPICongfigData = 0b00111100;
	//SPI_MasterTransmit(SPICongfigData);
	//SPICongfigData = 0b00100000;
	//SPI_MasterTransmit(SPICongfigData);
	//SPI_SlaveInit();
	//SPICongfigData = 0b00111101;
	//SPI_MasterTransmit(SPICongfigData);
	//Enable ADC
	//SPICongfigData = 0b00000000;
	//SPI_MasterTransmit(SPICongfigData)
	
	
	//X-axis data is 28L, 29H
	//Y-axis data is 2AL, 2BH
	//Z-axis data is 2CL, 2DH
	
	uint8_t letter = 1;
	
	
	
	
	
    while(1)
    {
        ////TODO:: Please write your application code
		//letter = 'k';
		//sendLetter(letter); 
		letter = 1;
		sendLetter(letter);
		SPICongfigData = 2;
		letter = SPI_MasterTransmit(SPICongfigData);
		sendLetter(letter);
		letter = 1;
		sendLetter(letter);
		//recievedData = SPI_MasterTransmit('s');
		//recievedData = SPI_MasterTransmit(letter);
		//SPI_SlaveInit();
		
		//recievedData = SPI_SlaveReceive();		
		
		//letter = 'b';
		//sendLetter(recievedData);
		//recievedData = SPI_SlaveReceive();
		//PORTB ^= (1<<SS);
		//SPDR = 0x00;
		//recievedData = SPDR;
		//for(int i = 0; i<20;i++){
			//UDR1 = name[i];
		//}
		//for(int i = 0; i < 8; i++);
		//UDR1 = letter;
		//SPSR ^= (1<<SPIF);
		
		//sendName(recievedData);
		//if(recievedData){
			//sendLetter(recievedData);
		//}else{
			//sendLetter(recievedData);
			
			
		//}
		
			
    }
}






void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPIE);
}

uint8_t SPI_MasterTransmit(uint8_t cData)
{
	
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
	SPCR ^= (1<<4);//changed
	return SPDR;	//changed
}

void SPI_SlaveInit(void)
{
	/* Set MISO output, all others input */
	DDR_SPI = (1<<DD_MISO);
	/* Enable SPI */
	SPCR = (1<<SPE);
}

char SPI_SlaveReceive(void)
{
	//SPSR ^= (1<<SPIF);
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)));
	//SPSR ^= (1<<SPIF);				//CHANGED
	/* Return Data Register */
	return SPDR;
}

void sendCharToPC(uint8_t number){
	//load value to UART sending register
	UDR1 = number;
	//wait loop for send to be completed
	while(~UCSR1A&(1 << TXC1)){
		asm("nop");
	}
	//clearing TX complete flag
	UCSR1A |= (1 << TXC1);
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

void sendName(char name[]){
	for( int i = 0; i < strlen(name); i++){
		sendLetter(name[i]);
	}
}

SIGNAL(USART1_RX_vect){
	// Enable timer interrupts
	TIMSK1 = (1<<OCIE1A);
	// Read UART
	uint8_t a = UDR1;
	sendLetter(a);
}