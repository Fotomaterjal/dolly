/*
 * AssemblerApplication1.asm
 *
 *  Created: 3.03.2014 8:59:44
 *   Author: Oll
 */ 


LDI r16,0xFF
OUT 0x25, R16
OUT 0x05, R16
LDI r17, 0xFA
OUT 0x25, R17
OUT 0x05, R17
NOP
JMP 0