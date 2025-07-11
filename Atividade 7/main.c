/*
 * atv7.c
 *
 * Created: 09/06/2025 17:32:38
 * Author : valld
 */ 

#include "defs_principais_AVR.h"
#include "ad_AVR.h"
#include "lcd_AVR.h"

#define POINTS 33

void mde(uint8_t s);
void updateLCD();
void dac_set(uint8_t y);
float cos_table[POINTS] = {
	1.0000,0.9808,0.9239,0.8315,0.7071,0.5556,0.3827,0.1951,0.0000,-0.1951,-0.3827,-0.5556,-0.7071,-0.8315,-0.9239,-0.9808,
	-1.0000,-0.9808,-0.9239,-0.8315,-0.7071,-0.5556,-0.3827,-0.1951,-0.0000,0.1951,0.3827,0.5556,0.7071,0.8315,0.9239,0.9808,
	1.0000
};

char top_line[] =  "Mod: AM  T:  0Hz\0";
char bottom_line[] =  "Msg:           0\0";

uint8_t estado = 0;
uint8_t mudouParametro = 0;
uint8_t mudouTexto = 1;

uint8_t modulacao = 1; // valor que indica o tipo de modulação. 1 = AM, 2 = FM, 3 = ASK, 4 = FSK
int taxa = 50; // valor de frêquencia usado no cálculo da saída modulada
uint8_t entrada = 0;// última entrada analógica

float saida_modulada_float = 0.0;
uint8_t saida_modulada_dac = 0;

volatile uint16_t cos_idx = 0;
volatile float cos_step_float = 0.0;

float fator_modulacao_am = 0.1; //indice de modulacao (80%)
float mensagem_norm_am;

float freq_desvio_fm;
float max_desvio_f_norm = 0.8;// Desvio máximo de frequência relativo (ex: 50% da freq. base)

float current_step_fsk;

volatile uint8_t current_bit_idx = 0; 
volatile uint16_t samples_per_bit_counter = 0;
volatile uint16_t samples_per_bit = 0;
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
	
	//inicializa o cos_step_float com um valor padrão 
	cos_step_float = (float)(taxa*POINTS)/1000.0;
	if (cos_step_float < 0.01) cos_step_float=0.01;
	
	samples_per_bit = 1000 / taxa; // Inicializa para digital também
	if (samples_per_bit == 0) samples_per_bit = 1;
	while (1)
	{
		entrada = (ad_get(0) >> 2);
		mde(estado);
		
		//modulação da entrada vem aqui depois da mde
		
		cos_idx = (uint16_t)((float)cos_idx + cos_step_float)%POINTS; // avança no indice na tabela dos cossenos
		
		float carrier_value = cos_table[cos_idx]; //o valor base da portadora
		
		
		switch (modulacao){
			case 1://AM - Modulação por Amplitude
				mensagem_norm_am = ((float)entrada/255.0) - 0.5; //Mapeia o 255 para -0.5 e 0.5
				
				saida_modulada_float = carrier_value *(1.0 +fator_modulacao_am+mensagem_norm_am);//Saída = Portadora * (1 + Fator_Modulacao * Mensagem_Normalizada)
				
				if(saida_modulada_float > 1.0) saida_modulada_float = 1.0;
				if(saida_modulada_float < -1.0) saida_modulada_float = -1.0;
				break;
			
			case 2: // FM - Modulação por Frequencia
			
				freq_desvio_fm = ((float)entrada / 255.0) - 0.5;
				
				float current_cos_step_fm = cos_step_float * (1.0 + max_desvio_f_norm * freq_desvio_fm);
				if(current_cos_step_fm < 0.01) current_cos_step_fm = 0.01;
				
				//Atualiza o índice com o passo modulado
				cos_idx = (uint16_t)((float)cos_idx + current_cos_step_fm) % POINTS;
				saida_modulada_float = cos_table[cos_idx];
				break;
			
			case 3: // ASK - Amplitude Shift Keying
				entrada = 67;
				for(int i = 0;i < 8;i++){
					if(tst_bit(entrada,i)){
						saida_modulada_float = carrier_value; // envia a portadora
					}else{
						saida_modulada_float = 0; // envia o níel mais baixo após o mapeamento 
					}
				}
			
				break;

				
				
			case 4: // FSK - Frequency Shift Keing
				
				if(entrada > 128) current_step_fsk = cos_step_float *1.2; // frequencia alta 1.2 vezes maior que a frequencia base
				else current_step_fsk = cos_step_float * 0.8; // frequencia baixa 0.8 vezes menor que a frequencia base
				
				if(current_step_fsk < 0.01) current_cos_step_fm = 0.01;
				
				cos_idx = (uint16_t)((float)cos_idx + current_step_fsk)%POINTS;
				saida_modulada_float = cos_table[cos_idx];
				
				break;
			
			default:
				saida_modulada_float = 0.8;
				break;
		}
		
		saida_modulada_dac = (uint8_t)(((saida_modulada_float + 1.0) / 2.0) * 255.0); // Mapeia o valor float, entre -1.0 a 1.0, para o range de 8 bits (0 a 255) do dac
		
		dac_set(saida_modulada_dac);
	
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
		
		cos_step_float = (float)(taxa*POINTS)/1000.0; //considerando 1khz de amostragem
		if(cos_step_float < 0.01) cos_step_float = 0.01;
		
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
