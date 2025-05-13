/*
 * Project_4.c
 *
 * Created: 12/05/2025 17:21:12
 * Author : chamo
 */ 
#include <avr/io.h>
#define F_CPU 16000000UL // define clock do sistema
#include <util/delay.h>
#include <avr/pgmspace.h>

#define set_bit(Y,bit_X) (Y|=(1<<bit_X)) // seta um bit
#define clr_bit(Y,bit_X) (Y&=~(1<<bit_X)) // clear um bit
#define cpl_bit(Y,bit_X) (Y^=(1<<bit_X)) // troca um bit
#define tst_bit(Y,bit_X) (Y&(1<<bit_X)) // testa um bit(isola bit)
#define pulso_enable() _delay_us(1); set_bit(CTRL_BUS,EN); _delay_us(1); clr_bit(CTRL_BUS,EN); _delay_us(45)

#define RS PD2
#define EN PD3
#define DATA_BUS PORTD // PD7 ate PD4
#define CTRL_BUS PORTD // PD7 ate PD4
#define PINRED PB3
#define PINGREEN PB2
#define PINBLUE PB1

const unsigned char msg1[] PROGMEM = "Pedro Manuel";

void lcd_cmd(unsigned char c, char cd);
void lcd_write(char *c);
void lcd_init();

int main(void)
{
	unsigned char i;
	DDRB = 0b00001110;
	DDRD = 0b11111100;
	//unsigned char *valor;
	//*valor = 0x38;
	//data8bit(valor);
	
	lcd_init();	
	lcd_cmd(0x01,0);
	lcd_cmd(0x02,0);
	for(i=0;i<16;i++) //enviando caractere por caractere
		lcd_cmd(pgm_read_byte(&msg1[i]),1); //lê na memória flash e usa cmd_LCD
		
	lcd_cmd(0xC0,0); //desloca o cursor para a segunda linha do LCD
	lcd_write("Pereira Chamorro");//a cadeia de caracteres é criada na RAM
	
    while (1) 
    {
		set_bit(PORTB,PINRED);
		_delay_ms(1000);
		clr_bit(PORTB,PINRED);
		_delay_ms(1000);
	}
	return 0;
}

void lcd_cmd(unsigned char c, char cd){
	unsigned char c_temp;
	//----------Upper_Nibble----------
	c_temp = c;
	
	c_temp &= 0xF0;
	DATA_BUS = c_temp; 
	
	if(cd==0)
		clr_bit(CTRL_BUS,RS);
	else
		set_bit(CTRL_BUS,RS);
	pulso_enable();
	
	if((cd==0) && (c < 4)) _delay_ms(2);
	
	//----------Lower_Nibble----------
	c_temp = c;
	c_temp &= 0x0F;
	c_temp = (c_temp << 4);
	DATA_BUS = c_temp;
	
	if(cd==0)
		clr_bit(CTRL_BUS,RS);
	else
		set_bit(CTRL_BUS,RS);
	pulso_enable();
	
	if((cd==0) && (c < 4)) _delay_ms(2);
	//-------------------------------
	
	return;
}

void lcd_write(char *c)
{
	for (; *c!=0;c++) lcd_cmd(*c,1);
}

void lcd_init(){
	unsigned char c;
	
	clr_bit(CTRL_BUS,RS);
	_delay_ms(15);
	
	//-----Passa_a_instrucao_0x38-----
	c = 0x28 & 0xF0;
	DATA_BUS = c;
	pulso_enable();
	
	c = 0x28 & 0x0F;
	c = (c << 4);
	DATA_BUS = c;
	pulso_enable();
	//------------------------------
	
	_delay_ms(5);
	pulso_enable();
	_delay_us(200);
	pulso_enable();
	pulso_enable();
	
	lcd_cmd(0x08,0);
	lcd_cmd(0x01,0);
	lcd_cmd(0x0C,0);
	lcd_cmd(0x80,0);
	return;
}