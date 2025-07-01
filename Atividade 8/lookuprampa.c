#include "lookuprampa.h"


void gen_rampa_wave(uint8_t *buffer, float amplitude_v, float offset_v){
    float v_to_dac = 255.0f / 5.0f;
    float currentVoltage = offset_v;
     
    for(uint8_t i=0; i<100;i++){
    buffer[i] = (uint8_t) (currentVoltage*v_to_dac+0.5f);
    currentVoltage += amplitude_v/100.0f;
    }
}