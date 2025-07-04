#define F_CPU 16000000UL
#include "adc.h"
#include "dac_8bit.h"
#include "lcd.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "eeprom.h"
#include "lookuprampa.h"
#include "lookupseno.h"
#include "lookupsquare.h"
#include "lookuptriangular.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define botaoS1 1
#define botaoS2 2
#define botaoS3 3
#define botaoS4 4
#define quadrada_t 0
#define triangular_t 1
#define rampa_t 2
#define sen_t 3
#define chType 0
#define chDutyCycle 1
#define chStatus 2
#define chFreq 3
#define chAmpl 4
#define chOff 5
#define botaoM botaoS1
#define botaoA botaoS3
#define botaoUP botaoS2
#define botaoDW botaoS4


//gVars
volatile short int gType =0;
volatile short int gDutyCycle= 50;
volatile short int gstatus=1;
volatile short int gfreq=20;
volatile float gamplitude = 0.1F;
volatile float goffset = 0.2F;
volatile uint8_t controle = 0x01;

void updateVars(volatile short int type, volatile short int dutycycle, volatile short int status, volatile short int frequencia, volatile float amplitude, volatile float offset);

ISR(PCINT1_vect);
void setInterruptions();
void writeHeader(short int type, short int duty_cycle, short int status, short int freq, float amplitude, float offset);
void setup();
void loop();


void setInterruptions(){
    PCICR = (1<<PCIE1);
    PCMSK1 = (1<<PCINT8) | (1<<PCINT9) | (1<<PCINT10) | (1<<PCINT11) ;
    sei();
}



void writeHeader(short int type, short int duty_cycle, short int status, short int freq, float amplitude, float offset){
    //0 = quadrada, 1= triang, 2=rampa,3=sen
    char offset_s[7];
    char duty_cycle_s[6];
    char amplitude_s[7];
    char freq_s[6];
    dtostrf(offset, 5, 1, offset_s);
    dtostrf(amplitude, 5, 1, amplitude_s);
    snprintf(duty_cycle_s, sizeof(duty_cycle_s), "%d",duty_cycle);
    snprintf(freq_s, sizeof(freq_s), "%d",freq);
    switch (type)
    {
    case quadrada_t:
        lcd_goto(0,0);
        lcd_print("T:QUA ");
        lcd_print("D:");
        lcd_print(duty_cycle_s);
        lcd_print("%   ");
        if(status) lcd_print("ON");
        else lcd_print("OFF");
        lcd_goto(1,0);
        lcd_print(freq_s);
        lcd_print("Hz");
        lcd_print(amplitude_s);
        lcd_print("V");
        lcd_print(offset_s);
        lcd_print("V");

        break;
    case triangular_t:
        lcd_goto(0,0);
        lcd_print("T:TRI ");
        lcd_print("D:");
        lcd_print(duty_cycle_s);
        lcd_print("%   ");
        if(status) lcd_print("ON");
        else lcd_print("OFF");
        lcd_goto(1,0);
        lcd_print(freq_s);
        lcd_print("Hz");
        lcd_print(amplitude_s);
        lcd_print("V");
        lcd_print(offset_s);
        lcd_print("V");
    break;
    case rampa_t:
        lcd_goto(0,0);
        lcd_print("T:RAM        ");
        if(status) lcd_print("ON");
        else lcd_print("OFF");
        lcd_goto(1,0);
        lcd_print(freq_s);
        lcd_print("Hz");
        lcd_print(amplitude_s);
        lcd_print("V");
        lcd_print(offset_s);
        lcd_print("V");
    break;
    case sen_t:
        lcd_goto(0,0);
        lcd_print("T:SEN        ");
        if(status) lcd_print("ON");
        else lcd_print("OFF");
        lcd_goto(1,0);
        lcd_print(freq_s);
        lcd_print("Hz");
        lcd_print(amplitude_s);
        lcd_print("V");
        lcd_print(offset_s);
        lcd_print("V");
    break;
    default:
        break;
    }
}

int main(void){
    setup();
    loop();
}


void setup(){
    setInterruptions();
    lcd_init();
    ADC_Init();
    DAC_Init();
}
#define fHertz 50
int delayfreqUs(uint8_t isSine, uint8_t freqHertz){
    if(isSine) return 3*10000/freqHertz;
    else return 10000/freqHertz;
}

void loop(){
    uint8_t buffer[360];
    //gen_square_wave(buffer, 2.5, 2.5, 50);
    //gen_triangular_wave(buffer, 2.0, 2.0, 80);
    //gen_rampa_wave(buffer, 2.0, 2.0);
    generate_sine_wave(buffer, 2.5, 2.5, 30, 1000);
    int frequency = delayfreqUs(1, 99);
    while(1){
        for(int i =0; i<360; i++){
            DAC_Write(buffer[i]);
            _delay_us(frequency);
        }
        
    }
}


void updateVars(volatile short int type, volatile short int dutycycle, volatile short int status, volatile short int frequencia, volatile float amplitude, volatile float offset){
    bool validType = (type >= 0 && type < 4);
    
  
    bool validDutyCycle = (dutycycle > 0 && dutycycle < 100);
    

    bool validStatus = (status == 0 || status == 1);
    
    bool validFreq = (frequencia > 0 && frequencia <= 100);

    bool validAmplitude = (amplitude >= 0.0f);
    
   
    bool validVoltage = ((offset + amplitude) <= 5.0f) && 
                       ((offset - amplitude) >= 0.0f);
    
    if (validType && validDutyCycle && validStatus && 
        validFreq && validAmplitude && validVoltage) {
        gType = type;
        gDutyCycle = dutycycle;
        gstatus = status;
        gfreq = frequencia;
        gamplitude = amplitude;
        goffset = offset;
    }
}
ISR(PCINT1_vect){
    if(~PINC&(1<<botaoUP)){
        while(~PINC&(1<<botaoUP)){
            switch (controle)
            {
            case (1<<chType):
                    updateVars(gType+1,gDutyCycle,gstatus,gfreq, gamplitude,goffset);
                break;
            case (1<<chDutyCycle):
                    updateVars(gType,gDutyCycle+1,gstatus,gfreq, gamplitude,goffset);
                break;
            case (1<<chStatus):
                    updateVars(gType,gDutyCycle,(gstatus ? 0:1),gfreq, gamplitude,goffset);
                break;
            case (1<<chFreq):
                    updateVars(gType,gDutyCycle,gstatus,gfreq+1, gamplitude,goffset);
                break;
            case (1<<chAmpl):
                    updateVars(gType,gDutyCycle,gstatus,gfreq, gamplitude+0.1F,goffset);
                break;
            case (1<<chOff):
                    updateVars(gType,gDutyCycle,gstatus,gfreq, gamplitude,goffset+0.1F);
                break;
            default:
                break;
            }
            writeHeader(gType, gDutyCycle, gstatus,gfreq,gamplitude, goffset);
            _delay_ms(1000);
        }
    }
    else if(~PINC&(1<<botaoM)){
        controle = controle<<1;
        if(controle>(1<<chOff)) controle=0x01;
    }
    
    writeHeader(gType,gDutyCycle,gstatus,gfreq, gamplitude,goffset);

    
    
}