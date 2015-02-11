/*
 * ul6.asm
 *
 *  Created: 3.03.2014 11:56:33
 *   Author: Oll
 */ 


LDI R16, 0x04 
OUT 0x04, R16 ; DDRB

 
Main_loop: ; andsime programmikohale nime “Main_loop” ,
	EOR R16, 0x07 ;ERROR
	OUT 0x05, R16 ; PORTB 
	LDI R17, 0x00 ; viivis algas 
	LDI R18, 0x00
	LDI R19, 0x00
 
 
	/*delay_loop:
		delay_loop2:
			delay_loop3:
				NOP
				NOP
				NOP
				NOP
				INC R19
				CPI R19, 0x90
				BRNE delay_loop3
			
			INC R18
			CPI R18, 0xE8
			BRNE delay_loop2

		INC R17
		CPI R17, 0x02		
		BRNE delay_loop*/
		/*kui lahutamise tulemus (v6rdlemine)
		on 0, siis Zero flag l2heb pusti.
		BRNE kontrolli Zero flagi*/
			
RJMP Main_loop 

/*Nyyd tootab koige kiirem LED kiirusega 2 Hz*/