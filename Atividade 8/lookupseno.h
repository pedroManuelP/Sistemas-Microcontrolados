#ifndef LOOKUPSENO_H
#define LOOKUPSENO_H
#include <stdint.h>
#include <math.h>
#define PI 3.14159265
#define NUM_TAPS 360
void generate_sine_wave(uint8_t *buffer, uint8_t amplitude, float frequency, float sample_rate);

#endif