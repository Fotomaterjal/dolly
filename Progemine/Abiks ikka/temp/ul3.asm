/*
 * ul3.asm
 *
 *  Created: 7.04.2014 8:32:49
 *   Author: Oll
 */ 

 IN R16, MCUCR
 LDI R17, (1<<JTD)
 OR R16, R17
 OUT MCUCR, R16
 OUT MCUCR, R16

 ; STACK POINTER
; initialising Stack Pointer to 0x10FF (using 16 bits)
LDI R16, 0xFF 
OUT SPL, R16 
LDI R16, 0x10 
OUT SPH, R16

; setting PORTA pins to output
LDI R16, 0xFF
OUT DDRA, R16

; setting pull-ups
LDI R16, (1 << DDF5)
OUT PORTF, R16

; lighting one LED
LDI R18, 0x01
OUT PORTA, R18


; R18 reserved for LED number
main:

	LDI R16, (1<< PF5)		; making the 5th bit '1' to check left_push
	IN R17, PINF
	AND	R16, R17		; comparing two registers
	BRNE main			; if result is not 0, jump back to main and check PF5 again
	JMP LED_change_left		; else: PF5 was '1', change to next LED

RJMP main


LED_change_left:
	LSL R18				; logical shift left, shift one bit a little left
	OUT PORTA, R18		; light that new LED
	CALL delayLoop		; delay for slow hands
	LDI R19, 0xFF		
	AND R18, R19		; if one LED is alive, jmp back to main
	
	BRNE main
	LDI R18, 0x01		; if not, light the rightmost LED
	OUT PORTA, R18
	
	JMP main			; go back to main
	
delayLoop:
	IN		R17, PINF
	ANDI	R17, 0b00101000
	CPI		R17, 0b00101000
	BRNE	delayLoop
	RET