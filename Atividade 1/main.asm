.include "m328pdef.inc"
.org 0x0000
rjmp main

loop:
	LDI R16, 0x25 ; ciclos de loop1
	LDI R17, 0xF5 ; ciclos de loop2
	LDI R18, 0xF0 ; delay entre cada digito
	; qtde. de ciclos em que o número é mostrado ~= 208 * R16 * R17

loop1:
	push r17 ;salva r17 no stack

	loop2:
		cpi r29, 0x00; compara centena com zero
		breq leddez ; vai para leddez se o digito for zero

		ledcent:
		OUT PORTD, r29 ; digito da centena para led7seg
		sbi PORTB, 0 ; liga led das centenas
		cbi PORTD, 3 ; desliga led das centenas
		cbi PORTD, 2 ; desliga led das centenas

		;------------
		PUSH r18 ;guarda qtde de delay no stack
		delay1:
		DEC R18 
		CPI r18, 0x00 
		BRNE delay1
		POP r18 ; recupera a qtde. e guarda em r18
		;------------

		leddez:
		cpi r29, 0x00 ; se menor que 100, vai verificar se menor que 10
		breq menor_cem
		rjmp mostra_dezena ; se maior que 100, liga o led da dezena

		menor_cem: 
		cpi r30, 0x00 ; se menor que 10, não liga o led das dezenas
		breq ledunid

		mostra_dezena:
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
		OUT PORTD, r31 ; sempre bota unidade para a saída
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
		BRNE LOOP2; volta para delay_loop2 se r17 =\= 0
		;fim do delay_loop2

	pop r17
	dec r16
	cpi r16, 0x00
	brne loop1; volta para delay_loop1 se r16 =\= 0
	;fim do delay_loop1

	;desliga os 3 leds de 7seg
	CBI PORTB, 0 
	CBI PORTD, 3
	CBI PORTD, 2
	RET

main: 
	LDI R16, 0xF6 ; r16 <- 1111 0110
	OUT DDRD, R16 ; coloca PD7, PD6, PD5, PD4, PD2 e PD1 como saída

	LDI R16, 0x01 ; r16 <- 0000 0001
	OUT DDRB, R16 ; set PB0 como saída

numeros:
	LDI R16,0xA0 ; r16 <- 0
	RCALL BCD ; pula para a subrotina BCD
	RCALL LOOP ; pula para a subrotina LOOP, onde será colocado nos leds os dígitos

	LDI R16,0x02 ; r16 <- 2
	RCALL BCD 
	RCALL LOOP

	LDI R16,0x03 ; r16 <- 3
	RCALL BCD 
	RCALL LOOP

	LDI R16,0x64 ; r16 <- 100
	RCALL BCD 
	RCALL LOOP

	LDI R16,0x6F ; r16 <- 111
	RCALL BCD 
	RCALL LOOP

	LDI R16,0xF0 ; r16 <- 240
	RCALL BCD
	RCALL LOOP

	LDI R16,0x7B ; r16 <- 123
	RCALL BCD
	RCALL LOOP

	LDI R16,0x0A ; r16 <- 10
	RCALL BCD
	RCALL LOOP

	LDI R16,0x11 ; r16 <- 17
	RCALL BCD
	RCALL LOOP

	LDI R16,0xA1 ; r16 <- 161
	RCALL BCD
	RCALL LOOP

	rjmp numeros; recomeça a sequencia

bcd:
	clr r29 ; centena
	clr r30 ; dezena
	clr r31 ; unidade
	clr r17 ; reg. aux.

	mov r17,r16 ; copia o número para o registrador auxiliar

	clc
	cpi r17,0x64 ; Se r17 for menor que 100, flag C = 1
	brcs verificar_dez ; pula para subdez se C = 1

	subcent:
	inc r29 ; incrementa digito das centenas
	subi r17,0x64 ; r17 - 100
	cpi r17,0x64
	brcc subcent; enquanto r17 > 100, volta pra subcent

	verificar_dez:
	clc
	cpi r17,0x0A ; Se r17 for menor que 10, flag C = 1
	brcs subunidade ; pula se C = 1

	subdez:
	inc r30 ; incrementa o digito das dezenas
	subi r17,0x0A ; r17 - 10
	cpi r17,0x0A
	brcc subdez

	clc
	subunidade:
	mov r31, r17 ; coloca r17 no digito das unidades

	;--------------------------------------Inverte e faz swap nos bits de cada digito--------------------------------------

	;r29 = 0000 abcd
	clc	
	rol r29 ; 000a bcd0
	mov r17, r29 ; r17 = r29
	andi r29, 0xF0 ; r29 = 000a 0000

	andi r17, 0x0F ; 0000 bcd0
	clc
	rol r17 ; 000b cd00
	push r17 ; guarda r17
	andi r17, 0xF0 ; 000b 0000
	clc
	rol r17 ; 00b0 0000
	add r29, r17 ;  r29 = 00ba 0000

	pop r17 ;  recupera r17. r17 = 000b cd00
	andi r17, 0x0F ;  0000 cd00
	clc
	rol r17
	push r17 ; guarda
	andi r17, 0xF0 ;  000c 0000
	clc
	rol r17
	rol r17 ;  0c00 0000
	add r29, r17 ;  r29 = 0cba 0000

	pop r17 ;  r17 = 000c d000
	andi r17, 0x0F ;  0000 d000
	clc
	swap r17 ;r17 = d000 0000
	add r29, r17 ;  r29 = dcba 0000

	;------------------------------------------

	clc	
	rol r30 ; 000a bcd0
	mov r17, r30 ; r17 = r30
	andi r30, 0xF0 ; 000a 0000

	andi r17, 0x0F ; 0000 bcd0
	clc
	rol r17 ; 000b cd00
	push r17
	andi r17, 0xF0 ; 000b 0000
	clc
	rol r17 ; 00b0 0000
	add r30, r17 ; r30 = 00ba 0000

	pop r17 ; 000b cd00
	andi r17, 0x0F ; 0000 cd00
	clc
	rol r17
	push r17
	andi r17, 0xF0 ; 000c 0000
	clc
	rol r17
	rol r17 ; 0c00 0000
	add r30, r17 ; r30 = 0cba 0000

	pop r17 ; 000c d000
	andi r17, 0x0F ; 0000 d000
	clc
	swap r17 ; d000 0000
	add r30, r17 ; r30 = dcba 0000

	;----------------------------------------

	clc	
	rol r31 ;000a bcd0
	mov r17, r31
	andi r31, 0xF0 ; 000a 0000

	andi r17, 0x0F ;0000 bcd0
	clc
	rol r17 ; 000b cd00
	push r17
	andi r17, 0xF0 ;000b 0000
	clc
	rol r17 ; 00b0 0000
	add r31, r17 ; 00ba 0000

	pop r17 ; 000b cd00
	andi r17, 0x0F ; 0000 cd00
	clc
	rol r17
	push r17
	andi r17, 0xF0 ; 000c 0000
	clc
	rol r17
	rol r17 ; 0c00 0000
	add r31, r17 ; 0cba 0000

	pop r17 ; 000c d000
	andi r17, 0x0F ; 0000 d000
	clc
	swap r17 ; d000 0000
	add r31, r17 ; dcba 0000
	;-----------------------------------------------

	ret
