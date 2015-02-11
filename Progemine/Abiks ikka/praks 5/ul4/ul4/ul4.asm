/*
 * ul4.asm
 *
 *  Created: 8.04.2014 21:17:51
 *   Author: Oll
 */



.org 0 jmp program_start
.org 0x0022 jmp compare_match_TC1A_interrupt	; IRQ 0x0022
.org 0x0032 jmp uart_interrupt					; USART1 Rx Complete


.org 0x4C program_start:


	//STACK
	LDI R16, 0x10									; initialising Stack Pointer to 0x10FF (using 16 bits)
	OUT SPH, R16 
	LDI R16, 0xFF 
	OUT SPL, R16

	//TIMER-COUNTER
	LDI R16, (1<<CS10) | (1<<CS12)| (1 << WGM12)	; setting clock to 1/1024 CPU clock
	STS TCCR1B, R16

	LDI R20, LOW(1953)		; load number into compare match, 0.5 Hz
	LDI R21, HIGH(1953)
	STS OCR1AH, R21
	STS OCR1AL, R20
	
	//LED INITIALISATION
	LDI R16, 0x01
	OUT DDRA, R16			; setting PORTA pin 0 to output

	LDI R16, 0x01
	OUT PORTA, R16			; lighting LED 0

	//UART
	LDI	R17, HIGH(25)		; setting baud rate to 9600
	LDI	R16, LOW(25)
	STS	UBRR1H, R17
	STS	UBRR1L, R16
	LDI	R16, (1 << U2X1)
	STS	UCSR1A, R16
											; by default, parity is none
	LDI R16, (1 << UCSZ10 | 1 << UCSZ11)	; setting up 8 data bits
	STS	UCSR1C, R16

	LDI R16, (1 << RXEN1| 1 << RXCIE1 | 1 << TXEN1)	; enabling reciever, transmitter, enabling interrupt
	STS UCSR1B, R16

	//INTERRUPTS
	SEI						; global interrupt enable

	LDI R16, (1 << OCIE1A)
	STS TIMSK1, R16			; Timer/Counter1 Interrupt Mask Register, Compare match A interrupt enable

	hang:
		RJMP hang


// MAIN INTERRUPT || blink LED 
compare_match_TC1A_interrupt:
	push R1					; salvestame registri mille sisse hakkame lugema SREGi 
	in R1, SREG				; loeme SREGi 
	push R1					; salvestame SREGi väärtuse stacki peale 
	push R16

	IN R16, PORTA
	LDI R17, 0x01
	EOR R16, R17			; flip LED value

	OUT PORTA, R16			; do sth with LED
	
	pop R16
	pop R1					; loeme salvestatud SREGi väärtuse stackist 
	out SREG, R1			; kirjutame selle SREGi sisse 
	pop R1					; taastame registri väärtust, mis oli enne 
RETI						; MITTE RET!!!! AINULT RETI 


//UART INTERRUPT, uses R21 and R22
uart_interrupt:
	push R1					; salvestame registri mille sisse hakkame lugema SREGi 
	in R1, SREG				; loeme SREGi 
	push R1					; salvestame SREGi väärtuse stacki peale 

	push R16
	push R17
	push R18
	push R20
	push R21
	push R22
	
	LDS R16, UDR1
	CALL sendLetter			; for testing purposes

	LDI R16, 0
	STS TCNT1H, R16			; setting timer count to 0
	STS TCNT1L, R16

	ldi r23, (1<<OCIE1A)
	sts TIMSK1, r23			; enable timer interrupts


	LDS R16, UDR1			; read UART input

	zero:
		LDI R18, '0'
		CP R16, r18
		BRNE one			; branch to one if not '0'
		ldi r22, 0			
		sts TIMSK1, r22		; disable timer interrupts
		out PORTA, r22		; write 0 to PORTA
		rjmp write_values

	one:
		LDI R18, '1'
		CP R16, r18
		BRNE two			; branch to two if not equal
		ldi R20, LOW(977)	; f = 1Hz
		ldi R21, HIGH(977)
		rjmp write_values

	two:
		ldi R18, '2'
		cp R16, R18
		brne three
		ldi R20, LOW(488)	; f = 2Hz
		ldi R21, HIGH(488)
		rjmp write_values

	three:
		ldi R18, '3'
		cp R16, R18
		brne four
		ldi R20, LOW(326)	; f = 3Hz
		ldi R21, HIGH(326)
		rjmp write_values

	four:
		ldi R18, '4'
		cp R16, R18
		brne five
		ldi R20, LOW(244)	; f = 4Hz
		ldi R21, HIGH(244)
		rjmp write_values

	five:
		ldi R18, '5'
		cp R16, R18
		brne six
		ldi R20, LOW(195)	; f = 5Hz
		ldi R21, HIGH(195)
		rjmp write_values

	six:
		ldi R18, '6'
		cp R16, r18
		brne seven
		ldi R20, LOW(163)	; f = 6Hz
		ldi R21, HIGH(163)
		rjmp write_values

	seven:
		ldi R18, '7'
		cp R16, r18
		brne eight
		ldi R20, LOW(140)	; f = 7Hz
		ldi R21, HIGH(140)
		rjmp write_values

	eight:
		ldi R18, '8'
		cp R16, r18
		brne nine
		ldi R20, LOW(122)	; f = 8Hz
		ldi R21, HIGH(122)
		rjmp write_values

	nine:
		ldi R18, '9'
		cp R16, R18
		brne xten
		ldi R20, LOW(109)	; f = 9Hz
		ldi R21, HIGH(109)
		rjmp write_values

	xten:
		ldi R18, 'x'
		cp R16, R18
		ldi R20, 0x01 
		ldi R21, 0x00
		sts TIMSK1, r21		; disable timer interrupts
		ldi r22, 0x01
		out PORTA, r22		; write 0x01 to PORTA


	write_values:			; set new compare match values
		STS OCR1AH, R21
		STS OCR1AL, R20

//////////////////////////////////////////////////////////////////////////
	pop R21
	pop R21
	pop R20
	pop R18
	pop R17
	pop R16
	pop R1
	out SREG, R1
	pop R1					; salvestame registri mille sisse hakkame lugema SREGi

RETI						; MITTE RET!!!! AINULT RETI 


sendLetter:					; for feedback
	push R18
	push R17
	STS UDR1, R16

	LDI R16, (1 << TXC1)	; checking TX complete flag
	wait:
		LDS R17, UCSR1A
		AND R17, R16	
	BREQ wait

	LDS R17, UCSR1A
	OR R17, R16
	STS UCSR1A, R17			; reinitializing TX complete flag

	pop R17
	pop R18
RET

