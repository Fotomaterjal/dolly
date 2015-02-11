/*
 * ul1.asm
 *
 *  Created: 17.03.2014 10:34:51
 *   Author: Oll
 */ 

;initialising Stack Pointer to 0x10FF (using 16 bits)
LDI R16, 0xFF 
OUT SPL, R16 
LDI R16, 0x10 
OUT SPH, R16


LDI R16, 0
STS TCCR1A, R16
STS TCCR1C, R16
LDI R16, (1 << CS10)
STS TCCR1B, R16
/*LDI R16, 0xD0
LDI R17, 0x07*/

loop:
JMP loop

 

