/*
 * ul2.asm
 *
 *  Created: 17.03.2014 11:38:19
 *   Author: Oll
 */ 


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

main:
	ldi R20, HIGH(1000)
	ldi R19, LOW(1000)
	delay_s:
		CALL delayLoop
		SUBI R19, 1
		SBCI R20, 1
		BRNE delay_s
	rjmp main


 delayLoop:
	PUSH R16
	PUSH R17

	
	LDI R16, 0
	STS TCCR1A, R16			; initializing timer values
	STS TCCR1C, R16			; initializing timer values
	LDI R16, (1 << CS10)	
	STS TCCR1B, R16			; setting default clock (CPU clock)

	LDI R17, (1 << WGM12)
	OR R16, R17				; adding compare match mode to TCCR1B (CTC)
	STS TCCR1B, R16

	LDI R16, 0x00
	STS OCR1AH, R16			; loading "2000" into compare match register
	LDI R16, 0x10			; high byte first!
	STS OCR1AL, R16 

	loop2:
		LDI R16, 0x02
		IN R17, TIFR1
		AND R16, R17
		;CPI R16, 0x02
		BREQ loop2

	POP R17
	POP R16
ret





	


 

