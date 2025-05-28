#ifndef _LCD_AVR_H
#define _LCD_AVR_H

#ifndef RS
#define RS PD2
#endif

#ifndef EN
#define EN PD3
#endif

#ifndef LCD_DATA
#define LCD_DATA PORTD
#endif

#ifndef pulso_enable()
#define pulso_enable() _delay_us(1); set_bit(LCD_DATA,EN); _delay_us(100); clr_bit(LCD_DATA,EN); _delay_us(45)
#endif

void lcd_cmd(unsigned char c, char cd);
void lcd_write(char *c);
void lcd_init();

void lcd_cmd(unsigned char c, char cd){
	//---------------Upper_nibble---------------
	LCD_DATA = (c & 0xF0) | (LCD_DATA & 0x0F);
	
	if(cd==0)
		clr_bit(LCD_DATA,RS);
	else
		set_bit(LCD_DATA,RS);
	pulso_enable();
	
	if((cd==0) && (c < 4)) _delay_ms(2);
	//---------------Lower_nibble---------------
	LCD_DATA = ((c & 0x0F) << 4) | (LCD_DATA & 0x0F);
	
	if(cd==0)
		clr_bit(LCD_DATA,RS);
	else
		set_bit(LCD_DATA,RS);
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
	clr_bit(LCD_DATA,EN);
	clr_bit(LCD_DATA,RS);
	_delay_ms(50);
	
	// Instruções: Set DDRAM adress
	LCD_DATA = (0x33 & 0xF0) | (LCD_DATA & 0x0F);
	pulso_enable();
	LCD_DATA = ((0x33 & 0x0F) << 4) | (LCD_DATA & 0x0F);
	pulso_enable();
	
	// Instruções: 
	LCD_DATA = (0x32 & 0xF0) | (LCD_DATA & 0x0F);
	pulso_enable();
	LCD_DATA = ((0x32 & 0x0F) << 4) | (LCD_DATA & 0x0F);
	pulso_enable();
	
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


#endif
