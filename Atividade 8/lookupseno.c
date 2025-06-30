#include "lookupseno.h"

void generate_sine_wave(uint8_t *buffer, uint8_t amplitude, float frequency, float sample_rate) {
    float phase = 0.0f;
    float phase_increment = 2.0f * PI * frequency / sample_rate;

    for (int i = 0; i < NUM_TAPS; i++) {
        float sine_val = sinf(phase);  // -1.0 to +1.0
        int16_t sample = 128 + (int16_t)(sine_val * amplitude);  // Centered at 128
        if (sample > 255) sample = 255;
        if (sample < 0)   sample = 0;
        buffer[i] = (uint8_t)sample;

        phase += phase_increment;
        if (phase >= 2.0f * PI) phase -= 2.0f * PI;
    }
}