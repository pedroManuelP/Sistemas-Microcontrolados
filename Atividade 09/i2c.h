#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <util/delay.h>

void TWI_init(void);
void TWI_start(void);
void TWI_stop(void);
void TWI_write(uint8_t data);
uint8_t TWI_read_ack(void);
uint8_t TWI_read_nack(void);

#endif
