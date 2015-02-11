/*
 * ul2.asm
 *
 *  Created: 3.03.2014 9:32:19
 *   Author: Oll
 */ 


LDI R16, 0x04
OUT 0x01, R16
OUT 0x02, R16
tsykkel:
JMP tsykkel

