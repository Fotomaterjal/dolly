/*
 * ul4.asm
 *
 *  Created: 10.03.2014 10:06:42
 *   Author: Oll
 */ 

;initialising Stack Pointer to 0x10FF (using 16 bits)
LDI R16, 0xFF 
OUT SPL, R16 
LDI R16, 0x10 
OUT SPH, R16


Tsykkel: 
	LDI r16, 10 
	OUT 0x05, r16 
	CALL StupidProcedure 
	LDI r16, 20 
	OUT 0x05, r16 

	CALL SmartProcedure 
	LDI r16, 25 
	OUT 0x05, r16 

RJMP Tsykkel 

StupidProcedure:  
	NOP
RET

SmartProcedure:
	CALL StupidProcedure
	CALL StupidProcedure
RET