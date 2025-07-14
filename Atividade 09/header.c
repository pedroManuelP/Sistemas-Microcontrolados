#include "header.h"


char str_accel_x[8], str_accel_y[8], str_accel_z[8];
char str_temp[8];
char str_euler_x[8], str_euler_y[8], str_euler_z[8];
char str_gyro_x[8], str_gyro_y[8], str_gyro_z[8];

void updateDisplay(uint8_t type, int16_t *gyro, float *temperatura, float* euler, float * accelF){
    dtostrf(accelF[0], 5, 1, str_accel_x);
dtostrf(accelF[1], 5, 1, str_accel_y);
dtostrf(accelF[2], 5, 1, str_accel_z);

dtostrf(temperatura[0], 5, 1, str_temp);

dtostrf(euler[0], 5, 1, str_euler_x);
dtostrf(euler[1], 5, 1, str_euler_y);
dtostrf(euler[2], 5, 1, str_euler_z);

// Gyro is integer-based, use snprintf
snprintf(str_gyro_x, sizeof(str_gyro_x), "%d", gyro[0]);
snprintf(str_gyro_y, sizeof(str_gyro_y), "%d", gyro[1]);
snprintf(str_gyro_z, sizeof(str_gyro_z), "%d", gyro[2]);

    switch (type)
    {
    case case_accel:
        lcd_clear();
        lcd_goto(0,0);
        lcd_print("x(g)  y(g)  z(g)");
        lcd_goto(1,0);
        lcd_print(str_accel_x);
        lcd_print(str_accel_y);
        lcd_print(str_accel_z);
        break;
    case case_gyro:
        lcd_clear();
        lcd_goto(0,0);
        lcd_print("x(d)  y(d)  z(d)");
        lcd_goto(1,0);
        lcd_print(str_gyro_x);
        lcd_print(" ");
        lcd_print(str_gyro_y);
        lcd_print(" ");
        lcd_print(str_gyro_z);
    break;
    case case_euler:
        lcd_clear();
        lcd_goto(0,0);
        lcd_print("x  y  z  temp");
        lcd_goto(1,0);
        lcd_print(str_euler_x);
        
        lcd_print(str_euler_y);
       
        lcd_print(str_euler_z);
        lcd_print(" ");
        lcd_print(str_temp);
    break;
    default:
        break;
    }
}