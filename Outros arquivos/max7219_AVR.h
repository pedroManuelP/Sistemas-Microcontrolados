/*
 * max7219_AVR.h
 *
 * Created: 29/05/2025 21:24:43
 *  Author: chamo
 */ 


#ifndef MAX7219_AVR_H_
#define MAX7219_AVR_H_

void m_send(uint8_t address, uint8_t data);//transmite os 2 byts para o max7219
void m_init();//inicializa o max7219
void m_clear(uint8_t m_rows[8]);//apaga todos os led
void m_set_load(uint8_t m_rows[8],uint8_t row, uint8_t col, uint8_t value);//acende um LED específico
void m_update(uint8_t m_rows[8]);//acende ou apaga os leds de acordo com o vetor inserido

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
void m_clear(uint8_t m_rows[8]){
	for(uint8_t i = 0; i<8; i++){
		m_rows[i] = 0;//zera as colunas de cada dígito
		m_send(i+1, 0x00);//apaga os leds de cada coluna
	}
}

void m_set_load(uint8_t m_rows[8],uint8_t row, uint8_t col, uint8_t value){
	if(value) m_rows[row] |= (1 << col);//se value = 1, seta o bit 'col' da linha 'row'
	else m_rows[row] &= ~(1 << col);//c.c, zera o bit
	
	m_send(row+1, m_rows[row]);//aciona o bit especificado
}

void m_update(uint8_t m_rows[8]){
	for (int i = 0;i<8;i++)
	{
		for (int w = 0;w < 8;w++)
		{
			if(tst_bit(m_rows[i],w)){
				m_set_load(m_rows,i,w,1);
				}else{
				m_set_load(m_rows,i,w,0);
			}
		}
	}
}

#endif /* MAX7219_AVR_H_ */
