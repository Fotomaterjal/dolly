/*
 * IR_katsetus.c
 *
 * Created: 16.06.2014 17:09:25
 *  Author: Ottk6uk
 */ 


#include <avr/io.h>
#include <avr/delay.h>

void delay(uint8_t x);

int main(void)
{
	
	DDRD = 0x20;
	DDRA = 0x20;
	
    while(1)
    {
		/*for(int i =0; i<2000; i++){
			for(int j=0; j<10; j++){
			PORTD ^= 0x20;
			_delay_us(27);
		
			}		
			for(int j=0; j<10; j++){
			PORTD & ~(0x20);
			_delay_us(27);
			}
		}*/
				
			for(int i = 0; i<10; i++){
					PORTD ^= 0x20;
					_delay_us(27);
			
					PORTD & ~(0x20);
					_delay_us(27);
				}
			
			for(int i = 0; i<10; i++){
					PORTD & ~(0x20);
					_delay_us(27);

					PORTD & ~(0x20);
					_delay_us(27);
			}				
			
		//}		
		/*
		for(int i=0; i<30; i++){
			PORTD ^= 0x20;
			_delay_us(27);
			
		}
		for(int i=0; i<10; i++){
			PORTD & ~(0x20);
			_delay_us(27);
		}
		
		
		for(int i=0; i<25; i++){
			PORTD ^= 0x20;
			_delay_us(27);
			
		}
		for(int i=0; i<10; i++){
			PORTD & ~(0x20);
			_delay_us(27);
		}*/		
    }
}