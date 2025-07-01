#ifndef DAC_8BIT_H
#define DAC_8BIT_H

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
// Initialize DAC pins
void DAC_Init(void);

// Write 8-bit value to DAC (0-255)
void DAC_Write(uint8_t value);
void DAC_Write_Reversed(uint8_t value);
#endif // DAC_8BIT_H