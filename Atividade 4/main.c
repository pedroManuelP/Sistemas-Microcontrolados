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
#define RED OCR2A
#define GREEN OCR1B
#define BLUE OCR1A

void lcd_cmd(unsigned char c, char cd);
void lcd_write(char *c);
void lcd_init();
void mde(char s);
void numIntoString(char str[], int start_pos,int num);

volatile char estado = 0;
volatile char mudarRGB = 0;
volatile char mudarTexto = 1;
char bottom_line[] = "  0    0    0\0";
volatile short int numRed = 0;
volatile short int numGreen = 0;
volatile short int numBlue = 0;

int main(void)
{
	TCCR1A = 0xF1;//COM2A1:0 = 3  COM2B1:0 = 3  WGM13:0 = 5
	TCCR1B = 0x09;//clk sem pre-escala
	
	TCCR2A = 0xC3; // COM2A1:0 = 2   WGM22:0 = 3
	TCCR2B = 0x01;// 0x01 clk sem pre-escala

	BLUE = 0x0000;
	GREEN = 0x0000;
	RED = 0x00;
	
	DDRB = 0x0E;
	DDRC = 0x00;
	DDRD = 0xFC;
	
	PCICR = 0x02;
	PCMSK1 = 0x0E;
	sei();
	
	lcd_init();	
	lcd_cmd(0x01,0);
	lcd_cmd(0x02,0);
	while (1) 
	{
		if(RED == 0x00){
			clr_bit(TCCR2A,7);
			clr_bit(TCCR2A,6);
			set_bit(PORTB,3);
			}else{
			set_bit(TCCR2A,7);
			set_bit(TCCR2A,6);
		}
		if(GREEN == 0x0000){
			clr_bit(TCCR1A,5);
			clr_bit(TCCR1A,4);
			set_bit(PORTB,2);
			}else{
			set_bit(TCCR1A,5);
			set_bit(TCCR1A,4);
		}
		if(BLUE == 0x0000){
			clr_bit(TCCR1A,7);
			clr_bit(TCCR1A,6);
			set_bit(PORTB,1);
			}else{
			set_bit(TCCR1A,7);
			set_bit(TCCR1A,6);
		}
		
		mde(estado);
		
		_delay_ms(250);
		switch (estado)
		{
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
			numRed += 50;
			break;
			default:
			break;
		}
		mudarTexto = 1;
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

void numIntoString(char str[], int start_pos,int num){
	int digitos;
	if(num > 100){
		digitos = 3;
	}else if(num < 100 && num >= 10){
		digitos = 2;
	}else{
		digitos = 1;
	}
	
	
	for (int i = 0; i++; i < digitos){
		str[start_pos - i] = num + '0';
	}
	
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
			lcd_cmd(0xC0,0);
			
			numIntoString(bottom_line, 2, numRed);
			bottom_line[3] = ' ';
			bottom_line[8] = ' ';
			bottom_line[13] = ' ';
			lcd_write(bottom_line);
			mudarTexto = 0;
		}
		break;
	case 1:
		if(mudarRGB == 1){
			numRed += 5;
			mudarRGB = 0;
		}else if(mudarRGB == 2){
			numRed -= 5;
			mudarRGB = 0;
		}else{}
		RED = numRed;
		
		if(mudarTexto == 1){
			lcd_cmd(0x01,0);
			lcd_cmd(0x02,0);
			lcd_write("RED GREEN BLUE\0");
			_delay_ms(10);
			lcd_cmd(0xC0,0); //desloca o cursor para a segunda linha do LCD
			
			bottom_line[3] = '*';
			bottom_line[8] = ' ';
			bottom_line[13] = ' ';
			lcd_write(bottom_line);
			mudarTexto = 0;
		}
		break;
	case 2:
		if(mudarRGB){
			numGreen += 5;
			mudarRGB = 0;
		}else if(mudarRGB == 2){
			numGreen -= 5;
			mudarRGB = 0;
		}else{}
		GREEN = numGreen;
		
		if(mudarTexto == 1){
			lcd_cmd(0x01,0);
			lcd_cmd(0x02,0);
			lcd_write("RED GREEN BLUE\0");
			_delay_ms(10);
			lcd_cmd(0xC0,0);
			
			bottom_line[3] = ' ';
			bottom_line[8] = '*';
			bottom_line[13] = ' ';
			lcd_write(bottom_line);
			mudarTexto = 0;
		}
		break;
	case 3:
		if(mudarRGB == 1){
			numBlue += 5;
			mudarRGB = 0;
		}else if(mudarRGB == 2){
			numBlue -= 5;
			mudarRGB = 0;
		}else{}
		BLUE = numBlue;
		
		if(mudarTexto == 1){
			lcd_cmd(0x01,0);
			lcd_cmd(0x02,0);
			lcd_write("RED GREEN BLUE\0");
			_delay_ms(10);
			lcd_cmd(0xC0,0);
			
			bottom_line[3] = ' ';
			bottom_line[8] = ' ';
			bottom_line[13] = '*';
			lcd_write(bottom_line);
			mudarTexto = 0;
		}
		break;
	default:
		break;
	}
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
		mudarTexto = 1;
		mudarRGB = 0;
	}else{}
	
	if(PINC == 0x0C) mudarRGB = 1;
	if(PINC == 0x06) mudarRGB = 2;
}
