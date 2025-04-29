.include "m328pdef.inc"
.org 0x0000
rjmp main
.org 0x0008
rjmp tratar_interrupt

;--------------------------------------------------INICIO_DO_CONTADOR--------------------------------------------------
contador:
	push r20 ; guarda passo no stack

	cpi r19, 0 ; verifica se inc ou dec
	breq incrementar
	decrementar:
		sbiw YH:YL, 1
		dec r20
		cpi r20, 0
		brne decrementar ; faz a operação Y - 1 r25 vezes. Fazendo assim Y - r25

	rjmp fim_operacao

	incrementar:
		adiw YH:YL, 1
		dec r20
		cpi r20, 0
		brne incrementar ; faz a operação Y + 1 r25 vezes. Fazendo assim Y + r25

	fim_operacao:
	pop r20 ; recupera o valor de passo

	cpi r19, 0x01
	breq verificar_minimo

	verificar_maximo:
		cp ZH, YH ; se ZH < YH, vai para maximo
		brlt maximo
		cp ZH, YH ; se ZH > YH vai para o final do contador
		brne final_contador

		cp ZL, YL ; se ZL < YL, vai para maximo
		brlo maximo
		rjmp final_contador

	verificar_minimo:
		brmi minimo
		cp YH, XH ; se YH < XH, vai para minimo
		brlt minimo
		cp YH, XH ; se YH > XH, vai para o final do contador
		brne final_contador

		cp YL, XL ; se YL < XL, vai para minimo
		brlo minimo
		rjmp final_contador

	maximo:
		movw YH:YL, XH:XL ; copia X para Y somente quando Z(max) < Y(atual)
		rjmp final_contador

	minimo:
		movw YH:YL, ZH:ZL ; copia Z para Y somente quando X(min) > Y(atual)

	final_contador:
		ret
;--------------------------------------------------FINAL_DO_CONTADOR--------------------------------------------------

;--------------------------------------------------INICIO_DO_MOSTRADOR--------------------------------------------------
mostrador:

	delay:
		;LDI R17, 0x01 ; ciclos externos
		;LDI R18, 0x01 ; ciclos externos
		LDI R25, 0x5F ; ciclos de delay entre digitos
		; qtde. de ciclos de delay_loop1 ~= 152 * r17 * r18
		; qtde. de ciclos de delay_loop2 = 152 * r17
		; r17 = 81 e r18 = 81
		; r17 = 0x51 e r18 = 0x51
		; qtde total de ciclos = 81 * 81 * 152 = 997272 ciclos = 0.997272 segundos

	delay_loop1:
		push r17
		delay_loop2:
			cpi r21, 0x00
			breq leddez ; vai para leddez se o digito for zero

			OUT PORTD, r21 ; centena para saída
			sbi PORTB, 0 ; liga led das centenas
			cbi PORTD, 3 ; desliga led das dezenas
			cbi PORTD, 2 ; desliga led das unidades

			;------------
			push r25
			delay1:
			DEC r25 
			CPI r25, 0x00 
			BRNE delay1
			pop r25
			;------------

			leddez:
			cpi r21, 0x00
			breq prox
			rjmp mostra
			prox: 
			cpi r22, 0x00
			breq ledunid ; vai para ledunid se o digito da dezena for zero

			mostra:
			OUT PORTD, r22 ; dezena para saída
			sbi PORTD, 3
			cbi PORTB, 0
			cbi PORTD, 2

			;------------
			push r25
			delay2:
			DEC r25 
			CP r25, r1 
			BRNE delay2
			pop r25
			;------------

			ledunid:
			OUT PORTD, r23 ; unidade para saída
			sbi PORTD, 2
			cbi PORTB, 0
			cbi PORTD, 3

			;------------
			push r25
			delay3:
			DEC r25 
			CP r25, r1 
			BRNE delay3
			pop r25
			;------------

			DEC r17 
			CPI r17, 0x00 
			BRNE DELAY_LOOP2; volta para delay_loop2 se r17 =/= 0
			;fim do delay_loop2

		pop r17
		dec r18
		cpi r18, 0x00
		brne delay_loop1
		;fim do delay_loop1
ret
;--------------------------------------------------FINAL_DO_MOSTRADOR--------------------------------------------------

;--------------------------------------------------INICIO_DA_MAIN--------------------------------------------------
main:
	ldi r19, 0x00 ; crescente ou decrescente

	ldi r20, 0x0F ; passo de fabrica

	ldi ZH, high(999) ; máximo de fabrica
	ldi ZL, low(999)

	ldi YH, high(0) ; atual de fabrica
	ldi YL, low(0)

	ldi XH, high(0) ; minimo de fabrica
	ldi XL, low(0)

	ldi r17, 0xF6; 1111 0110 
	out DDRD, r17
	; pd7, pd6, pd5, pd4 são bits do decodificador

	ldi r17, 0x0F; 0000 1111
	out DDRB, r17
	; pb3, pb2, pb1 são do RGB

	ldi r17, 0x00; 0000 0000
	out DDRC, r17
	; pc3, pc2, pc1 são os botões
	; pc0 é o potenciometro

	sei ; habilita interrupção global
	ldi r17, 0x02 ; 0000 0010
	sts PCICR, r17
	ldi r17, 0x0E ; 0000 1110
	sts PCMSK1, r17

	clr r16; estado
;--------------------------------------------------FINAL_DA_MAIN--------------------------------------------------

;--------------------------------------------------INICIO_DO_CONTROLADOR--------------------------------------------------
controlador:
	cpi r16, 0x00
	breq estado_contador

	cpi r16, 0x01
	breq estado_min
	
	cpi r16, 0x02
	breq estado_max

	cpi r16, 0x03
	breq estado_passo

	estado_contador:
		cbi PORTB, 2
		cbi PORTB, 3
		cbi PORTB, 4

		LDI R17, 0x51 ; ciclos externos
		LDI R18, 0x51 ; ciclos externos
		movw r25:r24, YH:YL ; copia o valor atual para o par de registradores do bcd
		rcall bcd ; converte r25:r24 para BCD
		rcall mostrador 
		
		rcall contador
		rjmp controlador
		

	estado_min: 
		sbi PORTB, 2 ; led red
		cbi PORTB, 3
		cbi PORTB, 4

		movw r25:r24, XH:XL ; copia o minimo para o par de registradores do bcd
		rcall bcd ; converte r25:r24 para BCD
		LDI R17, 0x0F ; ciclos externos
		LDI R18, 0x0F ; ciclos externos
		rcall mostrador 
		rjmp controlador
	
	estado_max: 
		sbi PORTB, 3 ; led green
		cbi PORTB, 2
		cbi PORTB, 4

		movw r25:r24, ZH:ZL ; copia o maximo para o par de registradores do bcd
		rcall bcd ; converte r25:r24 para BCD
		LDI R17, 0x0F ; ciclos externos
		LDI R18, 0x0F ; ciclos externos
		rcall mostrador 
		rjmp controlador

	estado_passo: 
		sbi PORTB, 4 ; led blue
		cbi PORTB, 2
		cbi PORTB, 3

		LDI R17, 0x0F ; ciclos externos
		LDI R18, 0x0F ; ciclos externos
		clr r25
		mov r24, r20 ; copia o passo para o par de registradores do bcd
		rcall bcd ; converte r25:r24 para BCD
		rcall mostrador 
		rjmp controlador
;--------------------------------------------------FINAL_DO_CONTROLADOR--------------------------------------------------

;--------------------------------------------------INICIO_DO_BCD--------------------------------------------------
bcd:
	clr r21 ; centena
	clr r22 ; dezena
	clr r23 ; unidade

	;push r25 ; guarda o valor que será mostrado no STACK
	;push r24

	;pop r24 ; recupera o valor que será mostrado no STACK
	;pop r25

	sbiw r25:r24, 0x3F ; subtrai 63 do valor binario
	sbiw r25:r24, 0x25 ; subtrai 37 do valor binario
	brmi menor_cem ; se X < 100 pula para menor_cem

	subcent:
	inc r21
	sbiw r25:r24, 0x3F
	sbiw r25:r24, 0x25 ; X - 100
	brpl subcent ; se X > 100 vai para subcent

	menor_cem:
	adiw r25:r24, 0x3F ; soma 100 ao valor binario para recuperar ele
	adiw r25:r24, 0x25

	sbiw r25:r24, 0x0A
	brmi menor_dez ; se X < 10 pula para menor_dez

	subdez:
	inc r22
	sbiw r25:r24,0x0A ; X - 10
	brpl subdez ; se X > 10 vai para subdez

	menor_dez:
	adiw r25:r24, 0x0A

	subunidade:
	mov r23, r24

	clc	
	rol r21 ;r21 = 000a bcd0
	mov r24, r21 ; r24 = r21
	andi r21, 0xF0 ; r21 = 000a 0000

	andi r24, 0x0F ; r24 = 0000 bcd0
	clc
	rol r24 ; r24 = 000b cd00
	push r24
	andi r24, 0xF0 ; r24 = 000b 0000
	clc
	rol r24 ; r24 = 00b0 0000
	add r21, r24 ; r21 = 00ba 0000

	pop r24 ; r24 = 000b cd00
	andi r24, 0x0F ; r24 = 0000 cd00
	clc
	rol r24
	push r24
	andi r24, 0xF0 ; r24 = 000c 0000
	clc
	rol r24
	rol r24 ; r24 = 0c00 0000
	add r21, r24 ; r24 = 0cba 0000

	pop r24 ; r24 = 000c d000
	andi r24, 0x0F ; r24 = 0000 d000
	clc
	rol r24
	rol r24
	rol r24
	rol r24 
	add r21, r24 ; r24 = dcba 0000

	; ---------------------------------------------------------------------------------------

	clc	
	rol r22 ;r21 = 000a bcd0
	mov r24, r22 ; r24 = r21
	andi r22, 0xF0 ; r21 = 000a 0000

	andi r24, 0x0F ; r24 = 0000 bcd0
	clc
	rol r24 ; r24 = 000b cd00
	push r24
	andi r24, 0xF0 ; r24 = 000b 0000
	clc
	rol r24 ; r24 = 00b0 0000
	add r22, r24 ; r21 = 00ba 0000

	pop r24 ; r24 = 000b cd00
	andi r24, 0x0F ; r24 = 0000 cd00
	clc
	rol r24
	push r24
	andi r24, 0xF0 ; r24 = 000c 0000
	clc
	rol r24
	rol r24 ; r24 = 0c00 0000
	add r22, r24 ; r24 = 0cba 0000

	pop r24 ; r24 = 000c d000
	andi r24, 0x0F ; r24 = 0000 d000
	clc
	rol r24
	rol r24
	rol r24
	rol r24 
	add r22, r24 ; r24 = dcba 0000

	;----------------------------------------------------------------------------------

	clc	
	rol r23 ;r21 = 000a bcd0
	mov r24, r23 ; r24 = r21
	andi r23, 0xF0 ; r21 = 000a 0000

	andi r24, 0x0F ; r24 = 0000 bcd0
	clc
	rol r24 ; r24 = 000b cd00
	push r24
	andi r24, 0xF0 ; r24 = 000b 0000
	clc
	rol r24 ; r24 = 00b0 0000
	add r23, r24 ; r21 = 00ba 0000

	pop r24 ; r24 = 000b cd00
	andi r24, 0x0F ; r24 = 0000 cd00
	clc
	rol r24
	push r24
	andi r24, 0xF0 ; r24 = 000c 0000
	clc
	rol r24
	rol r24 ; r24 = 0c00 0000
	add r23, r24 ; r24 = 0cba 0000

	pop r24 ; r24 = 000c d000
	andi r24, 0x0F ; r24 = 0000 d000
	clc
	rol r24
	rol r24
	rol r24
	rol r24 
	add r23, r24 ; r24 = dcba 0000
	ret
;--------------------------------------------------FINAL_DO_BCD--------------------------------------------------

tratar_interrupt:
lds r1,SREG
push r20
in r20, PINC
cbr r20, $F0 ; r20 = 0000 xxx0
cbr r20, 0

cpi r20, 0x0C ; 0000 1100
breq set_cresc

cpi r20, 0x06 ; 0000 0110
breq set_decre

cpi r20, 0x0A ; 0000 1010
breq mudar_estado

set_cresc:
	ldi r19, 0x00 ; contagem crescente
	pop r20
	sts SREG, r1
	RETI

set_decre:
	ldi r19, 0x01 ; contagem decrescente
	pop r20
	sts SREG, r1
	RETI

mudar_estado:
	cpi r16, 0x00
	breq estado_1

	cpi r16, 0x01
	breq estado_2

	cpi r16, 0x02
	breq estado_3

	cpi r16, 0x03
	breq estado_0

	estado_0:
		ldi r16, 0x00
		pop r20
		sts SREG, r1
		RETI

	estado_1:
		ldi r16, 0x01
		pop r20
		sts SREG, r1
		RETI

	estado_2:
		ldi r16, 0x02
		pop r20
		sts SREG, r1
		RETI

	estado_3:
		ldi r16, 0x03
		pop r20
		sts SREG, r1
		RETI
