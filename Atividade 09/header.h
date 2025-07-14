#ifndef HEADER_H
#define HEADER_H

#define case_accel 1
#define case_gyro 2
#define case_euler 3
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
void updateDisplay(uint8_t type, int16_t *gyro, float *temperatura, int16_t* euler, float * accelF);


#endif