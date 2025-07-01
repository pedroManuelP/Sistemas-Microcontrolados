#include <stdint.h>
#include <math.h>
#include "dac_8bit.h"
#include "lookupseno.h"

#define PI 3.14159265
#define NUM_TAPS 360

void generate_sine_wave2(uint8_t *buffer, float amplitude_v, float offset_v, float frequency, float sample_rate) {
    float phase = 0.0f;
    float phase_increment = 2.0f * PI * frequency / sample_rate;

    float v_to_dac = 255.0f / 5.0f;

    for (int i = 0; i < NUM_TAPS; i++) {
        float sine_val = sinf(phase);
        float voltage = offset_v + sine_val * amplitude_v;

        if (voltage > 5.0f) voltage = 5.0f;
        if (voltage < 0.0f) voltage = 0.0f;

        uint8_t dac_value = (uint8_t)(voltage * v_to_dac + 0.5f);
        buffer[i] = dac_value;

        phase += phase_increment;
        if (phase >= 2.0f * PI) phase -= 2.0f * PI;
    }
}

int main(void) {
    uint8_t seno[360];
    generate_sine_wave(seno, 1, 2, 30, 1000);
    DAC_Init();
    while (1){
        for (int i = 0; i < 360; i++)
        {
            DAC_Write(seno[i]);
            _delay_ms(20);
        }
        
    }
}
