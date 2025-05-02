.include "m328pdef.inc"
.org 0x0000
rjmp main
.org 0x0008 ; endereço da interrupção PCINT1
rjmp tratar_pcint1

;--------------------------------------------------INICIO_DO_CONTADOR--------------------------------------------------
contador:
	push r20 ; guarda passo no stack

	cpi r19, 0 ; verifica se é crescente ou decrescente
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
	breq verificar_minimo ; se decrescente, verifica se o valor é menor que o minímos

	verificar_maximo:
		cp ZH, YH ; se ZH < YH, vai para set_min
		brlt set_min
		cp ZH, YH ; se ZH > YH vai para o final do contador
		brne final_contador

		cp ZL, YL ; se ZL < YL, vai para set_min
		brlo set_min
		rjmp final_contador

	verificar_minimo:
		brmi set_max
		cp YH, XH ; se YH < XH, vai para set_max
		brlt set_max
		cp YH, XH ; se YH > XH, vai para o final do contador
		brne final_contador

		cp YL, XL ; se YL < XL, vai para set_max
		brlo set_max
		rjmp final_contador

	set_min:
		movw YH:YL, XH:XL ; copia X para Y somente quando Z(max) < Y(atual)
		rjmp final_contador

	set_max:
		movw YH:YL, ZH:ZL ; copia Z para Y somente quando X(min) > Y(atual)

	final_contador:
		ret
;--------------------------------------------------FINAL_DO_CONTADOR--------------------------------------------------

;--------------------------------------------------INICIO_DO_MOSTRADOR--------------------------------------------------
mostrador:
	delay:
		LDI R25, 0x5F ; qtde. de ciclos de delay entre digitos

		; qtde. de ciclos de delay_loop1 ~= 152 * r17 * r18
		; qtde. de ciclos de delay_loop2 = 152 * r17

	delay_loop1:
		push r17 ; guarda ciclos internos
		delay_loop2:
			cpi r21, 0x00
			breq digitoCemZero ; vai para digitoCemZero sem ligar a centena se o digito da centena for zero

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

			digitoCemZero:
			cpi r22, 0x00 
			breq ledunid ; vai para ledunid sem ligar a dezena se o digito da dezena for zero

			leddez:
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
		brne delay_loop1; volta para delay_loop1 se r18 =/= 0
		;fim do delay_loop1
ret
;--------------------------------------------------FINAL_DO_MOSTRADOR--------------------------------------------------

;--------------------------------------------------INICIO_DA_MAIN--------------------------------------------------
main:
	ldi r19, 0x00 ; crescente ou decrescente

	ldi r20, 0x0F ; passo de fabrica

	ldi ZH, high(999) ; máximo de fabrica
	ldi ZL, low(999)

	ldi YH, high(0) ; valor atual de fabrica
	ldi YL, low(0)

	ldi XH, high(0) ; minimo de fabrica
	ldi XL, low(0)

	ldi r17, 0xF6; 1111 0110 
	out DDRD, r17
	; pd7, pd6, pd5, pd4 são os bits do decodificador
	; pd2 e pd1 liga dois leds 7seg

	ldi r17, 0x0F; 0000 1111
	out DDRB, r17
	; pb3, pb2, pb1 são do RGB
	; pb0 liga um led 7seg

	ldi r17, 0x00; 0000 0000
	out DDRC, r17
	; pc3, pc2, pc1 são os botões
	; pc0 é o tensão do potenciometro

	sei ; habilita interrupção global
	ldi r17, 0x02 ; 0000 0010
	sts PCICR, r17 ; seta o grupo PCINT1 para interromper com pin change
	ldi r17, 0x0E ; 0000 1110
	sts PCMSK1, r17 ; seta pcint9, pcint10 e pcint11 para causarem interrupções 
	
	;-------------------------- conversor A/D de Sama --------------------------
	cbi     DDRC, 0      ; defina PC pin 0 como entrada(tensao do potenciometro)
	ldi     r16, 0x60
	sts     ADMUX, r16   ; defina AVcc como referencia e ajuste a esquerda             
	ldi     r16, 0x00
	sts     ADCSRB, r16  ; defina o modo de trigger free running		
	ldi     r16, 0x87    
	sts     ADCSRA, r16  ; habilite o adc e utilize uma pre-escala de 128

	ldi     r16,0x83
	sts     TCCR2A,r16   ; defina o modo de opercao fast pwm
	ldi     r16,0x01
	sts     TCCR2B,r16   ; defina o clock do timer 2 sem pre-escala 
	sts     TCNT2,r16    ; inicialize o timer 2 em 0x01
	ldi     r16,0x3F     
	sts     OCR2A,r16    ; inicialize o duty cycle do pwm em 0x3F

	clr r16; estado inicial
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
		cbi PORTB, 1 ; desliga RGB
		cbi PORTB, 2
		cbi PORTB, 3

		LDI R17, 0x61 ; ciclos internos do mostrador
		LDI R18, 0x61 ; ciclos externos do mostrador
		movw r25:r24, YH:YL ; copia o valor atual do contador para o par de registradores do bcd
		rcall bcd ; converte r25:r24 para BCD
		rcall mostrador 
		
		rcall contador ; faz Y = Y +/- r20
		rjmp controlador

	estado_min: 
		rcall conversor_de_sama; faz a conversão A/D da tensão do potenciometro e guarda o valor em r22:r21
		cpi r21, 0xE7 ; se r22:r21 >= 999, seta o valor para 999 
		brge maior999_min
		rjmp cont_min

		maior999_min:
		ldi r22, 0x03
		ldi r21,0xE7

		cont_min:
		sbi PORTB, 3 ; liga led red
		cbi PORTB, 2
		cbi PORTB, 1

		mov XH,r22 ; passa o valor do potenciometro para X(minimo)
		mov XL,r21

		movw r25:r24, XH:XL ; copia o minimo para o par de registradores do bcd
		rcall bcd ; converte r25:r24 para BCD

		LDI R17, 0x60 ; ciclos internos
		LDI R18, 0x60 ; ciclos externos
		rcall mostrador 
		rjmp controlador
	
	estado_max: 
		rcall conversor_de_sama
		cpi r21, 0xE7 ; se r22:r21 >= 999, seta o valor para 999 
		brge maior999_max
		rjmp cont_max

		maior999_max:
		ldi r22, 0x03
		ldi r21,0xE7

		cont_max:
		sbi PORTB, 2 ; led green
		cbi PORTB, 3
		cbi PORTB, 1

		mov ZH,r22 ; guarda o valor do potenciometro para o Z(maximo)
		mov ZL,r21

		movw r25:r24, ZH:ZL ; copia o maximo para o par de registradores do bcd
		rcall bcd ; converte r25:r24 para BCD

		LDI R17, 0x60 ; ciclos internos
		LDI R18, 0x60 ; ciclos externos
		rcall mostrador 
		rjmp controlador

	estado_passo: 
		rcall conversor_de_sama
		;----------
		lsr r22 ; 2^-1
		ror r21
		clc
		;----------
		;----------
		lsr r22 ; 2^-2
		ror r21
		clc
		;----------
		;----------
		lsr r22 ; 2^-3
		ror r21
		clc
		;----------
		;----------
		lsr r22 ; 2^-4
		ror r21
		clc
		;----------
		;----------
		lsr r22 ; 2^-5
		ror r21
		clc
		;----------
		;----------
		lsr r22 ; 2^-6
		ror r21
		clc
		;----------

		sbi PORTB, 1; led blue
		cbi PORTB, 3
		cbi PORTB, 2

		clr r22
		mov r20,r21 ; passa r21 para o passo

		clr r25
		mov r24, r20 ; copia o passo para o par de registradores do bcd
		rcall bcd ; converte r25:r24 para BCD

		LDI R17, 0x60 ; ciclos internos
		LDI R18, 0x60 ; ciclos externos
		rcall mostrador 
		rjmp controlador
;--------------------------------------------------FINAL_DO_CONTROLADOR--------------------------------------------------


;--------------------------------------------------INICIO_DO_CONVERSOR--------------------------------------------------
conversor_de_sama:
loop:
	rcall   Radc         ; salte para Radc (funcao para ler adc)
	sts     OCR2A,r22    ; atualize o valor do duty cycle do pwm
	ret     

Radc:   
	push r20 ; salva o passo no stack
	ldi     r20, 0xC7    
	sts     ADCSRA,r20   ; inicie a conversao 

loop_adc:
	lds     r20, ADCSRA  ; carregue no registrador 20 o valor de ADCSRA
	sbrs    r20, ADIF    ; verifique se o processo de conversao finalizou
	rjmp    loop_adc 

	lds     r21, ADCL    ; carregue o valor (L) do conversor ad no registrador 21
	lds     r22, ADCH    ; carregue o valor (H) do conversor ad no registrador 22

	;--------------------------------------DIVIDE_POR_2--------------------------------------
	lsr r22 ; r22 = 0xxx xxxx c = r22(0)
	ror r21 ; r21 = b0 xxx xxxx c = r21(0)
	clc
	;--------------------------------------DIVIDE_POR_2--------------------------------------
	;----------
	lsr r22 ; 2^-2
	ror r21
	clc
	;----------
	lsr r22 ; 2^-3
	ror r21  
	clc
	;----------
	lsr r22 ; 2^-4
	ror r21
	clc
	;----------
	lsr r22  ; 2^-5
	ror r21  
	clc
	;----------
	lsr r22 ; 2^-6
	ror r21
	clc
	;----------

	pop r20 ; recupera o passo do stack
	ret ; retorne 
;--------------------------------------------------FINAL_DO_CONVERSOR--------------------------------------------------


;--------------------------------------------------INICIO_DO_BCD--------------------------------------------------
bcd:
	clr r21 ; centena
	clr r22 ; dezena
	clr r23 ; unidade

	sbiw r25:r24, 0x3F ; X -= 63
	sbiw r25:r24, 0x25 ; X -= 37
	brmi menor_cem ; se X < 100 pula para menor_cem

	subcent:
	inc r21
	sbiw r25:r24, 0x3F ; X -= 100
	sbiw r25:r24, 0x25 
	brpl subcent ; se X > 100 vai para subcent

	menor_cem:
	adiw r25:r24, 0x3F ; X += 100 para recuperar ele
	adiw r25:r24, 0x25

	sbiw r25:r24, 0x0A ; X -= 10
	brmi menor_dez ; se X < 10 pula para menor_dez

	subdez:
	inc r22
	sbiw r25:r24,0x0A ; X - 10
	brpl subdez ; se X > 10 vai para subdez

	menor_dez:
	adiw r25:r24, 0x0A ; X += 10 para recuperar ele

	subunidade:
	mov r23, r24

	;----------------------------------------CODIFICAÇÃO_PARA_O_DECODIFICADOR_LED7SEG----------------------------------------
	clc	
	rol r21 ; 000a bcd0
	mov r24, r21 ; r24 = r21
	andi r21, 0xF0 ; 000a 0000

	andi r24, 0x0F ; 0000 bcd0
	clc
	rol r24 ; 000b cd00
	push r24
	andi r24, 0xF0 ; 000b 0000
	clc
	rol r24 ; 00b0 0000
	add r21, r24 ; 00ba 0000

	pop r24 ; 000b cd00
	andi r24, 0x0F ; 0000 cd00
	clc
	rol r24
	push r24
	andi r24, 0xF0 ; 000c 0000
	clc
	rol r24
	rol r24 ; 0c00 0000
	add r21, r24 ; 0cba 0000

	pop r24 ; 000c d000
	andi r24, 0x0F ; 0000 d000
	clc
	swap r24 ; d000 0000
	add r21, r24 ; r24 = dcba 0000

	; ---------------------------------------------------------------------------------------

	clc	
	rol r22 ; 000a bcd0
	mov r24, r22 ; r24 = r22
	andi r22, 0xF0 ; 000a 0000

	andi r24, 0x0F ; 0000 bcd0
	clc
	rol r24 ; 000b cd00
	push r24
	andi r24, 0xF0 ; 000b 0000
	clc
	rol r24 ; 00b0 0000
	add r22, r24 ; 00ba 0000

	pop r24 ; 000b cd00
	andi r24, 0x0F ; 0000 cd00
	clc
	rol r24
	push r24
	andi r24, 0xF0 ; 000c 0000
	clc
	rol r24
	rol r24 ; 0c00 0000
	add r22, r24 ; 0cba 0000

	pop r24 ; 000c d000
	andi r24, 0x0F ; 0000 d000
	clc
	swap r24 ; d000 0000
	add r22, r24 ; r24 = dcba 0000

	;----------------------------------------------------------------------------------

	clc	
	rol r23 ; 000a bcd0
	mov r24, r23 ; r24 = r23
	andi r23, 0xF0 ; 000a 0000

	andi r24, 0x0F ; 0000 bcd0
	clc
	rol r24 ; 000b cd00
	push r24
	andi r24, 0xF0 ; 000b 0000
	clc
	rol r24 ; 00b0 0000
	add r23, r24 ; 00ba 0000

	pop r24 ; 000b cd00
	andi r24, 0x0F ; 0000 cd00
	clc
	rol r24
	push r24
	andi r24, 0xF0 ; 000c 0000
	clc
	rol r24
	rol r24 ; 0c00 0000
	add r23, r24 ; 0cba 0000

	pop r24 ; 000c d000
	andi r24, 0x0F ; 0000 d000
	clc
	swap r24 ; d000 0000
	add r23, r24 ; r24 = dcba 0000

	ret
;--------------------------------------------------FINAL_DO_BCD--------------------------------------------------

;--------------------------------------------------INICIO_DA_INTERRUPÇÃO_PCINT1--------------------------------------------------
tratar_pcint1:
push r20 ; guarda no stack

in r20, PINC ; pega o vetor PINC
andi r20, 0x0E ; r20 = 0000 xxx0

cpi r20, 0x0C ; se r20 = 0000 1100
breq set_cresc

cpi r20, 0x06 ; se r20 = 0000 0110
breq set_decre

cpi r20, 0x0A ; se r20 = 0000 1010
breq mudar_estado

cpi r20, 0x0E ; caso nenhum botão esteja apertado
breq return_pcint1

set_cresc:
	ldi r19, 0x00 ; contagem crescente
	rjmp return_pcint1

set_decre:
	ldi r19, 0x01 ; contagem decrescente
	rjmp return_pcint1

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
		ldi r16, 0x00 ; muda estado
		rjmp return_pcint1

	estado_1:
		ldi r16, 0x01 ; muda estado
		rjmp return_pcint1

	estado_2:
		ldi r16, 0x02 ; muda estado
		rjmp return_pcint1

	estado_3:
		ldi r16, 0x03 ; muda estado
		rjmp return_pcint1

return_pcint1:
pop r20 ; recupera do stack
reti
;--------------------------------------------------FINAL_DA_INTERRUPÇÃO_PCINT1--------------------------------------------------
