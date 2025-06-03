#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define set_bit(Y,bit_X) (Y|=(1<<bit_X)) // set num bit
#define clr_bit(Y,bit_X) (Y&=~(1<<bit_X)) // clear num bit
#define cpl_bit(Y,bit_X) (Y^=(1<<bit_X)) // inverte um bit
#define tst_bit(Y,bit_X) (Y&(1<<bit_X)) // testa um bit(isola um bit)
#define pulso_enable() _delay_us(1); set_bit(LCD_CTRL,EN); _delay_us(100); clr_bit(LCD_CTRL,EN); _delay_us(45)

#define RS PD2
#define EN PD3
#define LCD_DATA PORTD // PD7 a PD4
#define LCD_CTRL PORTD

void lcd_cmd(unsigned char c, char cd);
void lcd_write(char *c);
void lcd_init();
void exibir_tela_inicial();
void exibir_coeficiente();
void numIntoString(char *str, int start_pos,int num);

volatile uint8_t estado = 0; // 0: tela inicial, 1 a 16: ajuste coeficiente 0-15
volatile int16_t coef[16];   // 16 coeficientes do FIR
volatile uint8_t coef_idx = 0; // índice atual (0 a 15)

char linha_inferior[] = "Coef00: +0000\0"; // buffer da linha inferior

// EEPROM (endereços 0 a 31 para os 16 coef)
int16_t EEMEM coef_eeprom[16];

int main(void) {
	DDRB = 0x0E;
	DDRD = 0xFC; // LCD no PORTD (PD7-PD4: dados, PD3: EN, PD2: RS)
	DDRC = 0x00; // Botões no PC1, PC2, PC3

	PCICR = 0x02; // Ativa interrupção no PCINT[14:8] (PORTC)
	PCMSK1 = 0x0E; // PC1, PC2, PC3
	TIMSK0 = 0x01;
	sei(); // Habilita interrupções globais

	lcd_init();
	lcd_cmd(0x01, 0);
	lcd_cmd(0x02,0);
	lcd_write("    ELE 3717  ");
	lcd_cmd(0xc0,0);
	lcd_write("   FILTRO FIR  ");

	// Carregar coeficientes da EEPROM
	//for (uint8_t i = 0; i < 16; i++) {
	//	coef[i] = (int16_t)eeprom_read_word((const uint16_t*)&coef_eeprom[i]);
	//}

	exibir_tela_inicial();

	while (1) {
		// Nada no loop principal - lógica controlada via interrupções
	}
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
	// Preencher índice e valor
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

void lcd_cmd(unsigned char c, char cd){
	//---------------Upper_nibble---------------
	LCD_DATA = (c & 0xF0) | (LCD_DATA & 0x0F);
	
	if(cd==0)
	clr_bit(LCD_CTRL,RS);
	else
	set_bit(LCD_CTRL,RS);
	pulso_enable();
	
	if((cd==0) && (c < 4)) _delay_ms(2);
	//---------------Lower_nibble---------------
	LCD_DATA = ((c & 0x0F) << 4) | (LCD_DATA & 0x0F);
	
	if(cd==0)
	clr_bit(LCD_CTRL,RS);
	else
	set_bit(LCD_CTRL,RS);
	pulso_enable();
	
	if((cd==0) && (c < 4)) _delay_ms(2);
	////------------------------------------------
}

void lcd_write(char *c)
{
	for (; *c!='\0';c++) lcd_cmd(*c,1);
}

void lcd_init(){
	DDRD = 0xFC;
	clr_bit(LCD_CTRL,EN);
	clr_bit(LCD_CTRL,RS);
	_delay_ms(50);
	
	lcd_cmd(0x03,0);
	_delay_ms(10);
	lcd_cmd(0x03,0);
	_delay_us(200);
	lcd_cmd(0x03,0);
	lcd_cmd(0x02,0);
	
	lcd_cmd(0x28,0);// Instruções: Canal de 4 bits, 2 linhas, caracteres de 5x10 bits
	lcd_cmd(0x08,0); //Display off, Cursor off, Blink off
	lcd_cmd(0x01,0); //Clear display, DDRAM address in counter = 0
	lcd_cmd(0x0C,0); //Display on, Cursor off, Blink off
	lcd_cmd(0x80,0); //DDRAM address = 0 (primeira posição na esquerda)
}
void numIntoString(char *str, int start_pos,int num) {
	str[start_pos-2] = (num/100)%10 + '0';
	str[start_pos-1] = (num/10)%10 + '0';
	str[start_pos] = num%10 + '0';
}

ISR(PCINT1_vect) {
	_delay_ms(50); // debounce simples

	if((PINC & 0x0E) == 0x0A) { // S3 - M
		if (estado == 0) {
			estado = 1;
			coef_idx = 0;
			exibir_coeficiente();
			} else {
			coef_idx++;
			if (coef_idx >= 16) {
				exibir_tela_inicial();
				} else {
				exibir_coeficiente();
			}
		}
	}

	if((PINC & 0x0E) == 0x0C) { // S2 - ?
		if (estado != 0) {
			coef[coef_idx]++;
			if (coef[coef_idx] > 1000) coef[coef_idx] = 1000; // limite superior
			eeprom_write_word((uint16_t*)&coef_eeprom[coef_idx], coef[coef_idx]);
			exibir_coeficiente();
		}
	}

	if((PINC & 0x0E) == 0x06) { // S1 - ?
		if (estado != 0) {
			coef[coef_idx]--;
			if (coef[coef_idx] < -1000) coef[coef_idx] = -1000; // limite inferior
			eeprom_write_word((uint16_t*)&coef_eeprom[coef_idx], coef[coef_idx]);
			exibir_coeficiente();
		}
	}
}