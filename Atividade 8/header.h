#ifndef HEADER_H
#define HEADER_H
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
void writeHeader(short int type, short int duty_cycle, short int status, short int freq, float amplitude, float offset);
void updateVars(volatile short int type, volatile short int dutycycle, volatile short int status, volatile short int frequencia, volatile float amplitude, volatile float offset);
//gVars
volatile short int gType =0;
volatile short int gDutyCycle= 50;
volatile short int gstatus=1;
volatile short int gfreq=20;
volatile float gamplitude = 0.1F;
volatile float goffset = 0.2F;
volatile uint8_t controle = 0x01;




#endif
