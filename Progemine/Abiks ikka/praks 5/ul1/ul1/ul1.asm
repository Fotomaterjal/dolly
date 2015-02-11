/*
 * ul1.asm
 *
 *  Created: 7.04.2014 11:14:16
 *   Author: Oll
 */ 

.org 0 jmp program_start
.org 0x0028 jmp compare_match_TC1B_interrupt	; IRQ 0x0028


.org 0x4C
program_start:
	//STACK
	LDI R16, 0xFF			; initialising Stack Pointer to 0x10FF (using 16 bits)
	OUT SPL, R16 
	LDI R16, 0x10 
	OUT SPH, R16
	
	//TIMER
	LDI R16, (1 << CS10)	; set timer clock speed
	STS TCCR1B, R16
	
	//INTERRUPTS
	LDI R16, (1 << TOIE1)	; ovf interrupt enable
	STS TIMSK1, R16

	LDI R16, (1 << OCIE1B)
	STS TIMSK1, R16			; Timer/Counter1 Interrupt Mask Register, Compare match B interrupt enable
	
	SEI						; global interrupt enable

	//LED INITIALISATION
	LDI R16, 0x01
	OUT DDRA, R16			; setting PORTA pin 1 to output

	LDI R16, 0x01
	OUT PORTA, R16			; lighting one LED



	

	hang:					; just hang like a gangster
		RJMP hang



// MAIN INTERRUPT
compare_match_TC1B_interrupt:
	push R1					; salvestame registri mille sisse hakkame lugema SREGi 
	in R1, SREG				; loeme SREGi 
	push R1					; salvestame SREGi väärtuse stacki peale 
	push R16

	IN R16, PORTA
	LDI R17, 0xFF
	EOR R16, R17			; flip LED value

	OUT PORTA, R16			; do sth with LED

	continue:
	pop R16
	pop R1					; loeme salvestatud SREGi väärtuse stackist 
	out SREG, R1			; kirjutame selle SREGi sisse 
	pop R1					; taastame registri väärtust, mis oli enne 
RETI						; MITTE RET!!!! AINULT RETI 
