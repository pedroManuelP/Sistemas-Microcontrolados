#include "lookuptriangular.h"


void gen_triangular_wave(uint8_t *buffer, float amplitude_v, float offset_v, uint8_t duty_cycle){
    float v_to_dac = 255.0f / 5.0f; 
    float incHIGH = amplitude_v/((float) duty_cycle);
    float incLOW = amplitude_v/((float) (100.0f-duty_cycle));
    float currentVoltage = offset_v;
    for(uint8_t i =0;i<100;i++){
        buffer[i] = (uint8_t) (currentVoltage*v_to_dac+0.5f);
        if(i<duty_cycle) currentVoltage += incHIGH;
        else currentVoltage -= incLOW;
    }
}