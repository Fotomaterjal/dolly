/*
 * ul1.asm
 *
 *  Created: 24.03.2014 10:40:44
 *   Author: Oll
 */ 

; STACK POINTER
; initialising stack pointer to first byte
LDI	R16, 0x10
OUT	SPH, R16
LDI	R16, 0XFF
OUT	SPL, R16

; UART
; setting baud rate to 9600
LDI	R17, 0
LDI	R16, 25
STS	UBRR1H, R17
STS	UBRR1L, R16
LDI	R16, (1 << U2X1)
STS	UCSR1A, R16

; setting parity to none
LDI R16, (1 << UPM10) | (1 << UPM11)
STS UCSR1C, R16

/*; setting 1 stop bit
LDI R16, (0 << USBS0)
OR R16, UCSR1C
STS UCSR1C, R16*/

; setting up 8 data bits
LDI R16, (1 << UCSZ10) | (1 << UCSZ11)
STS	UCSR1C, R16

; enabling transmitter
LDI R16, (1 << TXEN1)
STS UCSR1B, R16



; ADC
; selecting reference voltage VCC, measure voltage on pin ADC2
LDI R16, (1 << REFS0 | 1 << MUX1)
STS ADMUX, R16

; ADC enable
LDI R16, (1 << ADEN)	
STS ADCSRA, R16

; setting PORTA pins to output
LDI R16, 0xFF
OUT DDRA, R16


main:	
	CALL measure_voltage
	
	LDI R20, '0'
	LDI R16, LOW(101)
	LDI R17, HIGH(101)
	CP R18, R16
	CPC R19, R17
	BRLO sendVoltage

	LDI R20, '1'
	LDI R16, LOW(201)
	LDI R17, HIGH(201)
	CP R18, R16
	CPC R19, R17
	BRLO sendVoltage

	LDI R20, '2'
	LDI R16, LOW(301)
	LDI R17, HIGH(301)
	CP R18, R16
	CPC R19, R17
	BRLO sendVoltage

	LDI R20, '3'
	LDI R16, LOW(401)
	LDI R17, HIGH(401)
	CP R18, R16
	CPC R19, R17
	BRLO sendVoltage

	LDI R20, '4'
	LDI R16, LOW(501)
	LDI R17, HIGH(501)
	CP R18, R16
	CPC R19, R17
	BRLO sendVoltage

	LDI R20, '5'
	LDI R16, LOW(601)
	LDI R17, HIGH(601)
	CP R18, R16
	CPC R19, R17
	BRLO sendVoltage

	LDI R20, '6'
	LDI R16, LOW(701)
	LDI R17, HIGH(701)
	CP R18, R16
	CPC R19, R17
	BRLO sendVoltage

	LDI R20, '7'
	LDI R16, LOW(801)
	LDI R17, HIGH(801)
	CP R18, R16
	CPC R19, R17
	BRLO sendVoltage

	LDI R20, '8'
	LDI R16, LOW(901)
	LDI R17, HIGH(901)
	CP R18, R16
	CPC R19, R17
	BRLO sendVoltage

	LDI R20, '9'

sendVoltage:	
	PUSH R16
	PUSH R17

	STS UDR1, R20

	LDI R16, (1 << TXC1)	; checking TX complete flag
	wait:
		LDS R17, UCSR1A
		AND R17, R16	
	BREQ wait
	LDS R17, UCSR1A
	OR R17, R16
	STS UCSR1A, R17			; clearing TX complete flag

	POP R17
	POP R16
JMP main

; returns ADC value in R19:R18
measure_voltage:
	PUSH R16
	PUSH R17
	
	LDI R16, (1 << ADEN | 1 << ADSC)	;ADC enable, ADC start conversion
	STS ADCSRA, R16

	LDI R16, (1 << ADIF)				; mask for checking ADIF flag
	clear_delay:
		LDS R17, ADCSRA
		AND R17, R16
	BREQ clear_delay					; if ADIF == 0: continue
	STS ADCSRA, R16						; reset ADIF flag

	LDS R18, ADCL						; read ADC value
	LDS R19, ADCH
	OUT PORTA, R18

	POP R17
	POP R16
RET

	




