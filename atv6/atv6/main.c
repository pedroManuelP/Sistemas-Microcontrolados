#include "defs_principais_AVR.h"
#include "lcd_AVR.h"
#include "ad_AVR.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include <stdio.h>

#define N 16

volatile float dac_float = 0;

volatile uint8_t estado = 0;
volatile uint16_t coef[N] = {0};
volatile uint8_t coef_idx = 0;

volatile float coef_float[N] = {0.01, 0.01, 0.03, 0.05, 0.07, 0.09, 0.11, 0.12, 0.12, 0.11, 0.09, 0.07, 0.05, 0.03, 0.01, 0.01};

volatile float amostras[N] = {0};
volatile uint8_t idx_amostra = 0;

float EEMEM coef_eeprom[N];

void exibir_tela_inicial();
void exibir_coeficiente();
float aplicar_filtro_FIR();
void dac_set(uint8_t y);

volatile int numOVF0 = 0;

int main(void) {
	DDRB = 0xFF;
	DDRD = 0xFC;
	DDRC = 0x30;

	lcd_init();
	exibir_tela_inicial();

	ad_init();

	TCCR0A = 0x00;
	TCCR0B = 0x00;
	TCNT0 = 0x00;
	TIMSK0 = 0x01;

	/*
	for (uint8_t i = 0; i < N; i++) {
		coef[i] = eeprom_read_float(&coef_eeprom[i]);
	}
	*/

	PCICR = 0x02;
	PCMSK1 = 0x0E;
	OCR1A = 249;
	TCCR1B |= (1 << CS11) | (1 << CS10);
	TIMSK1 |= (1 << OCIE1A);

	sei();

	uint8_t dac_value = 0;
	TCCR0B = 0x05;

	while (1) {
		if(numOVF0 > 30){
			numOVF0 = 0;

			if((PINC & 0x0E) == 0x0C && estado != 0) { // S2
				coef[coef_idx] += 0.01;
				if (coef[coef_idx] > 1.0) coef[coef_idx] = 1.0;
				eeprom_write_float(&coef_eeprom[coef_idx], coef[coef_idx]);
				exibir_coeficiente();
			}

			if((PINC & 0x0E) == 0x06 && estado != 0) { // S1
				coef[coef_idx] -= 0.01;
				//if (coef[coef_idx] < -1.0) coef[coef_idx] = -1.0;
				eeprom_write_float(&coef_eeprom[coef_idx], coef[coef_idx]);
				exibir_coeficiente();
			}
		}

		for (int i = N - 1; i > 0; i--) {
			amostras[i] = amostras[i - 1];
		}

		dac_float = ad_get(0);
		dac_float = (dac_float - 512.0) / 512.0;  // normaliza para -1.0 a +1.0
		amostras[0] = dac_float;

		dac_float = aplicar_filtro_FIR();
		dac_float = dac_float * 127.0 + 127.0;

		if (dac_float < 0.0) dac_float = 0.0;
		if (dac_float > 255.0) dac_float = 255.0;

		dac_value = (uint8_t)dac_float;
		dac_set(dac_value);
	}
}

void dac_set(uint8_t y){
	if(tst_bit(y,7)) set_bit(PORTB,5); else clr_bit(PORTB,5);
	if(tst_bit(y,6)) set_bit(PORTB,4); else clr_bit(PORTB,4);
	if(tst_bit(y,5)) set_bit(PORTB,3); else clr_bit(PORTB,3);
	if(tst_bit(y,4)) set_bit(PORTB,2); else clr_bit(PORTB,2);
	if(tst_bit(y,3)) set_bit(PORTB,1); else clr_bit(PORTB,1);
	if(tst_bit(y,2)) set_bit(PORTB,0); else clr_bit(PORTB,0);
	if(tst_bit(y,1)) set_bit(PORTC,5); else clr_bit(PORTC,5);
	if(tst_bit(y,0)) set_bit(PORTC,4); else clr_bit(PORTC,4);
}

ISR(PCINT1_vect) {
	_delay_ms(50);

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

	if((PINC & 0x0E) == 0x0C && estado != 0) { // S2
		coef[coef_idx] += 0.01;
		if (coef[coef_idx] > 1.0) coef[coef_idx] = 1.0;
		eeprom_write_float(&coef_eeprom[coef_idx], coef[coef_idx]);
		exibir_coeficiente();
	}

	if((PINC & 0x0E) == 0x06 && estado != 0) { // S1
		coef[coef_idx] -= 0.01;
		//if (coef[coef_idx] < -1.0) coef[coef_idx] = -1.0;
		eeprom_write_float(&coef_eeprom[coef_idx], coef[coef_idx]);
		exibir_coeficiente();
	}
}

ISR(TIMER0_OVF_vect){
	numOVF0++;
}

ISR(TIMER1_COMPA_vect) {
	float nova_amostra = ad_get(0);
	nova_amostra = (nova_amostra - 512.0) / 512.0;
	amostras[idx_amostra] = nova_amostra;
	idx_amostra = (idx_amostra + 1) % N;
}

float aplicar_filtro_FIR() {
	float acc = 0;
	for (uint8_t i = 0; i < N; i++) {
		acc += amostras[i] * coef_float[i];
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
	char linha[17];
	char valor_str[8];
	dtostrf(coef[coef_idx], 1, 2, valor_str);  // até 2 casas decimais

	sprintf(linha, "C%02d: %s", coef_idx, valor_str);

	lcd_cmd(0x02, 0);
	lcd_write("   Coeficiente");
	lcd_cmd(0xc0, 0);
	lcd_write(linha);
}
