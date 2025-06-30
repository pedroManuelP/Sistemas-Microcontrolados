#ifndef ADC_H
#define ADC_H

#include <stdint.h>

// Initialize ADC for A0 input
void ADC_Init(void);

// Read 10-bit analog value from A0 (0-1023)
uint16_t ADC_Read(void);

#endif // ADC_H