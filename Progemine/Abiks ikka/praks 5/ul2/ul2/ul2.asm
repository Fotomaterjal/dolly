/*
 * ul2.asm
 *
 *  Created: 8.04.2014 20:36:27
 *   Author: Oll
 */ 

.org 0 jmp program_start

.org 0x0028 jmp compare_match_TC1B_interrupt	; IRQ 0x0028


.org 0x4C program_start:
	//STACK
	LDI R16, 0xFF			; initialising Stack Pointer to 0x10FF (using 16 bits)
	OUT SPL, R16 
	LDI R16, 0x10 
	OUT SPH, R16

	//LED INITIALISATION
	LDI R16, 0x01
	OUT DDRA, R16			; setting PORTA pin 1 to output

	LDI R16, 0x01
	OUT PORTA, R16			; lighting one LED

	//TIMER
	LDI R16, (1 << CS10 | 1 << CS11)	; set timer clock speed
	STS TCCR1B, R16
	
	//INTERRUPTS
	LDI R16, (1 << TOIE1)	; ovf interrupt enable
	STS TIMSK1, R16

	LDI R16, (1 << OCIE1B)
	STS TIMSK1, R16			; Timer/Counter1 Interrupt Mask Register, Compare match B interrupt enable
	
	SEI						; global interrupt enable

	//UART
	LDI	R16, 0x00			; setting baud rate to 9600
	LDI	R17, 0x19
	STS	UBRR1H, R16
	STS	UBRR1L, R17
	LDI	R16, (1 << U2X1)
	STS	UCSR1A, R16
											; by default, parity is none
	LDI R16, (1 << UCSZ10 | 1 << UCSZ11)	; setting up 8 data bits
	STS	UCSR1C, R16

	LDI R16, (1 << TXEN1)	; enabling transmitter
	STS UCSR1B, R16


	hang:					; just hang like a gangster
		CALL sendName		; also send my name to UART
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


sendName:
	PUSH R24
	LDI		R24, 'S'
	CALL sendLetter
	LDI		R24, 'T'
	CALL sendLetter
	LDI		R24, 'E'
	CALL sendLetter
	LDI		R24, 'N'
	CALL sendLetter
	LDI		R24, '-'
	CALL sendLetter
	LDI		R24, 'O'
	CALL sendLetter
	LDI		R24, 'L'
	CALL sendLetter
	LDI		R24, 'I'
	CALL sendLetter
	LDI		R24, 'V'
	CALL sendLetter
	LDI		R24, 'E'
	CALL sendLetter
	LDI		R24, 'R'
	CALL sendLetter

	POP R24
RET

sendLetter:	
	STS UDR1, R24

	LDI R16, (1 << TXC1)	; checking TX complete flag
	wait:
		LDS R17, UCSR1A
		AND R17, R16	
	BREQ wait
	LDS R17, UCSR1A
	OR R17, R16
	STS UCSR1A, R17			; reinitializing TX complete flag
RET