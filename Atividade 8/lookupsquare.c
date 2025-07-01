#include "lookupsquare.h"



void gen_square_wave(uint8_t *buffer, float amplitude_v, float offset_v, uint8_t duty_cycle){
    float v_to_dac = 255.0f / 5.0f; 
    for(uint8_t i=0; i<100;i++){
    if(i<duty_cycle) buffer[i] = (uint8_t) ((offset_v+amplitude_v)*(v_to_dac)+0.5f);
    else buffer[i] = (uint8_t) ((offset_v-amplitude_v)*v_to_dac+0.5f);
 }   
 
}