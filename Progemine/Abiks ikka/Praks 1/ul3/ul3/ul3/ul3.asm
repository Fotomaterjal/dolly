/*
 * ul3.asm
 *
 *  Created: 3.03.2014 10:21:12
 *   Author: Oll
 */ 

/*LDI r16,0xFF 
OUT 0x01, R16
OUT 0x02, R16
INC R16
LDI R17,0xFF
NOP
NOP 
DEC R17
BRNE 5
RJMP 2*/

LDI R16, 0xFF 
OUT 0x01, R16 ; DDRB 
 
Main_loop: ; andsime programmikohale nime “Main_loop” 
	OUT 0x02, R16 ; PORTB 
	INC R16 
 
	LDI R17, 0x00 ; viivis algas 
 
delay_loop: 
	NOP ; no operation – aja raiskamine :) 
	NOP 
	INC R17 
	CPI R17, 0xFF 
	
	/*kui lahutamise tulemus (v6rdlemine)
	on 0, siis Zero flag l2heb pusti.
	BRNE kontrolli Zero flagi*/


	BRNE delay_loop

	
RJMP Main_loop 

