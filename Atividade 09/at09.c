#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpu6050.h"
#include "header.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "semphr.h"

#define botaoS1 1
#define botaoS2 2
#define botaoS3 3
#define botaoS0 0


ISR(PCINT1_vect);
void setup();
SemaphoreHandle_t xSemaphore;
void setInterruptions();
void loadDisplay();
void readMPU();

int16_t g_accel_int[3];
int16_t g_gyro_int[3];
float g_euler_int[3];
float g_temperatura;
float g_accel_float[3];
uint8_t state;
void setup(){
    setInterruptions();
    lcd_init();
    MPU6050_init();
    xSemaphore = xSemaphoreCreateMutex();
    state =1;
}

void setInterruptions(){
	PCICR = (1<<PCIE1);
	PCMSK1 = (1<<PCINT8) | (1<<PCINT9) | (1<<PCINT10) | (1<<PCINT11) ;
	sei();
}

int main(){
    setup(); 
    xTaskCreate(loadDisplay, "loadDisplay", 256, NULL, 0, NULL);
    xTaskCreate(readMPU, "readMPU", 256, NULL, 0, NULL);
    vTaskStartScheduler();
    for(;;);
    return 0;
}

void readMPU(){
    for(;;){
    MPU6050_READ_SCALED(g_accel_int, g_gyro_int, &g_temperatura, g_euler_int, g_accel_float);
    vTaskDelay(50/portTICK_PERIOD_MS);
    }
}
void loadDisplay(){
    for(;;){
    updateDisplay(state, g_gyro_int, &g_temperatura, g_euler_int, g_accel_float);
    vTaskDelay(400/portTICK_PERIOD_MS);
    }
}

ISR(PCINT1_vect){
    if(~PINC&(1<<botaoS1)){
        state = 1;
    }
    else if(~PINC&(1<<botaoS2)) state =2;
    else if(~PINC&(1<<botaoS3)) state =3;
    else if(~PINC&(1<<botaoS0)) {
        for(uint8_t i =0; i<3; i++){
            g_euler_int[i] = 0;
        }
    }
}