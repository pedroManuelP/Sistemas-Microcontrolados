/*
 * Project_4.c
 *
 * Created: 12/05/2025 17:21:12
 * Author : chamo
 */ 
#include <avr/io.h>
#define F_CPU 16000000UL // define clock do sistema
#include <util/delay.h>
#include <avr/interrupt.h>

#define set_bit(Y,bit_X) (Y|=(1<<bit_X)) // set num bit
#define clr_bit(Y,bit_X) (Y&=~(1<<bit_X)) // clear num bit
#define cpl_bit(Y,bit_X) (Y^=(1<<bit_X)) // inverte um bit
#define tst_bit(Y,bit_X) (Y&(1<<bit_X)) // testa um bit(isola um bit)
#define pulso_enable() _delay_us(1); set_bit(LCD_CTRL,EN); _delay_us(100); clr_bit(LCD_CTRL,EN); _delay_us(45)

#define RS PD2
#define EN PD3
#define LCD_DATA PORTD // PD7 ate PD4
#define LCD_CTRL PORTD
#define PINRED PB3
#define PINGREEN PB2
#define PINBLUE PB1

void lcd_cmd(unsigned char c, char cd);
void lcd_write(char *c);
void lcd_init();
void mde(char s);

char estado = 0;
volatile char mudarRGB = 0;
volatile char mudarTexto = 0;

int main(void)
{
	TCCR1A = 0b10100001;//COM2A1:0 = 3  COM2B1:0 = 3  WGM13:0 = 5
	TCCR1B = 0b00001001;//clk sem pre-escala
	
	TCCR2A = 0b10000011; // COM2A1:0 = 2   WGM22:0 = 3
	TCCR2B = 0x01;// 0x01 clk sem pre-escala

	OCR1A = 0x0000;//blue
	OCR1B = 0x0000;//green
	OCR2A = 0x00;// red
	
	DDRB = 0b00001110;
	DDRC = 0x00000000;
	DDRD = 0b11111100;
	
	PCICR = 0b00000010;
	PCMSK1 = 0b00001110;
	sei();
	
	lcd_init();	
	lcd_cmd(0x01,0);
	lcd_cmd(0x02,0);
	lcd_write("RED GREEN BLUE\0");
	_delay_ms(10);
	lcd_cmd(0xC0,0); //desloca o cursor para a segunda linha do LCD
	lcd_write("  0    0    0\0");//a cadeia de caracteres é criada na RAM
	while (1) 
	{
		mde(estado);
	}
	return 0;
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
	
	return;
}

void lcd_write(char *c)
{
	for (; *c!='\0';c++) lcd_cmd(*c,1);
}

void lcd_init(){
	clr_bit(LCD_CTRL,EN);
	clr_bit(LCD_CTRL,RS);
	_delay_ms(50);
	
	// Instruções: Canal de 4 bits, 2 linhas, caracteres de 5x10 bits
	LCD_DATA = (0x28 & 0xF0) | (LCD_DATA & 0x0F);
	pulso_enable();
	
	LCD_DATA = ((0x28 & 0x0F) << 4) | (LCD_DATA & 0x0F);
	pulso_enable();
	
	_delay_ms(5);
	pulso_enable();
	_delay_us(200);
	pulso_enable();
	pulso_enable();
	
	lcd_cmd(0x08,0); //Display off, Cursor off, Blink off
	lcd_cmd(0x01,0); //Clear display, DDRAM address in counter = 0
	lcd_cmd(0x0C,0); //Display on, Cursor off, Blink off
	lcd_cmd(0x80,0); //DDRAM address = 0 (primeira posição na esquerda)
	return;
}

void mde(char s){
	switch(s){
	case 0:
		if(mudarTexto == 1){
			lcd_cmd(0x01,0);
			lcd_cmd(0x02,0);
			lcd_write("RED GREEN BLUE\0");
			_delay_ms(10);
			lcd_cmd(0xC0,0); //desloca o cursor para a segunda linha do LCD
			lcd_write("estado 1\0");//a cadeia de caracteres é criada na RAM
		}
		
		if(mudarRGB == 1){
			
		}
		break;
	case 1:
		OCR2A += 0x05;
		break;
	case 2:
		OCR1B += 0x0005;
		break;
	case 3:
		OCR1A += 0x0005;
		break;
	default:
		break;
	}
	if(mudarRGB == 1){
		switch(s){
			case 0:
			break;
			case 1:
			OCR2A += 0x05;
			break;
			case 2:
			OCR1B += 0x0005;
			break;
			case 3:
			OCR1A += 0x0005;
			break;
			default:
			break;
		}
		mudarRGB = 0;
	}else if(mudarRGB == 2){
		switch(s){
			case 0:
			break;
			case 1:
			OCR2A -= 0x05;
			break;
			case 2:
			OCR1B -= 0x0005;
			break;
			case 3:
			OCR1A -= 0x0005;
			break;
			default:
			break;
		}
		mudarRGB = 0;
	}else{}
	return;
}

ISR(PCINT1_vect) {
	if(PINC == 0x0A){
		switch(estado){
		case 0:
			estado = 1;
			break;
		case 1:
			estado = 2;
			break;
		case 2:
			estado = 3;
			break;
		case 3:
			estado = 0;
			break;
		default:
			break;
		}
	}else{}
	
	if(PINC == 0x0C){
		mudarRGB = 1;
	}else if(PINC == 0x06){
		mudarRGB = 2;
	}else{}
}
