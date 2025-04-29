.include "m328pdef.inc"
.org 0x0000
rjmp main

loop:
	LDI R16, 0x25 ; ciclos externos
	LDI R17, 0xF5 ; ciclos internos
	LDI R18, 0xF0 ; delay entre digito
	; qtde. de ciclos de delay_loop1 ~= 208 * R16 * R17
	; valor escolhido: r16 = r17 = 69
	; qtde. de ciclos = 208 * 69 * 69 = 990288 

loop1:
	push r17

	loop2:
		cpi r29, 0x00; compara digito com zero
		breq leddez ; vai para leddez se o digito for zero
		OUT PORTD, r29 ; digito da centena para saída
		sbi PORTB, 0 ; liga led das centenas
		cbi PORTD, 3 ; desliga led das centenas
		cbi PORTD, 2 ; desliga led das centenas

		;------------
		PUSH r18
		delay1:
		DEC R18 
		CPI r18, 0x00 
		BRNE delay1
		POP r18
		;------------

		leddez:
		cpi r29, 0x00 ; se centena = 0, não liga o led
		breq prox
		rjmp mostra
		prox: 
		cpi r30, 0x00 ; se dezena = 0, não liga o led
		breq ledunid

		mostra:
		OUT PORTD, r30 ; dezena para saída
		sbi PORTD, 3
		cbi PORTB, 0
		cbi PORTD, 2

		;------------
		POP R18
		delay2:
		DEC R18 
		CPI r18, 0x00 
		BRNE delay2
		PUSH R18
		;------------

		ledunid:
		OUT PORTD, r31
		sbi PORTD, 2
		cbi PORTB, 0
		cbi PORTD, 3

		;------------
		PUSH R18
		delay3:
		DEC R18
		CPI r18, 0x00 
		BRNE delay3
		POP R18
		;------------

		DEC R17 
		CPI r17, 0x00 
		BRNE LOOP2; volta para delay_loop2 se r16 =\= 0
		;fim do delay_loop2

	pop r17
	dec r16
	cpi r16, 0x00
	brne loop1
	;fim do delay_loop1

	CBI PORTB, 0
	CBI PORTD, 3
	CBI PORTD, 2
	RET

main: 
	LDI R16, 0xF6 ; r16 <- 1111 0110
	OUT DDRD, R16 ; seta PD7, PD6, PD5, PD4 como saída e PD3, PD2

	LDI R16, 0x01 ; r16 <- 0000 0001
	OUT DDRB, R16 ; set PB0

numeros:
	LDI R16,0xFF ; r20 <- primeiro valor(255)
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP

	LDI R16,0x0F ; r20 <- primeiro valor(15)
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP

	LDI R16,0x53 ; r20 <- primeiro valor(83)
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP

	LDI R16,0x64 ; r20 <- primeiro valor(100)
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP

	LDI R16,0x6F ; r20 <- primeiro valor(111)
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP

	LDI R16,0xF0 ; r20 <- primeiro valor(240)
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP

	LDI R16,0x7B ; r20 <- primeiro valor(123)
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP

	LDI R16,0x0A ; r20 <- primeiro valor(10)
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP

	LDI R16,0x11 ; r20 <- primeiro valor(17)
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP

	LDI R16,0xA1 ; r20 <- primeiro valor(161)
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP

	rjmp numeros

bcd:
	clr r29 ; centena
	clr r30 ; dezena
	clr r31 ; unidade
	clr r17

	mov r17,r16 ; copia o número para o registrador auxiliar

	clc
	cpi r17,0x64 ; compara r21 com 100. Se r21 for menor que 100, flag C = 1
	brcs subdez ; pula para subdez se C = 1

	subcent:
	inc r29
	subi r17,0x64 ; r21 - 100
	cpi r17,0x64
	brcc subcent; enquanto r17 > 100, volta pra subcent

	clc
	cpi r17,0x0A ; compara r17 com 10. Se r17 for menor que 10, flag C = 1
	brcs subunidade ; pula se C = 1

	subdez:
	inc r30
	subi r17,0x0A ; r21 - 10
	cpi r17,0x0A
	brcc subdez

	clc
	subunidade:
	mov r31, r17

	;--------------------------------------

	;r17
	;r29 = 0000 abcd
	clc	
	rol r29 ; 000a bcd0
	mov r17, r29
	andi r29, 0xF0 ; 000a 0000

	andi r17, 0x0F ; 0000 bcd0
	clc
	rol r17 ; 000b cd00
	push r17
	andi r17, 0xF0 ; 000b 0000
	clc
	rol r17 ; 00b0 0000
	add r29, r17 ;  00ba 0000

	pop r17 ;  000b cd00
	andi r17, 0x0F ;  0000 cd00
	clc
	rol r17
	push r17
	andi r17, 0xF0 ;  000c 0000
	clc
	rol r17
	rol r17 ;  0c00 0000
	add r29, r17 ;  0cba 0000

	pop r17 ;  000c d000
	andi r17, 0x0F ;  0000 d000
	clc
	rol r17
	rol r17
	rol r17
	rol r17 
	add r29, r17 ;  dcba 0000

	;------------------------------------------

	clc	
	rol r30 ;r21 = 000a bcd0
	mov r17, r30 ; r24 = r21
	andi r30, 0xF0 ; r21 = 000a 0000

	andi r17, 0x0F ; r24 = 0000 bcd0
	clc
	rol r17 ; r24 = 000b cd00
	push r17
	andi r17, 0xF0 ; r24 = 000b 0000
	clc
	rol r17 ; r24 = 00b0 0000
	add r30, r17 ; r21 = 00ba 0000

	pop r17 ; r24 = 000b cd00
	andi r17, 0x0F ; r24 = 0000 cd00
	clc
	rol r17
	push r17
	andi r17, 0xF0 ; r24 = 000c 0000
	clc
	rol r17
	rol r17 ; r24 = 0c00 0000
	add r30, r17 ; r24 = 0cba 0000

	pop r17 ; r24 = 000c d000
	andi r17, 0x0F ; r24 = 0000 d000
	clc
	rol r17
	rol r17
	rol r17
	rol r17 
	add r30, r17 ; r24 = dcba 0000

	;----------------------------------------

	clc	
	rol r31 ;r21 = 000a bcd0
	mov r17, r31 ; r24 = r21
	andi r31, 0xF0 ; r21 = 000a 0000

	andi r17, 0x0F ; r24 = 0000 bcd0
	clc
	rol r17 ; r24 = 000b cd00
	push r17
	andi r17, 0xF0 ; r24 = 000b 0000
	clc
	rol r17 ; r24 = 00b0 0000
	add r31, r17 ; r21 = 00ba 0000

	pop r17 ; r24 = 000b cd00
	andi r17, 0x0F ; r24 = 0000 cd00
	clc
	rol r17
	push r17
	andi r17, 0xF0 ; r24 = 000c 0000
	clc
	rol r17
	rol r17 ; r24 = 0c00 0000
	add r31, r17 ; r24 = 0cba 0000

	pop r17 ; r24 = 000c d000
	andi r17, 0x0F ; r24 = 0000 d000
	clc
	rol r17
	rol r17
	rol r17
	rol r17 
	add r31, r17 ; r24 = dcba 0000
	;-----------------------------------------------

	ret
