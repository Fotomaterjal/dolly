/*
 * ir_recieve_2.c
 *
 * Created: 22.06.2014 16:07:37
 *  Author: Oll
 */ 
#define F_CPU 2000000

#include <avr/io.h>
#include <util/delay.h>



int main(void)
{
	DDRE = 0x00;
	PINE = 0x00;
	PORTE = 0x00;
	DDRA = 0xFF;
	
	
    while(1)
    {
		uint8_t lugem = PINE;
		
        if(lugem&(0x20) == 0x00){	//0 in input
			PORTA = 0xF0;
		}else if(lugem&(0x20) == 0x20){	// 1 in input
			PORTA = 0x0F;
		}else{
			PORTA = 0x01;
		}
		
		//_delay_ms(500);
		
	} 
}