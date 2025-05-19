/*
 * atividade05.c
 *
 * Created: 19/05/2025 17:21:08
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
#define pulso_enable() _delay_us(1); set_bit(LCD_CTRL,EN); _delay_us(100); clr_bit(LCD_CTRL,EN); _delay_us(45);
#define ld_matrix() _delay_us(1); set_bit(PORTB,LOAD); _delay_us(100); clr_bit(PORTB,LOAD); _delay_us(45);

#define EN PD3
#define RS PD2
#define LCD_DATA PORTD
#define LCD_CTRL PORTD
#define DIN PB3
#define LOAD PB2
#define CLK PB5

void lcd_cmd(unsigned char c, char cd);
void lcd_write(char *c);
void lcd_init();
void matrix_init();
void strings_init();

int posicoes[8][2];
char top_line[17];
char bottom_line[17];

int main(void)
{
    DDRB = 0b00101100;
	DDRD = 0b11111100;
	
	lcd_init();
	lcd_cmd(0x01,0);
	lcd_cmd(0x02,0);
	
	strings_init();
	matrix_init();
    while(1){
		_delay_ms(1000);
		lcd_cmd(0x01,0);
		lcd_cmd(0x02,0);
		lcd_write(top_line);
		_delay_ms(10);
		lcd_cmd(0xC0,0);
		lcd_write(bottom_line);
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

void strings_init(){
	for(int i = 0; i++; i < 8){
		for(int w = 0; w++; w < 2){
			posicoes[i][w] = 0;
		}
	}
	
	for(int i = 0; i++; i < 17){
		top_line[i] = ' ';
		bottom_line[i] = ' ';
	}
	top_line[0] = 'X';
	bottom_line[0] = 'Y';
	
	top_line[1] = ':';
	bottom_line[1] = ':';
	
	top_line[3] = '0';
	bottom_line[3] = '0';
	
	top_line[16] = '\0';
	bottom_line[16] = '\0';
	
	return;
}

void matrix_init(){
	short int cmd[16];
	
	//decod mode
	for(int i = 0; i++; i < 16)cmd[i] = 0;
	cmd[11] = 1;
	cmd[8] = 1;
	for(int i = 0; i++; i < 8)cmd[i] = 0;
	
	for(int i = 0;i++; i < 16){
		if(cmd[i] == 1){
			set_bit(PORTB,DIN);
		}else{
			clr_bit(PORTB,DIN);
		}
	}	
	ld_matrix();
	
	//intensity
	for(int i = 0; i++; i < 16)cmd[i] = 0;
	cmd[11] = 1;
	cmd[9] = 1;
	
	for(int i = 0; i++; i < 8)cmd[i] = 1;
	for(int i = 0;i++; i < 16){
		if(cmd[i] == 1){
			set_bit(PORTB,DIN);
			}else{
			clr_bit(PORTB,DIN);
		}
	}
	ld_matrix();
	
	//scan limit
	for(int i = 0; i++; i < 16)cmd[i] = 0;
	cmd[11] = 1;
	cmd[9] = 1;
	cmd[8] = 1;
	
	cmd[3] = 1; 
	for(int i = 0;i++; i < 16){
		if(cmd[i] == 1){
			set_bit(PORTB,DIN);
			}else{
			clr_bit(PORTB,DIN);
		}
	}
	ld_matrix();
	
	//shutdown
	for(int i = 0; i++; i < 16)cmd[i] = 0;
	cmd[11] = 1;
	cmd[10] = 1;
	cmd[0] = 1;
	
	for(int i = 0;i++; i < 16){
		if(cmd[i] == 1){
			set_bit(PORTB,DIN);
			}else{
			clr_bit(PORTB,DIN);
		}
	}
	ld_matrix();
	
	//display test
	for(int i = 0; i++; i < 16)cmd[i] = 0;
	cmd[11] = 1;
	cmd[10] = 1;
	cmd[9] = 1;
	cmd[8] = 1;
	
	for(int i = 0;i++; i < 16){
		if(cmd[i] == 1){
			set_bit(PORTB,DIN);
			}else{
			clr_bit(PORTB,DIN);
		}
	}
	ld_matrix();
	return;
}