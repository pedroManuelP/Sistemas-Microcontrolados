#include "lookupseno.h"

#include <stdint.h>
#include <math.h>

#define PI 3.14159265
#define NUM_TAPS 360

/**
 * Generates a sine wave into a buffer for 8-bit DAC output (0–5V).
 *
 * @param buffer      Pointer to an array of 360 bytes to store the waveform.
 * @param amplitude_v Peak amplitude in volts (e.g., 2.0 for ±2 V swing).
 * @param offset_v    DC offset in volts (e.g., 2.5 to center the wave in 0–5 V).
 * @param frequency   Frequency in hertz (Hz).
 * @param sample_rate Number of samples per second (Hz).
 */
void generate_sine_wave(uint8_t *buffer, float amplitude_v, float offset_v, float frequency, float sample_rate) {
    float phase = 0.0f;
    float phase_increment = 2.0f * PI * frequency / sample_rate;

    float v_to_dac = 255.0f / 5.0f;  // conversion factor: volts to DAC units

    for (int i = 0; i < NUM_TAPS; i++) {
        float sine_val = sinf(phase);  // range: -1.0 to +1.0

        // Voltage at this point: V = offset + sin(t) * amplitude
        float voltage = offset_v + sine_val * amplitude_v;

        // Clamp voltage to 0–5V
        if (voltage > 5.0f) voltage = 5.0f;
        if (voltage < 0.0f) voltage = 0.0f;

        // Convert to DAC value
        uint8_t dac_value = (uint8_t)(voltage * v_to_dac + 0.5f);  // +0.5 for rounding
        buffer[i] = dac_value;

        phase += phase_increment;
        if (phase >= 2.0f * PI) phase -= 2.0f * PI;
    }
}


void sine_rad(uint8_t *buffer, uint8_t amplitude, float phase_increment) {
    float phase = 0.0;
    for (int i = 0; i < NUM_TAPS; i++) {
        float sine_val = sin(phase);  // range: -1.0 to +1.0
        // scale: center at 128, amplitude max ±127
        int16_t sample = 128 + (int16_t)(sine_val * amplitude);
        if (sample > 255) sample = 255;
        if (sample < 0)   sample = 0;
        buffer[i] = (uint8_t)sample;
        phase += phase_increment;
        if (phase >= 2 * PI) phase -= 2 * PI;  // wrap around
    }
}