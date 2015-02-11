/*
 * ul7.asm
 *
 *  Created: 17.03.2014 8:27:16
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
		CALL delay_16bit
		INC r16
		CPI r16, 10
	BRNE loop4



RJMP main


delay_16bit:				; DOES R17x256 + R16 1ms delays
	PUSH R16
	PUSH R17

	LDI R16, 0xF4			; CHANGE VALUE IF NEEDED
	LDI R17, 0x01			; CHANGE VALUE IF NEEDED

	iterator:
		CALL delay_1ms
		SUBI R16, 0x01
		SBCI R17, 0x00
		BRNE iterator
	
	POP R17
	POP R16
RET


 delay_1ms:
	PUSH r16
	LDI R16, 0xFA
	loop:	
		NOP
		NOP
		NOP
		NOP
		NOP				
		DEC R16
		BRNE loop
	POP r16
RET

shortSignalProcedure:
	PUSH r16
	LDI r16, 0xFF
	OUT 0x02, r16			;LED põlema
	CALL delay_16bit	
	LDI r16, 0x00
	OUT 0x02, r16			;LED kustu
	CALL delay_16bit
	POP r16
RET

longSignalProcedure:
	PUSH r16
	LDI r16, 0xFF
	OUT 0x02, r16			;LED põlema
	CALL delay_16bit
	CALL delay_16bit
	CALL delay_16bit	
	LDI r16, 0x00
	OUT 0x02, r16			;LED kustu
	CALL delay_16bit
	POP r16
RET


