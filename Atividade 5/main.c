/*
 * Project_05.c
 *
 * Created: 23/05/2025 01:03:54
 * Author : chamo
 */ 
#include "defs_principais_AVR.h"
#include "lcd_AVR.h"
#include "spi_avr.h"
#include "ad_AVR.h"
#include "max7219_AVR.h"
//===================================================================
#define CENTRO_DO_SINAL 0x01FF
#define DEAD_ZONE 0x0064

void ad_joystick(uint16_t sgn_hor, uint16_t sgn_ver);//recebe os sinais do joystick e muda a variável de movimento
void lcd_text_update();//atualiza o texto que mostra as posições dos segmentos da cobra

void snake_move(char mov);//atualiza as posições dos pedaços da cobra
void snake_status(uint8_t m_rows[8]);//verifica se o movimento é válido e move a cobra na matriz
void snake_perdeu(uint8_t m_rows[8]);

char lcd_text[2][17] = {"X:              \0","Y:              \0"};//texto do LCD
uint8_t matrix[8] = {0};

volatile uint8_t snake[8][2] = {0};
volatile uint8_t snake_size = 1;
volatile char mov = 'd';
volatile uint8_t segundosPartida = 0;
volatile uint8_t ciclosDelay = 10;
int main(void){
	TCCR1A = 0x00;//modo CTC
	OCR1A = 15624;
	TCNT1 = 0;
	set_bit(TIMSK1,OCIE1A);
	sei();

	//SDA -> PC4 -> A4 -> Horiz
	//SCL -> PC5 -> A5 -> Vert
	ad_init();
	lcd_init();
	spi_init();
	
	snake[0][1] = 2;
	snake[0][0] = 3;
	m_init();
	m_clear(matrix);
	TCCR1B = 0x0D;//inicia timer1 e pre-escala
	//INICIO DO LOOP INFINITO
	while (1){
		ad_joystick(ad_get(4),ad_get(5));
		snake_move(mov);
		snake_status(matrix);
		m_update(matrix);
		lcd_text_update();

		if(segundosPartida == 5){
			if(ciclosDelay > 2)ciclosDelay--;
			segundosPartida = 0;
			if(snake_size < 8)snake_size++;
			}
		
		for (int i = 0;i < ciclosDelay;i++){
			_delay_ms(50);
		}
	}
	//FIM DO LOOP INFINITO
}

// ===================SNAKE CODE=========================
void snake_move(char mov){
	for(int i = (snake_size-1);i > 0;i--){
		snake[i][0] = snake[i-1][0];
		snake[i][1] = snake[i-1][1];
	}
	switch(mov){
		case 'w':
		snake[0][1]++;
		break;
		case 's':
		snake[0][1]--;
		break;
		case 'd':
		snake[0][0]++;
		break;
		case 'a':
		snake[0][0]--;
		break;
		default:
		break;
	}
}

void snake_status(uint8_t m_rows[8]){
	uint8_t bateu = 0;
	if( ((snake[0][0] > 7) | (snake[0][0] < 0)) | ((snake[0][1] > 7) | (snake[0][1] < 0)))bateu = 1;
	
	for(int i = 1; i < snake_size;i++){
		if((snake[0][0] == snake[i][0]) && (snake[0][1] == snake[i][1])) bateu = 1;
	}

	if(bateu){
		segundosPartida = 0;
		snake_perdeu(m_rows);
		m_clear(matrix);
		for (int i = 1;i < 8;i++){
			snake[i][1] = 0;
			snake[i][0] = 0;
		}
		snake[0][1] = 3;
		snake[0][0] = 4;
		mov = 'd';
		ciclosDelay = 10;
		TCNT1 = 0;
		segundosPartida = 0;
		snake_size = 1;
	}

	m_clear(m_rows);
	for (int i = 0;i < snake_size;i++){
		set_bit(m_rows[ snake[i][0] ],snake[i][1]);
	}
}

void snake_perdeu(uint8_t m_rows[8]){
	for(int i = 0;i < 5;i++){
		m_clear(m_rows);
		_delay_ms(500);
		
		//Caveira - Obs: 
		m_rows[0] = 0b00111100;
		m_rows[1] = 0b11110010;
		m_rows[2] = 0b01110010;
		m_rows[3] = 0b11011110;
		m_rows[4] = 0b01110010;
		m_rows[5] = 0b11110010;
		m_rows[6] = 0b00111100;
		m_rows[7] = 0x00;
		/*
		m_rows[0] = 0b00111100;
		m_rows[1] = 0b01000010;
		m_rows[2] = 0b10100101;
		m_rows[3] = 0b10010001;
		m_rows[4] = 0b10010001;
		m_rows[5] = 0b10100101;
		m_rows[6] = 0b01000010;
		m_rows[7] = 0b00111100;
		*/
		m_update(m_rows);
		_delay_ms(500);
	}
}
// ===================LCD TEXT UPDATE=========================
void lcd_text_update(){
	for(int i = 0; i < 2; i++){
		for (int w = 0; w < 7; w++){
			lcd_text[i][2*w+3] = snake[w][i] + '0';
			lcd_text[i][2*w+2] = ' ';
		}
	}
	
	lcd_cmd(0x01,0);
	lcd_cmd(0x02,0);
	lcd_write(lcd_text[1]);
	_delay_ms(5);
	lcd_cmd(0xC0,0);
	lcd_write(lcd_text[0]);
}

// ===================JOYSTICK=========================
void ad_joystick(uint16_t sgn_hor, uint16_t sgn_ver){
	if(sgn_hor < CENTRO_DO_SINAL - DEAD_ZONE)mov = 's';//
	if(sgn_hor > CENTRO_DO_SINAL + DEAD_ZONE)mov = 'w';//
	if(sgn_ver > CENTRO_DO_SINAL + DEAD_ZONE)mov = 'a';//cima
	if(sgn_ver < CENTRO_DO_SINAL - DEAD_ZONE)mov = 'd';//baixo
}

ISR(TIMER1_COMPA_vect){
	segundosPartida++;
}
