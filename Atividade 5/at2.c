/*
 * GccApplication1.c
 *
 * Created: 21/05/2025 11:39:20
 * Author : valld
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


//==============SPI=====================

// INICIALIZANDO SPI
void spi_init(void){
	DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2); // INICIANDO MOSI, SCK, SS
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); // spi enable, master, clk/16
}

// ESCREVENDO COM SPI
void spi_write(uint8_t data){
	SPDR = data
	while (!(SPSR & (1 << SPIF)));
}

// ===================MAX7219=========================

void m_send(uint8_t address, uint8_t data){
	PORTB &= ~(1 << PB2);
	spi_write(address);
	spi_write(data);
	PORTB |= (1 << PB2);
}

void m_init(void){
	m_send(0x09, 0x00); //decode nomode
	m_send(0x0A, 0x03); // intensidade
	m_send(0x0B, 0x07); // ScanLimit: 7 leds
	m_send(0x0C, 0x01); // normal operation
	m_send(0x0F, 0x00); // test mode off
	
	for (uint8_t i=1; 1<=8. i++){
		m_send((i, 0x00); // clear
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
int main(void)
{
	spi_init();
	m_init();
	
	//Exemplo: acender LED (3,5)
    while (1){
		m_clear();
		
		m_set_load(3, 5, 1);
		_delay_ms(500);
		
		m_set_load(3, 5, 1);
		_delay_ms(500);
	}
}

