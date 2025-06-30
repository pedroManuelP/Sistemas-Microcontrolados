#include "dac_8bit.h"
#include <avr/io.h>


void DAC_Init(void) {
    // Set A4 (PC4) and A5 (PC5) as outputs
    DDRC |= (1 << DDC4) | (1 << DDC5);
    
    // Set D8-D13 (PB0-PB5) as outputs
    DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | 
            (1 << DDB3) | (1 << DDB4) | (1 << DDB5);
    
    // Initialize to 0V
    DAC_Write(0);
}

void DAC_Write(uint8_t value) {
  // Write bits 7..2 to PORTB
  PORTB = (PORTB & 0b11000000) | ((value >> 2) & 0b00111111);

  // Write bits 1 and 0 to PORTC: PC4 (bit1), PC5 (bit0)
  PORTC = (PORTC & 0b11001111)
         | ((value << 3) & 0b00110000); // bit1 to PC4, bit0 to PC5
}


void DAC_Write_Reversed(uint8_t value) {
  /*
    Reversed Bit Mapping:
    D13 (PB5) = bit7 (MSB)
    D12 (PB4) = bit6
    D11 (PB3) = bit5
    D10 (PB2) = bit4
    D9  (PB1) = bit3
    D8  (PB0) = bit2
    A4  (PC4) = bit1
    A5  (PC5) = bit0 (LSB)
  */

  // Handle PORTC (A4 and A5)
  PORTC = (PORTC & 0xCF) // Clear PC4 & PC5 (binary 11001111)
         | ((value << 4) & 0x30); // bit1 → PC4, bit0 → PC5

  // Handle PORTB (D8 to D13)
  PORTB = (PORTB & 0xC0) // Preserve upper 2 bits
         | ((value >> 2) & 0x3F); // bits 2-7 → PB0-PB5
}


