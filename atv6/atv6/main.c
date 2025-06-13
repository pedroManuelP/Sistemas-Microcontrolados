#include "defs_principais_AVR.h"
#include "lcd_AVR.h"
#include "ad_AVR.h"
#include <avr/eeprom.h>
#include <stdlib.h>

#define N 16
#define SCALE 100

volatile float dac_float = 0;

volatile uint8_t estado = 0;
volatile int16_t coef[N] = {1,1,3,5,7,9,11,12,12,11,9,7,5,3,1,1};         // Coeficientes escalados ×1000
volatile uint8_t coef_idx = 0;

volatile float amostras[N] = {0};
volatile uint8_t idx_amostra = 0;

int16_t EEMEM coef_eeprom[N];

void exibir_tela_inicial();
void exibir_coeficiente();
void numIntoString(char *str, int start_pos,int num);

float aplicar_filtro_FIR();
void dac_set(uint8_t y);

volatile int numOVF0 = 0;
int main(void) {
	DDRB = 0xFF;  // <<<<<<<< PORTB todo como saída para DAC
	DDRD = 0xFC;
	DDRC = 0x30;

	// Inicialização do LCD e botões
	lcd_init();
	exibir_tela_inicial();

	// Inicializa ADC
	ad_init();

	// Inicializa contador para permitir mudar o coef. segurando o botão
	TCCR0A = 0x00;
	TCCR0B = 0x00;// 0x05 para clk/1024
	TCNT0 = 0x00;// zera timer0
	TIMSK0 = 0x01;

	/*
	// Carregar coeficientes da EEPROM
	for (uint8_t i = 0; i < N; i++) {
		coef[i] = (int16_t)eeprom_read_word((const uint16_t*)&coef_eeprom[i]);
	}
	*/
	// Interrupções dos botões
	PCICR = 0x02;
	PCMSK1 = 0x0E;
	OCR1A = 249;                 // 16 MHz / (64 * 1000) - 1
	TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler 64
	TIMSK1 |= (1 << OCIE1A);

	sei();
	
	uint8_t dac_value = 0;
	TCCR0B = 0x05;// 0x05 para clk/1024
	while (1) {
		
		if(numOVF0 > 30){
			numOVF0 = 0;
			if((PINC & 0x0E) == 0x0C) { // S2 - ?
				if (estado != 0) {
					coef[coef_idx] += 1;
					if (coef[coef_idx] > 1000) coef[coef_idx] = 1000;
					eeprom_write_word((uint16_t*)&coef_eeprom[coef_idx], coef[coef_idx]);
					exibir_coeficiente();
				}
			}

			if((PINC & 0x0E) == 0x06) { // S1 - ?
				if (estado != 0) {
					coef[coef_idx] -= 1;
					if (coef[coef_idx] < -1000) coef[coef_idx] = -1000;
					eeprom_write_word((uint16_t*)&coef_eeprom[coef_idx], coef[coef_idx]);
					exibir_coeficiente();
				}
			}
		}
		
		for (int i = 15; i > 1;i--)
		{
			amostras[i] = amostras[i - 1];
		}
		
		dac_float = ad_get(0);
		dac_float = (dac_float - 512)/512;
		amostras[0] = dac_float;
		// Normaliza para 8 bits para DAC (0-255)
		// Ajusta para que a saída fique dentro do intervalo 0..255
		// Assumindo que saída pode ser negativa, faz offset e escala:

		//int32_t saida_dac = saida + 32768; // desloca valor para positivo (assumindo max valor 16 bits)
		//if (saida_dac < 0) saida_dac = 0;
		//if (saida_dac > 65535) saida_dac = 65535;

		//uint8_t dac_value = (uint8_t)(saida >> 2);  // pega os 8 bits mais significativos
		//(dac_value < 256)dac_value++;
		
		dac_float = aplicar_filtro_FIR();
		
		dac_float = dac_float*127 + 127;
		dac_value = (uint32_t)(dac_float);
		dac_set(dac_value);
	}
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
	
	//_delay_us(500);//delay para segurar o sinal
}

ISR(PCINT1_vect) {
	_delay_ms(50); // debounce

	if((PINC & 0x0E) == 0x0A) { // S3 - M
		if (estado == 0) {
			estado = 1;
			coef_idx = 0;
			exibir_coeficiente();
			} else {
			coef_idx++;
			if (coef_idx >= N) {
				exibir_tela_inicial();
				} else {
				exibir_coeficiente();
			}
		}
	}

	if((PINC & 0x0E) == 0x0C) { // S2 - ?
		if (estado != 0) {
			coef[coef_idx] += 1;
			if (coef[coef_idx] > 1000) coef[coef_idx] = 1000;
			eeprom_write_word((uint16_t*)&coef_eeprom[coef_idx], coef[coef_idx]);
			exibir_coeficiente();
		}
	}

	if((PINC & 0x0E) == 0x06) { // S1 - ?
		if (estado != 0) {
			coef[coef_idx] -= 1;
			if (coef[coef_idx] < -1000) coef[coef_idx] = -1000;
			eeprom_write_word((uint16_t*)&coef_eeprom[coef_idx], coef[coef_idx]);
			exibir_coeficiente();
		}
	}
}

ISR(TIMER0_OVF_vect){
	numOVF0++;
}

ISR(TIMER1_COMPA_vect) {
	uint16_t nova_amostra = ad_get(0);
	amostras[idx_amostra] = nova_amostra;
	idx_amostra = (idx_amostra + 1) % N;

	//int32_t saida = aplicar_filtro_FIR();
	
	
}

float aplicar_filtro_FIR() {
	float acc = 0;
	float coeficiente = 0;
	
	for (uint8_t i = 0; i < N; i++) {
		coeficiente = coef[i]/100;
		uint8_t j = (idx_amostra - i + N) % N;
		acc += amostras[j] * coeficiente;
	}
	
	return acc;
}

void exibir_tela_inicial() {
	lcd_cmd(0x01, 0);
	lcd_cmd(0x02,0);
	lcd_write("    ELE 3717  ");
	lcd_cmd(0xc0,0);
	lcd_write("   FILTRO FIR  ");
	estado = 0;
}

void exibir_coeficiente() {
	char buffer[16] = "C00:         ";
	buffer[1] = (coef_idx / 10) + '0';
	buffer[2] = (coef_idx % 10) + '0';
	int16_t valor = coef[coef_idx];
	if (valor >= 0) buffer[8] = '+';
	else {
		buffer[8] = '-';
		valor = -valor;
	}
	numIntoString(buffer, 13, valor);
	lcd_cmd(0x02,0);
	lcd_write("   Coeficiente");
	lcd_cmd(0xc0, 0);
	lcd_write(buffer);
}

void numIntoString(char *str, int start_pos,int num) {
	str[start_pos-2] = (num/100)%10 + '0';
	str[start_pos-1] = (num/10)%10 + '0';
	str[start_pos] = num%10 + '0';
}
