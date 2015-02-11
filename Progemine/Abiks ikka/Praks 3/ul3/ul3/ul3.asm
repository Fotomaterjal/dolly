/*
 * ul3.asm
 *
 *  Created: 24.03.2014 9:05:07
 *   Author: Oll
 */ 

; initialising stack pointer to first byte
LDI	R16, 0x10
OUT	SPH, R16
LDI	R16, 0XFF
OUT	SPL, R16

; setting baud rate to 9600
LDI	R16, 0x00
LDI	R17, 0x19
STS	UBRR1H, R16
STS	UBRR1L, R17
LDI	R16, (1 << U2X1)
STS	UCSR1A, R16

; setting parity to none
LDI R16, (1 << UPM10)
LDI R17, (1 << UPM11)
OR R16, R17
STS UCSR1C, R16

/*; setting 1 stop bit
LDI R16, (0 << USBS0)
OR R16, UCSR0C
STS UCSR0C, R16*/

; setting up 8 data bits
LDI R16, (1 << UCSZ10)
LDI R17, (1 << UCSZ11)
OR R16, R17
STS	UCSR1C, R16

; enabling transmitter
LDI R16, (1 << TXEN1)
STS UCSR1B, R16

sendName:
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

lopp:
	RJMP lopp

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
