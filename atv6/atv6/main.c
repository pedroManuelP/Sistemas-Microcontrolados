#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdlib.h>

#define set_bit(Y,bit_X) (Y|=(1<<bit_X))
#define clr_bit(Y,bit_X) (Y&=~(1<<bit_X))
#define cpl_bit(Y,bit_X) (Y^=(1<<bit_X))
#define tst_bit(Y,bit_X) (Y&(1<<bit_X))
#define pulso_enable() _delay_us(1); set_bit(LCD_CTRL,EN); _delay_us(100); clr_bit(LCD_CTRL,EN); _delay_us(45)

#define RS PD2
#define EN PD3
#define LCD_DATA PORTD
#define LCD_CTRL PORTD

#define N 16
#define SCALE 100

volatile uint8_t estado = 0;
volatile int16_t coef[N];         // Coeficientes escalados ×100
volatile uint8_t coef_idx = 0;

volatile uint16_t amostras[N] = {0};
volatile uint8_t idx_amostra = 0;

int16_t EEMEM coef_eeprom[N];

void lcd_cmd(unsigned char c, char cd);
void lcd_write(char *c);
void lcd_init();
void exibir_tela_inicial();
void exibir_coeficiente();
void numIntoString(char *str, int start_pos,int num);
uint16_t ad_get(uint8_t canal);
int32_t aplicar_filtro_FIR();

int main(void) {
	DDRB = 0xFF;  // <<<<<<<< PORTB todo como saída para DAC
	DDRD = 0xFC;
	DDRC = 0x30;

	// Inicialização do LCD e botões
	lcd_init();
	exibir_tela_inicial();

	// Inicializa ADC
	ADMUX = (1 << REFS0); // Referência AVcc
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Prescaler 64

	// Carregar coeficientes da EEPROM
	for (uint8_t i = 0; i < N; i++) {
		coef[i] = (int16_t)eeprom_read_word((const uint16_t*)&coef_eeprom[i]);
	}

	// Interrupções dos botões
	PCICR = 0x02;
	PCMSK1 = 0x0E;
	OCR1A = 249;                 // 16 MHz / (64 * 1000) - 1
	TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler 64
	TIMSK1 |= (1 << OCIE1A);

	sei();
	//8_t dac_value = 0;
	while (1) {
		int32_t saida = ad_get(0);

		// Normaliza para 8 bits para DAC (0-255)
		// Ajusta para que a saída fique dentro do intervalo 0..255
		// Assumindo que saída pode ser negativa, faz offset e escala:

		//int32_t saida_dac = saida + 32768; // desloca valor para positivo (assumindo max valor 16 bits)
		//if (saida_dac < 0) saida_dac = 0;
		//if (saida_dac > 65535) saida_dac = 65535;

		uint8_t dac_value = (uint8_t)(saida >> 2);  // pega os 8 bits mais significativos
		//(dac_value < 256)dac_value++;
		if(tst_bit(dac_value,7)){
			set_bit(PORTB,5);
			}else {
			clr_bit(PORTB,5);
		}
		if(tst_bit(dac_value,6)){
			set_bit(PORTB,4);
			}else{
			clr_bit(PORTB,4);
		}
		if(tst_bit(dac_value,5)){
			set_bit(PORTB,3);
			}else{
			clr_bit(PORTB,3);
		}
		if(tst_bit(dac_value,4)){
			set_bit(PORTB,2);
			}else{
			clr_bit(PORTB,2);
		}
		if(tst_bit(dac_value,3)){
			set_bit(PORTB,1);
			}else{
			clr_bit(PORTB,1);
		}
		if(tst_bit(dac_value,2)){
			set_bit(PORTB,0);
			}else{
			clr_bit(PORTB,0);
		}
		
		if(tst_bit(dac_value,1)){
			set_bit(PORTC,5);
			}else{
			clr_bit(PORTC,5);
		}
		if(tst_bit(dac_value,0)){
			set_bit(PORTC,4);
			}else{
			clr_bit(PORTC,4);
		}
		
		_delay_ms(2);
		
		aplicar_filtro_FIR();
	}
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

	if((PINC & 0x0E) == 0x0C) { // S2 - ▲
		if (estado != 0) {
			coef[coef_idx] += 1;
			if (coef[coef_idx] > 1000) coef[coef_idx] = 1000;
			eeprom_write_word((uint16_t*)&coef_eeprom[coef_idx], coef[coef_idx]);
			exibir_coeficiente();
		}
	}

	if((PINC & 0x0E) == 0x06) { // S1 - ▼
		if (estado != 0) {
			coef[coef_idx] -= 1;
			if (coef[coef_idx] < -1000) coef[coef_idx] = -1000;
			eeprom_write_word((uint16_t*)&coef_eeprom[coef_idx], coef[coef_idx]);
			exibir_coeficiente();
		}
	}
}

ISR(TIMER1_COMPA_vect) {
	uint16_t nova_amostra = ad_get(0);
	amostras[idx_amostra] = nova_amostra;
	idx_amostra = (idx_amostra + 1) % N;

	//int32_t saida = aplicar_filtro_FIR();
	
	
}

int32_t aplicar_filtro_FIR() {
	int32_t acc = 0;
	for (uint8_t i = 0; i < N; i++) {
		uint8_t j = (idx_amostra - i + N) % N;
		acc += (int32_t)amostras[j] * coef[i];
	}
	return acc / SCALE;
}

uint16_t ad_get(uint8_t canal){
	clr_bit(DDRC,canal);//direciona pino do canal como entrada
	ADMUX = 0x40 | canal;//seleciona canal
	set_bit(ADCSRA,ADSC);//inicia conversão
	while (!(ADCSRA & (1 << ADIF)));//espera conversão acabar
	_delay_ms(1);
	return ADC;//devolve sinal convertido
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

void lcd_cmd(unsigned char c, char cd){
	LCD_DATA = (c & 0xF0) | (LCD_DATA & 0x0F);
	if(cd==0) clr_bit(LCD_CTRL,RS);
	else set_bit(LCD_CTRL,RS);
	pulso_enable();
	if((cd==0) && (c < 4)) _delay_ms(2);

	LCD_DATA = ((c & 0x0F) << 4) | (LCD_DATA & 0x0F);
	if(cd==0) clr_bit(LCD_CTRL,RS);
	else set_bit(LCD_CTRL,RS);
	pulso_enable();
	if((cd==0) && (c < 4)) _delay_ms(2);
}

void lcd_write(char *c) {
	for (; *c!='\0'; c++) lcd_cmd(*c,1);
}

void lcd_init(){
	DDRD = 0xFC;
	clr_bit(LCD_CTRL,EN);
	clr_bit(LCD_CTRL,RS);
	_delay_ms(50);
	lcd_cmd(0x03,0); _delay_ms(10);
	lcd_cmd(0x03,0); _delay_us(200);
	lcd_cmd(0x03,0); lcd_cmd(0x02,0);
	lcd_cmd(0x28,0);
	lcd_cmd(0x08,0);
	lcd_cmd(0x01,0);
	lcd_cmd(0x0C,0);
	lcd_cmd(0x80,0);
}
