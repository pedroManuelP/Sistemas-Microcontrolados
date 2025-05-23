/*
 * Project_05.c
 *
 * Created: 23/05/2025 01:03:54
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
#define LCD_DATA PORTD
#define LCD_CTRL PORTD
#define DIN PB3
#define LOAD PB2
#define CLK PB5

void lcd_cmd(unsigned char c, char cd);
void lcd_write(char *c);
void lcd_init();
void text_update();

void spi_init();
void spi_write(uint8_t data);

void m_send(uint8_t address, uint8_t data);
void m_init();
void m_clear();
void m_set_load(uint8_t row, uint8_t col, uint8_t value);

char lcd_text[2][17] = {"X:              \0","Y:              \0"};
char cobra[8][2] = {0};
short int mudarTexto = 1;
int main(void)
{
	DDRD = 0xFC;
	
	cobra[0][0] = 3;
	cobra[0][1] = 4;
	
	spi_init();
	m_init();
	lcd_init();
	m_clear();
    while (1){
		//m_clear();
		
		m_set_load(3,5,1);//acende led 3,5
		_delay_ms(500);
		m_set_load(3,5,0);//desliga led 3,5
		_delay_ms(500);
		
		text_update();
    }
}

void text_update(){
	for(int i = 0;i++;i<2){
		for (int w = 0;w++;w<7){
			lcd_text[i][2+w] = cobra[w][i] + '0';
		}
	}
	
	lcd_cmd(0x01,0);
	lcd_cmd(0x02,0);
	lcd_write(lcd_text[0]);
	lcd_cmd(0xC0,0);
	lcd_write(lcd_text[1]);
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
	
	// Instruções: Set DDRAM adress
	LCD_DATA = (0x33 & 0xF0) | (LCD_DATA & 0x0F);
	clr_bit(LCD_CTRL,RS);
	pulso_enable();
	LCD_DATA = ((0x33 & 0x0F) << 4) | (LCD_DATA & 0x0F);
	clr_bit(LCD_CTRL,RS);
	pulso_enable();
	
	// Instruções:
	LCD_DATA = (0x32 & 0xF0) | (LCD_DATA & 0x0F);
	clr_bit(LCD_CTRL,RS);
	pulso_enable();
	LCD_DATA = ((0x32 & 0x0F) << 4) | (LCD_DATA & 0x0F);
	clr_bit(LCD_CTRL,RS);
	pulso_enable();
	
	// Instruções: Canal de 4 bits, 2 linhas, caracteres de 5x10 bits
	LCD_DATA = (0x28 & 0xF0) | (LCD_DATA & 0x0F);
	clr_bit(LCD_CTRL,RS);
	pulso_enable();
	LCD_DATA = ((0x28 & 0x0F) << 4) | (LCD_DATA & 0x0F);
	clr_bit(LCD_CTRL,RS);
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

//==============SPI=====================

// INICIALIZANDO SPI
void spi_init(void){
	DDRB |= (1 << 3) | (1 << 5) | (1 << 2); // INICIANDO MOSI, SCK, SS
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); // spi enable, master, clk/16
}

// ESCREVENDO COM SPI
void spi_write(uint8_t data){
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
}

// ===================MAX7219=========================
void m_send(uint8_t address, uint8_t data){
	clr_bit(PORTB,LOAD);
	spi_write(address);//envia 1 byte
	spi_write(data);//envia o segundo byte
	set_bit(PORTB,LOAD);//envios os 16 bits
}

void m_init(){
	m_send(0x09, 0x00);//decode mode
	m_send(0x0A, 0x03);//intensidade
	m_send(0x0B, 0x07);//ScanLimit
	m_send(0x0C, 0x01);//normal operation
	m_send(0x0F, 0x00);//test mode off
	
	for(uint8_t i = 1;i++;i <=8){
			m_send(i,0x00);//clear
	}
}

uint8_t m_rows[8] = {0}; //memoria da matriz
	
// funcao para dar um clear na matriz
void m_clear(void){
	for(uint8_t i = 0; i<8; i++){
		m_rows[i] = 0;
		m_send(i+1, 0x00);
	}
}

void m_set_load(uint8_t row, uint8_t col, uint8_t value){
	if(value) m_rows[row] |= (1 << col);
	else m_rows[row] &= ~(1 << col);
	
	m_send(row+1, m_rows[row]);
}
