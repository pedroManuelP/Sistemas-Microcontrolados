/*
 * Project_05.c
 *
 * Created: 23/05/2025 01:03:54
 * Author : chamo
 */ 
#include <defs_principais_AVR.h>
#include <lcd_AVR.h>
#include <spi_avr.h>
#include <ad_AVR.h>

#define CENTRO_DO_SINAL 0x01FF
#define DEAD_ZONE 0x0064

void m_send(uint8_t address, uint8_t data);
void m_init();
void m_clear();
void m_set_load(uint8_t row, uint8_t col, uint8_t value);

void ad_joystick(uint16_t sgn_hor, uint16_t sgn_ver);

void lcd_text_update();
void m_update();
void snake_into_m();
void snake_move(uint8_t direction);
void snake_status();

char lcd_text[2][17] = {"                \0","                \0"};
volatile uint8_t m_rows[8] = {0};
	
volatile uint8_t snake[8][2] = {0};
volatile uint8_t snake_size = 1;
volatile uint8_t mov = 2;

volatile uint16_t sinal_vert;
volatile uint16_t sinal_hori;
//volatile int tempoDeJogoSeg; 
int main(void){
	/*
	TCCR1A = 0b00000000;//CTC
	TCCR1B = 0x00;
	OCR1A = 15624;//1 seg = 15625 contagens para clk/1024
	set_bit(TIMSK1,OCIE1A);//overflow interrupt enable
	*/
	
	DDRC = 0x00;
	//SDA -> PC4 -> A4 -> Horiz
	//SCL -> PC5 -> A5 -> Vert
	
	ad_init();
	lcd_init();
	spi_init();
	m_init();
	m_clear();
	
	/*
	TCCR1B = 0b00001101;//comeca timer
	tempoDeJogoSeg = 0;
	*/
	
	//INICIO DO LOOP
    while (1){
		ad_joystick(ad_get(5),ad_get(4));//lê o joystick e mude a direção da cobra
		_delay_ms(500);
		
		snake_move(mov);
		snake_status();
		snake_into_m();//atualiza a posição da cobra na matriz m_rows[]
		
		lcd_text_update();//atualiza o texto do LCD
		_delay_ms(10);
    }
	//FIM DO LOOP
}
void snake_move(uint8_t direction){
	for(int i = (snake_size-1);i > 0;i--){
		snake[i][0] = snake[i-1][0];
		snake[i][1] = snake[i-1][1];
	}
	if(direction == 0)snake[0][0]--;
	if(direction == 1)snake[0][0]++;
	if(direction == 2)snake[0][1]--;
	if(direction == 3)snake[0][1]++;
}

void snake_status(){
	uint8_t bateu = 0;
	if( ((snake[0][0] > 7) | (snake[0][0] < 0)) | ((snake[0][1] > 7) | (snake[0][1] < 0)))bateu = 1;
	
	for(int i = 4; i < 8;i++){
		if((snake[0][0] == snake[i][0]) && (snake[0][1] == snake[i][1])) bateu = 1;
	}
	
	if(bateu){
		m_clear();
		for (int i = 0;i < 8;i++){
			snake[i][0] = 0;
			snake[i][1] = 0;
		}
		mov = 3;
	}
}

void snake_into_m(){
	m_clear();
	for (int i = 0;i < snake_size;i++){
			m_set_load(snake[i][0],snake[i][1],1);
	}
}

void lcd_text_update(){
	for(int i = 0; i < 2; i++){
		for (int w = 0; w < 8; w++){
			lcd_text[i][2*w+1] = snake[w][i] + '0';
			lcd_text[i][2*w] = ' ';
		}
	}
	lcd_cmd(0x01,0);
	lcd_cmd(0x02,0);
	lcd_write(lcd_text[0]);
	lcd_cmd(0xC0,0);
	lcd_write(lcd_text[1]);
}

// ===================MAX7219=========================
void m_send(uint8_t address, uint8_t data){
	clr_bit(PORTB,SS);
	spi_write(address);//transmite adress byte
	spi_write(data);//transmite data byte
	set_bit(PORTB,SS);//envia os 2 bytes
}

void m_init(){
	m_send(0x09, 0x00);//decode mode
	m_send(0x0A, 0x03);//intensity
	m_send(0x0B, 0x07);//ScanLimit
	m_send(0x0C, 0x01);//normal operation
	m_send(0x0F, 0x00);//test mode off
	
	for(uint8_t i = 1; i <= 8; i++){
			m_send(i,0x00);//clear
	}
}
	
// funcao para dar um clear na matriz
void m_clear(void){
	for(uint8_t i = 0; i<8; i++){
		m_rows[i] = 0;//zera as colunas de cada dígito
		m_send(i+1, 0x00);//apaga os leds de cada coluna
	}
}

void m_set_load(uint8_t row, uint8_t col, uint8_t value){
	if(value) m_rows[row] |= (1 << col);//se value = 1, seta o bit 'col' da linha 'row'
	else m_rows[row] &= ~(1 << col);//c.c, zera o bit
	
	m_send(row+1, m_rows[row]);//aciona o bit especificado
}

// ===================CONVERSOR AD=========================
void ad_joystick(uint16_t sgn_hor, uint16_t sgn_ver){
		if((sgn_ver > CENTRO_DO_SINAL+DEAD_ZONE) && (sgn_hor > CENTRO_DO_SINAL - DEAD_ZONE) && (sgn_hor < CENTRO_DO_SINAL + DEAD_ZONE))mov = 3;
		if((sgn_ver < CENTRO_DO_SINAL-DEAD_ZONE) && (sgn_hor > CENTRO_DO_SINAL - DEAD_ZONE) && (sgn_hor < CENTRO_DO_SINAL + DEAD_ZONE))mov = 2;
		if((sgn_hor > CENTRO_DO_SINAL+DEAD_ZONE) && (sgn_ver > CENTRO_DO_SINAL - DEAD_ZONE) && (sgn_ver < CENTRO_DO_SINAL + DEAD_ZONE))mov = 0;
		if((sgn_hor < CENTRO_DO_SINAL-DEAD_ZONE) && (sgn_ver > CENTRO_DO_SINAL - DEAD_ZONE) && (sgn_ver < CENTRO_DO_SINAL + DEAD_ZONE))mov = 1;
}

/*
// ===================INTERRUPÇÃO CTC=========================
ISR(TIMER1_COMPA_vect){
	tempoDeJogoSeg++;
}
*/
