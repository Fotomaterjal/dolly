.ORG 0x00
	JMP main
.ORG 0x002A
	JMP aegVilgutada
.ORG 0x0032
	JMP aegMuutaKiirust
.ORG 0x004C

main:
	LDI		R16, 0x10
	LDI		R17, 0xFF
	LDI		R18, 0b00000100
	OUT		SPH, R16
	OUT		SPL, R17
	OUT		DDRA, R18
	OUT		DDRB, R17

	LDI		R16, 0b00000010	;TCCR0A
	LDI		R17, 0b00000101 ;TCCR0B
	LDI		R18, 0xFF		;OCR0A
	LDI		R19, 0b00000010	;TIMSK0
	OUT		TCCR0A, R16
	OUT		TCCR0B, R17
	OUT		OCR0A, R18
	STS		TIMSK0, R19

	LDI		R16, 0b10010000	;UCSR1B
	LDI		R17, 0b00000110 ;UCSR1C
	LDI		R18, 0x00		;UBRR1H
	LDI		R19, 0x0C		;UBRR1L
	STS		UCSR1B, R16
	STS		UCSR1C, R17
	STS		UBRR1H, R18
	STS		UBRR1L, R19

	SEI

	LOOP:
		JMP LOOP

aegVilgutada:
	PUSH	R16
	IN		R16, SREG
	PUSH	R16

	INC		R20
	OUT		PORTA, R20

	POP		R16
	OUT		SREG, R16
	POP		R16
	RETI


aegMuutaKiirust:
	PUSH	R16
	IN		R16, SREG
	PUSH	R16

	LDS		R16, UDR1
	CPI		R16, '0'
	BREQ	A0		
	CPI		R16, '1'
	BREQ	A1		
	CPI		R16, '2'
	BREQ	A2		
	CPI		R16, '3'
	BREQ	A3	
	CPI		R16, '4'
	BREQ	A4		
	CPI		R16, '5'
	BREQ	A5		
	CPI		R16, '6'
	BREQ	A6		
	CPI		R16, '7'
	BREQ	A7		
	CPI		R16, '8'
	BREQ	A8		
	CPI		R16, '9'
	BREQ	A9		
	CPI		R16, 'X'
	BREQ	AX
	RJMP	ALOPP	
	
	A0:
		LDI		R21, 0b00010000
		OUT		PORTB, R21
		LDI		R16, 0X00
		OUT		TCCR0B, R16
		OUT		PORTA, R16
		JMP ALOPP

	A1:
		LDI		R16, 0xFF
		OUT		OCR0A, R16
		JMP		ALOPP
	A2:
		LDI		R16, 0x7F
		OUT		OCR0A, R16
		JMP		ALOPP
	A3:
		LDI		R16, 0x55
		OUT		OCR0A, R16
		JMP		ALOPP
	A4:
		LDI		R16, 0x3F
		OUT		OCR0A, R16
		JMP		ALOPP
	A5:
		LDI		R16, 0x33
		OUT		OCR0A, R16
		JMP		ALOPP
	A6:
		LDI		R16, 0x2A
		OUT		OCR0A, R16
		JMP		ALOPP
	A7:
		LDI		R16, 0x24
		OUT		OCR0A, R16
		JMP		ALOPP
	A8:
		LDI		R16, 0x1F
		OUT		OCR0A, R16
		JMP		ALOPP
	A9:
		LDI		R16, 0x1C
		OUT		OCR0A, R16
		JMP		ALOPP
	AX:
		LDI		R16, 0X00
		OUT		TCCR0B, R16
		LDI		R16, 0B00000100
		OUT		PORTA, R16
		

	ALOPP:	
	POP		R16
	OUT		SREG, R16
	POP		R16
	RETI
	