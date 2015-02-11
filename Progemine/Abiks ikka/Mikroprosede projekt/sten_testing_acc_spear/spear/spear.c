#define F_CPU 2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint16_t mooda();
uint16_t tulemus;
uint16_t tulemusBuf = 0;
uint16_t lahutus = 9362; // 1024/7  //siin tuleb muuta vastavalt lahutusele
uint8_t led = 0x01;
uint8_t led1;
uint8_t led2;

int16_t x;

//timer compare match interrupt
ISR(TIMER1_COMPA_vect){
	PORTA = led1;
}
ISR(TIMER1_COMPB_vect){
	PORTA = led2;
}

int16_t getMeanMeas(void)
{
	int16_t measurement = 0;
	int32_t meanMeas = 0;
	
	for(uint16_t i=0; i<300; i++)
	{
		//Madala baidi pärimine
		SPI_MasterTransmit(0xFF, 0xA8);
		measurement = SPDR;
		
		//Kõrge baidi pärimine
		SPI_MasterTransmit(0xFF, 0xA9);
		measurement |= SPDR<<8;

		meanMeas += measurement;
	}
	meanMeas = meanMeas/300 + 32768; //pool resolutsioonist
	
	return (int16_t)meanMeas;
}

void SPI_MasterInit(void)
{
	/* Set MOSI, SS and SCK output, all others input */
	DDRB = (1<<PB2)|(1<<PB1)|(1<<PB0);
	//PORTB = (0<<PB0);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	
}
void USART_init()
{
	UCSR1B = (1<<TXEN1);
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);
	UBRR1 = 12;
}

void USART_transmit(char send)
{
while(!( UCSR1A & (1<<UDRE1))){}
UDR1 = send;
}

void SPI_MasterTransmit(char cData, char address)
{	
	PORTB = (0<<PB0);
	/* Start transmission */
	SPDR = address;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	
	SPDR = cData;
	while(!(SPSR & (1<<SPIF)));
	
	PORTB = (1<<PB0);
}

uint16_t mooda(){
	
	tulemus = getMeanMeas();
	OCR1B = (tulemus%lahutus)*7;
	if (tulemus < lahutus){
			led2 = (1 << PA0);
			led1 = (1 << PA1);
	}

	else if (tulemus < (lahutus*2)){
		
			led1 = (1 << PA2);
			led2 = (1 << PA1);
	}

	else if (tulemus <  (lahutus*3)){
			led1 = (1 << PA3);
			led2 = (1 << PA2);
	}

	else if (tulemus < (lahutus*4)){
		
			led2 = (1 << PA3);
			led1 = (1 << PA4);
	}
	else if (tulemus < (lahutus*5)){
		
			led2 = (1 << PA4);
			led1 = (1 << PA5);
	}
	else if (tulemus < (lahutus*6)){
		
			led2 = (1 << PA5);
			led1 = (1 << PA6);
	}
	else if (tulemus < (lahutus*7)){
		
			led2 = (1 << PA6);
			led1= (1 << PA7);
	}

	return (tulemus);

}

int main(void)
{
	USART_init();
	SPI_MasterInit();
	SPI_MasterTransmit(0b01110111,0x20);
	
	cli();//keelan seadistamiseks katkestused
	//TIMER
	TCCR1A = (1<<WGM10)| (1<<WGM11);
	TCCR1B = (1<<CS10)| (1<<WGM12) ;
	//enable timer interrupts
	TIMSK1 = (1<<OCIE1A) | (1<<OCIE1B);
	sei();//luban katkestused peale seadistamist
	//LED PORTA
	OCR1A = 1023;
	DDRA = 0xFF;
	
    while(1)
    {	
		mooda();																												
    }
}