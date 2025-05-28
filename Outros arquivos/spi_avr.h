#ifndef SPI_AVR_H
#define SPI_AVR_H

#ifndef MOSI
#define MOSI PB3
#endif

#ifndef SS
#define SS PB2
#endif

#ifndef SCK
#define SCK PB5
#endif

void spi_init();
void spi_write(uint8_t data);

// INICIALIZANDO SPI
void spi_init(void){
	DDRB |= (1 << MOSI) | (1 << SCK) | (1 << SS); // INICIANDO MOSI, SCK, SS
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); // spi enable, master, clk/16
}

// ESCREVENDO COM SPI
void spi_write(uint8_t data){
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));//espera o byte ser enviado
}
#endif