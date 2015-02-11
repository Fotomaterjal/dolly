/*
 * ul2.asm
 *
 *  Created: 31.03.2014 10:23:03
 *   Author: Oll
 */ 

; STACK POINTER
; initialising Stack Pointer to 0x10FF (using 16 bits)
LDI R16, 0xFF 
OUT SPL, R16 
LDI R16, 0x10 
OUT SPH, R16

; ADC
; selecting reference voltage VCC, measure voltage on pin ADC2
LDI R16, (1 << REFS0 | 1 << MUX1)
STS ADMUX, R16

; ADC enable
LDI R16, (1 << ADEN)	
STS ADCSRA, R16

; setting PORTB pins to output
LDI R16, 1 << PORTB5
OUT DDRB, R16
  

; TIMER
LDI R16, 0
STS TCCR1A, R16						; initializing timer values
STS TCCR1C, R16						; initializing timer values
LDI R16, (1 << CS10 | 1 << WGM12)	; setting default clock (CPU clock)
STS TCCR1B, R16						; adding compare match mode to TCCR1B (CTC)

LDI R16, (1<<WGM11 | 1<<WGM10 | 1<<COM1A1)	; setting TCCR1 to "Fast PWM 10bit"
STS TCCR1A, R16											; setting OC1A low after compare match
  
PWMLoop:
	CALL measure_voltage
	STS OCR1AH, R19
	STS OCR1AL, R18

JMP PWMLoop


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
	;OUT PORTA, R18

	POP R17
	POP R16
RET

	


	

