/*
 * ul6.asm
 *
 *  Created: 10.03.2014 10:18:28
 *   Author: Oll
 */ 

 ;initialising Stack Pointer to 0x10FF (using 16 bits)
LDI R16, 0xFF 
OUT SPL, R16 
LDI R16, 0x10 
OUT SPH, R16

LDI r16, 0xFF
OUT 0x01, r16			;PORTA väljundiks kõik LED-id
main:

	LDI r16, 0
	loop1:
		CALL shortSignalProcedure
		INC r16
		CPI r16, 3
	BRNE loop1

	LDI r16, 0
	loop2:
		CALL longSignalProcedure
		INC r16
		CPI r16, 3
	BRNE loop2

	LDI r16, 0
	loop3:
		CALL shortSignalProcedure
		INC r16
		CPI r16, 3
	BRNE loop3

	LDI r16, 0
	loop4:
		CALL delay_procedure
		INC r16
		CPI r16, 10
	BRNE loop4



RJMP main

delay_procedure:
	PUSH r16
	PUSH r17
	PUSH r18

	LDI R16, 0x00
	delay_helper1:

		LDI R17, 0x00
		delay_helper2:			

			LDI R18, 0x00
			delay_helper3:
				INC R18
				CPI R18, 0xFF
			BRNE delay_helper3

			INC R17
			CPI R17, 0xFA
		BRNE delay_helper2
	
	INC R16
	CPI R16, 0x02
	BRNE delay_helper1		

	POP r18
	POP r17
	POP r16

RET

shortSignalProcedure:
	PUSH r16
	LDI r16, 0xFF
	OUT 0x02, r16			;LED põlema
	CALL delay_procedure	
	LDI r16, 0x00
	OUT 0x02, r16			;LED kustu
	CALL delay_procedure
	POP r16
RET

longSignalProcedure:
	PUSH r16
	LDI r16, 0xFF
	OUT 0x02, r16			;LED põlema
	CALL delay_procedure
	CALL delay_procedure
	CALL delay_procedure	
	LDI r16, 0x00
	OUT 0x02, r16			;LED kustu
	CALL delay_procedure
	POP r16
RET


