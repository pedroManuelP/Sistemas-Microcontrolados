.include "m328pdef.inc"
.org 0x0000
rjmp main
.org 0x0004
rjmp tratar_interrupt

;--------------------------------------------------INICIO_DO_MOSTRADOR--------------------------------------------------
mostrador:
	rcall bcd

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

rjmp mostrador;
;--------------------------------------------------FINAL_DO_MOSTRADOR--------------------------------------------------

;--------------------------------------------------INICIO_DA_MAIN--------------------------------------------------
main:
	ldi XH, high(100) ; distância mostrada
	ldi XL, low(100)
	
	ldi YH, high(000) ; distância salva
	ldi YL, low(000)

	ldi r17, 0xF6; 1111 0110 
	out DDRD, r17
	; pd7, pd6, pd5, pd4 são bits do decodificador
	; pd3, pd2 liga led
	; tj2, tj3

	ldi r17, 0x01; 0000 0001
	out DDRB, r17
	; pb0 é tj1

	ldi r17, 0x10; 0001 0000
	out DDRC, r17
	; pc3, pc2, pc1 são os botões s3, s2, s1
	; pc4 é o trigger
	; pc5 é o echo

	
	sei ; habilita interrupção global
	ldi r17, 0x02 ; 0000 0010
	sts PCICR, r17
	ldi r17, 0x0E ; 0000 1110
	sts PCMSK1, r17

	ldi r17, 0x0E; 0000 1111
	out DDRB, r17
	; pb3, pb2, pb1 são do RGB
		sbi PORTB, 2 ; led red
		cbi PORTB, 3
		cbi PORTB, 4

	rjmp mostrador
;--------------------------------------------------FINAL_DA_MAIN--------------------------------------------------

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
ldi XH, high(100)
ldi XL, low(100)
reti
ldi r16, 0xFF
;blink:
sbi PORTB, 2 ; led red
		cbi PORTB, 3
		cbi PORTB, 4
		dec r16
		cpi r16, 0x00
		brne trigger
reti

push r20 ; valor original
in r20, PINC ; pega vetor PINC

push r20 ; pinc
andi r20, 0x02 ; 0000 0010
cpi r20, 0x02
breq s1

pop r20 ; pinc
push r20
andi r20, 0x04 ; 0000 0100
cpi r20, 0x04
breq s2

pop r20 ; pinc
push r20
andi r20, 0x08 ; 0000 1000
cpi r20, 0x08
breq s3

s1:
	pop r20
	ldi r16, 0x02
	trigger:
		sbi PORTC, 4 ; trigger = 1 por 10 us
		dec r16
		cpi r16, 0x00
		brne trigger
		cbi PORTC, 4

	ldi ZH, 0x00 ; numero de ciclos de echo
	ldi ZL, 0x00 ; numero de ciclos de echo

	ldi r20, 0x07
	echo:
		adiw ZH:ZL, 0x06
		in r16, PINC ; r16 = PINC
		andi r16, 0x20 ; r16 = 00x0 0000
		cpi r16, 0x20
		breq echo  

	ldi r19, 0x00 ; multiplo de 2 cm
	sbiw ZH:ZL, 0x3A ; pra compensar depois

	distancia:
		adiw ZH:ZL, 0x3A
		sbiw ZH:ZL, 0x3A ; 58 ciclos são 2 cm
		inc r19
		sbiw ZH:ZL, 0x3A ; subtracao para verificar se Z > 58
		brpl distancia

	calculo:
		adiw XH:XL, 0x02
		dec r19
		cpi r19, 0x00
		brne calculo

	pop r20
	reti 

s2:
	pop r20
	;movw YH:YL, 

	pop r20
	reti 
s3:
	pop r20
	movw XH:XL, YH:YL ; bota no mostrador o valor salvo
	pop r20
	reti