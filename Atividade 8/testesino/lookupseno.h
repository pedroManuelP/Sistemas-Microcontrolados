#ifndef LOOKUPSENO_H
#define LOOKUPSENO_H
#include <stdint.h>
#include <math.h>
#define PI 3.14159265
#define NUM_TAPS 360
void sine_rad(uint8_t * buffer, uint8_t amplitude, float phase_increment);
void generate_sine_wave(uint8_t *buffer, float amplitude_v, float offset_v, float frequency, float sample_rate);
#endif