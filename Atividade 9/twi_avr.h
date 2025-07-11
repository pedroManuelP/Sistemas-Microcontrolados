/*
 * twi_avr.h
 *
 * Created: 10/07/2025 13:15:25
 *  Author: chamo
 */ 

#ifndef TWI_AVR_H_
#define TWI_AVR_H_

void TWI_init_master(uint8_t BIT_RATE_DIV, uint8_t PRESCALER_BITS){
	//SCL Frequency = 16MHz/(16 + 2*(TWBR)*(PrescalerValue))
	TWBR = BIT_RATE_DIV;
	TWSR = (TWSR & 0xFC)|(PRESCALER_BITS);
}

void TWI_start(){
	TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);// send START condition
	while(!(TWCR & (1 << TWINT))); // wait for START condition to be transmitted
	while((TWSR & 0xF8)!= 0x08); // Check for the acknowledgement
}

void TWI_read_address(unsigned char data){
	TWDR = data; // address and read instruction
	TWCR = (1 << TWINT)|(1 << TWEN); // Clear TWI interrupt flag, enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte received
	while((TWSR & 0xF8)!= 0x40); 
}

void TWI_write_data(unsigned char data)
{
	TWDR=data;    // put data in TWDR
	TWCR=((1 << TWINT)|(1 << TWEN));    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
	while((TWSR & 0xF8) != 0x28); // Check for the acknoledgement
}

void TWI_stop(void)
{
	// Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while(!(TWCR & (1<<TWSTO)));  // Wait till stop condition is transmitted
}

uint8_t TWI_read_data(void)
{
	TWCR=((1 << TWINT)|(1 << TWEN));    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1 << TWINT))); // Wait till complete TWDR byte transmitted
	while((TWSR & 0xF8) != 0x58); // Check for the acknoledgement
	return TWDR;
}

void TWI_master_transmitter(unsigned char read_address, unsigned char write_data){
	TWI_start();
	TWI_read_address(read_address);
	TWI_write_data(write_data);
	TWI_stop();
}

uint8_t TWI_master_receiver(unsigned char address, unsigned char data){
	TWI_start();
	TWI_read_address(address);
	uint8_t recv_data = TWI_read_data();
	TWI_stop();
	return recv_data;
}

#endif /* TWI_AVR_H_ */