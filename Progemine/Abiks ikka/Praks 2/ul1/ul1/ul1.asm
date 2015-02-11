/*
 * AssemblerApplication1.asm
 *
 *  Created: 10.03.2014 9:46:29
 *   Author: Oll
 */ 

;initialising Stack Pointer to 0x10FF (using 16 bits)
LDI R16, 0xFF 
OUT SPL, R16 
LDI R16, 0x10 
OUT SPH, R16

LDI R16, 10

delay_loop:
	PUSH r16
	PUSH r16
	PUSH r16
	POP r16
	POP r16
	POP r16

JMP delay_loop