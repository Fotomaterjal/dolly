/*
 * ul2.c
 *
 * Created: 7.05.2014 10:00:29
 *  Author: Oll
 */ 


#include <avr/io.h>

uint8_t concatenate(char *dest, char* src, int destSize);


int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

uint8_t concatenate(char *dest, char* src, int destSize){

	char* destPtr = dest;
	int i = 0;				//looking out fo rdestination size
	while(*destPtr != 0){	//taking dest pointer to concatenation spot
		destPtr++;
		i++;
	}
	
		
	while(*srcPtr != 0){
		if(i+1 >= destSize){
			*destPtr = 0;
			return 0;
		}
		*destPtr = *srcPtr;
		
		destPtr++;
		srcPtr++;
		i++;
	}
	return 1;
	
}