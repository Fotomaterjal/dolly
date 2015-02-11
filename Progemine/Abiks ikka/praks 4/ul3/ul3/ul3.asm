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
LDI R16, (1 << PF5 | 1 << PF3)	
OUT PORTF, R16

; lighting one LED
LDI R18, 0x01
OUT PORTA, R18


; R18 reserved for LED number
main:
	;isLeft:
		IN R17, PINF
		LDI R16, (1 << PF5)				
		AND R16, R17
		BREQ LED_change_left

	;isRight:
		IN R17, PINF
		LDI R16, (1 << PF3)
		AND R16, R17
		BREQ LED_change_right
	JMP main

LED_change_left:
	LSL R18				; logical shift left, shift one bit a little left
	OUT PORTA, R18		; light that new LED
	CALL delayLoop		; delay for slow hands	
	cpi R18, 0			; if one LED is alive, jmp back to main
	
	BRNE main
	LDI R18, 0x01		; if not, light the rightmost LED
	OUT PORTA, R18
	
JMP main				; go back to main

LED_change_right:
	LSR R18				; logical shift left, shift one bit a little right
	OUT PORTA, R18		; light that new LED
	CALL delayLoop		; delay for slow hands
	LDI R19, 0xFF		
	AND R18, R19		; if one LED is alive, jmp back to main
	
	BRNE main
	LDI R18, 0x80		; if not, light the leftmost LED
	OUT PORTA, R18
	
JMP main				; go back to main

	
delayLoop:
	IN		R17, PINF
	ANDI	R17, 0b00101000
	CPI		R17, 0b00101000
	BRNE	delayLoop
RET