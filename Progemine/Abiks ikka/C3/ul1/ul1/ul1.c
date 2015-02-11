/*
 * ul1.c
 *
 * Created: 6.05.2014 17:49:13
 *  Author: Oll
 */ 

#include <stdio.h>
#include <avr/io.h>

uint8_t strcopy(char *dest, char *src, int destSize);

int main(void)
{
	char nimi[] = "Sten";
	char dest[100];
	
    while(1)
    {
		strcopy(dest, nimi, 100);
    }
}



uint8_t strcopy(char *dest, char *src, int destSize){
	char* destPtr = dest;
	char* srcPtr = src;
	int i = 0;
	
	while(*srcPtr != 0){
		if(i >= destSize){
			return 0;
		}
		*destPtr = *srcPtr;
		
		destPtr++;
		srcPtr++;
		i++;
	}
	return 1;
}
