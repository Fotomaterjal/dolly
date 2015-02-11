/*
 * ul3.asm
 *
 *  Created: 10.03.2014 10:03:52
 *   Author: Oll
 */ 

 
;initialising Stack Pointer to 0x10FF (using 16 bits)
LDI R16, 0xFF 
OUT SPL, R16 
LDI R16, 0x10 
OUT SPH, R16

 
LDI r17, 10 
LDI r18, 20 
PUSH r17 
PUSH r18 
POP r17 
POP r18 
NOP