/*
 * ul3.asm
 *
 *  Created: 8.04.2014 21:17:51
 *   Author: Oll
 */



.org 0 jmp program_start
.org 0x0022 jmp compare_match_TC1B_interrupt	; IRQ 0x0022


.org 0x4C program_start:

	//JTAG
	IN R17, MCUCR
	LDI R16, (1 << JTD)
	OR R16, R17
	OUT MCUCR, R16
	OUT MCUCR, R16

	//STACK
	LDI R16, 0xFF			; initialising Stack Pointer to 0x10FF (using 16 bits)
	OUT SPL, R16 
	LDI R16, 0x10 
	OUT SPH, R16

	; setting pull-ups
	LDI R16, (1 << PORTF5)	
	OUT PORTF, R16

	//TIMER-COUNTER
	LDI R16, (1 << CS11 | 1 << WGM12)	; set clock to 1/256 CPU clock
	STS TCCR1B, R16

	LDI R20, 0x10			; load number into compare match
	LDI R21, 0xFF
	STS OCR1AH, R21
	STS OCR1AL, R20
	
	//LED INITIALISATION
	LDI R16, 0x01
	OUT DDRA, R16			; setting PORTA pin 1 to output

	LDI R16, 0x01
	OUT PORTA, R16			; lighting one LED

	//INTERRUPTS
	SEI						; global interrupt enable

	LDI R16, (1 << OCIE1A)
	STS TIMSK1, R16			; Timer/Counter1 Interrupt Mask Register, Compare match A interrupt enable



	hang:					; just hang like a gangster
		;isLeft:
		IN R17, PINF
		LDI R16, (1 << PF5)				
		AND R16, R17
		BREQ blinkSlower

		;isRight:
		IN R17, PINF
		LDI R16, (1 << PF3)
		AND R16, R17
		BREQ blinkFaster


		RJMP hang



// MAIN INTERRUPT || blink LED 
compare_match_TC1B_interrupt:
	push R1					; salvestame registri mille sisse hakkame lugema SREGi 
	in R1, SREG				; loeme SREGi 
	push R1					; salvestame SREGi väärtuse stacki peale 
	push R16

	IN R16, PORTA
	LDI R17, 0xFF
	EOR R16, R17			; flip LED value

	OUT PORTA, R16			; do sth with LED
	
	pop R16
	pop R1					; loeme salvestatud SREGi väärtuse stackist 
	out SREG, R1			; kirjutame selle SREGi sisse 
	pop R1					; taastame registri väärtust, mis oli enne 
RETI						; MITTE RET!!!! AINULT RETI 


/*change_led_blinking_rate_interrupt:
	//TODO check button address!!!, initialize pull-ups and everything else!!!*/


blinkFaster:
	LDI R16, 0x00
	STS TCNT1H, R16
	STS	TCNT1L, R16
	LDS R16, OCR1AL
	LDS R17, OCR1AH
	LSR R17
	STS OCR1AH, R17
	STS OCR1AL, R16
	Call delayLoop
	JMP hang
	
blinkSlower:
	LDI R16, 0x00
	STS TCNT1H, R16
	STS	TCNT1L, R16
	LDS R16, OCR1AL
	LDS R17, OCR1AH
	LSL R17
	STS OCR1AH, R17			
	STS OCR1AL, R16
	Call delayLoop
	JMP hang

delayLoop:
	IN		R17, PINF
	ANDI	R17, 0b00101000
	CPI		R17, 0b00101000
	BRNE	delayLoop
RET
