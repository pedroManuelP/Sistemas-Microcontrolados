/*
 * Project 7.c
 *
 * Created: 09/06/2025 17:59:32
 * Author : chamo
 */ 

#include "defs_principais_AVR.h"
#include "ad_AVR.h"
#include "lcd_AVR.h"

void mde(uint8_t s);
void updateLCD();
void dac_set(uint8_t y);

char top_line[] =  "Mod: AM  T:  0Hz\0";
char bottom_line[] =  "Msg:           0\0";

uint8_t estado = 0;
uint8_t mudouParametro = 0;
uint8_t mudouTexto = 1;

uint8_t modulacao = 1; // valor que indica o tipo de modulação. 1 = AM, 2 = FM, 3 = ASK, 4 = FSK
int taxa = 50; // valor de frêquencia usado no cálculo da saída modulada
uint8_t entrada = 0;// última entrada analógica
int main(void)
{
	// Inicializando LCD
	lcd_init();
	
	// Inicializando AD
	ad_init();

	// Pinos do DAC = PB5:0 e PC5:4
	DDRB = 0x3F;
	DDRC = 0x30;
	
	// Inicializando os botões
	PCICR = 0x02;
	PCMSK1 = 0x0E;
	sei();
	while (1)
	{
		entrada = (ad_get(0) >> 2);
		mde(estado);
		
		//modulação da entrada vem aqui depois da mde
		_delay_ms(1);
	}
}

void mde(uint8_t s){
	switch (s)
	{
		//tela inicial
		case 0:
		if(mudouTexto) updateLCD();
		break;
		//tela inicial
		
		//tela ajuste modulacao
		case 1:
		if(mudouParametro == 1){
			if(modulacao < 5) modulacao++;
			if(modulacao == 5) modulacao = 1;
			}else if(mudouParametro == 2){
			if(modulacao > 0) modulacao--;
			if(modulacao == 0) modulacao = 4;
		}
		
		if(mudouTexto) updateLCD();
		break;
		//tela ajuste modulacao
		
		//tela ajuste taxa
		case 2:
		if(mudouParametro == 1){
			if(taxa < 255) taxa++;
			}else if(mudouParametro == 2){
			if(taxa > 0) taxa--;
		}
		
		if(mudouTexto) updateLCD();
		break;
		//tela ajuste taxa
		
		default:
		break;
	}
}

void updateLCD(){
	//muda o tipo de modulacao no texto do LCD
	top_line[5] = ' ';
	top_line[6] = ' ';
	top_line[7] = ' ';
	switch(modulacao){
		case 1:
		top_line[5] = 'A';
		top_line[6] = 'M';
		break;
		case 2:
		top_line[5] = 'F';
		top_line[6] = 'M';
		break;
		case 3:
		top_line[5] = 'A';
		top_line[6] = 'S';
		top_line[7] = 'K';
		break;
		case 4:
		top_line[5] = 'F';
		top_line[6] = 'S';
		top_line[7] = 'K';
		break;
		default:
		break;
	}
	
	//limpa o texto da última entrada e taxa
	for (int i = 0;i<12;i++){
		bottom_line[15-i] = ' ';
	}
	for (int i = 0;i<5;i++)
	{
		top_line[15-i] = ' ';
	}
	
	//escreve o valor entrada e da taxa ou em forma analógia ou digital
	if(modulacao == 1 || modulacao == 2){
		top_line[9] = 'F';
		numIntoString(top_line,13,taxa);
		top_line[14] = 'H';
		top_line[15] = 'z';
		numIntoString(bottom_line,15,entrada);
		}else if(modulacao == 3 || modulacao == 4){
		top_line[9] = 'T';
		numIntoString(top_line,13,(taxa*8));
		top_line[14] = 'b';
		top_line[15] = 's';
		
		for (int i = 0;i<4;i++)
		{
			bottom_line[15- 3*i] = (tst_bit(entrada,2*i) >> (2*i)) + '0';
			bottom_line[14- 3*i] = (tst_bit(entrada,(2*i+1)) >> (2*i+1)) + '0';
			bottom_line[13- 3*i]='.';
		}
		bottom_line[4] = ' ';
	}
	
	lcd_update(top_line,bottom_line);
	mudouTexto = 0;
	mudouParametro = 0;
}

void dac_set(uint8_t y){
	//Y[7:0] => [PB5:0, PC5:4]
	if(tst_bit(y,7)){
		set_bit(PORTB,5);
		}else {
		clr_bit(PORTB,5);
	}
	if(tst_bit(y,6)){
		set_bit(PORTB,4);
		}else{
		clr_bit(PORTB,4);
	}
	if(tst_bit(y,5)){
		set_bit(PORTB,3);
		}else{
		clr_bit(PORTB,3);
	}
	if(tst_bit(y,4)){
		set_bit(PORTB,2);
		}else{
		clr_bit(PORTB,2);
	}
	if(tst_bit(y,3)){
		set_bit(PORTB,1);
		}else{
		clr_bit(PORTB,1);
	}
	if(tst_bit(y,2)){
		set_bit(PORTB,0);
		}else{
		clr_bit(PORTB,0);
	}
	if(tst_bit(y,1)){
		set_bit(PORTC,5);//SCL
		}else{
		clr_bit(PORTC,5);
	}
	if(tst_bit(y,0)){
		set_bit(PORTC,4);//SDA
		}else{
		clr_bit(PORTC,4);
	}
	_delay_ms(2);//delay para segurar o sinal
}

ISR(PCINT1_vect){
	_delay_ms(50); // debounce
	
	if((PINC & 0x0E) == 0x0C){//mudar estado
		if(estado == 0){
			estado = 1;
			}else if(estado == 1){
			estado = 2;
			}else if(estado == 2){
			estado = 0;
		}
		mudouTexto = 1;
	}
	
	if((PINC & 0x0E) == 0x0A){//incrementar parâmetro
		mudouParametro = 1;
		mudouTexto = 1;
	}

	if((PINC & 0x0E) == 0x06){//decrementar parâmetro
		mudouParametro = 2;
		mudouTexto = 1;
	}
}