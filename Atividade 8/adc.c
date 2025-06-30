#include "adc.h"
#include <avr/io.h>
#include <util/delay.h>

void ADC_Init(void) {
    // Set reference to AVcc (5V) and left-adjust result disabled
    ADMUX = (1 << REFS0);
    
   
   ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADC_Read(void) {
    // Select ADC channel 0 (A0 = PC0)
    ADMUX = (ADMUX & 0xF0) | (0 & 0x0F);
    
    // Start single conversion
    ADCSRA |= (1 << ADSC);
    
    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC));
    
    // Return 10-bit result
    return ADC;
}