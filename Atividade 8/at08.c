#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 16000000UL
#include <stdbool.h>
#include "adc.h"
#include "dac_8bit.h"
#include "lcd.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "eeprom.h"
#include "lookuptables.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "semphr.h"

#define botaoS1 1
#define botaoS2 2
#define botaoS3 3
#define botaoS0 0
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
#define botaoDW botaoS3
int delayMachine = 0;
ISR(PCINT1_vect);
void setInterruptions();
void setup();
static void writeLCD();
static void	dac();

SemaphoreHandle_t xSemaphore;
volatile uint8_t buffer[360];

int delayfreqUs(uint8_t isSine, uint8_t freqHertz){
	if(isSine) return 3*10000/freqHertz;
	else return 10000/freqHertz;
}
void setInterruptions(){
	PCICR = (1<<PCIE1);
	PCMSK1 = (1<<PCINT8) | (1<<PCINT9) | (1<<PCINT10) | (1<<PCINT11) ;
	sei();
}

void setup(){
	setInterruptions();
	lcd_init();
	DAC_Init();
	gen_square_wave(buffer, gamplitude, goffset, gDutyCycle);
	writeHeader(gType,gDutyCycle,gstatus,gfreq,gamplitude,goffset);
	xSemaphore = xSemaphoreCreateMutex();  
}

int main(){
	setup();
	
	//xTaskCreate(writeLCD,(const char *)"writeLCD",256,NULL,0,NULL);
	xTaskCreate(dac,(const char *)"dac",256,NULL,2,NULL);
	vTaskStartScheduler();
	for (;;)
	return 0;
}

static void writeLCD(){
	for(;;){
		if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
			writeHeader(gType,gDutyCycle,gstatus,gfreq,gamplitude,goffset);
			xSemaphoreGive(xSemaphore);       // Libera o semáforo
		}
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}

static void dac(){
	for(;;){
		if(gstatus){
			if(gType == sen_t){
			for (int i = 0;i < 100;i++)
			{
				DAC_Write(buffer[i]);
				for(int j=0; j<delayMachine;j++) {
					_delay_us(1);
				}
			}
		}
		else{
			for (int i = 0;i < 100;i++)
			{
				DAC_Write(buffer[i]);
				for(int j=0; j<delayMachine;j++) {
					_delay_us(1);
				}
			}
			
		}
		vTaskDelay(10/portTICK_PERIOD_MS);
		
	}
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
			_delay_ms(250);
		}
		
	}
	else if(~PINC&(1<<botaoM)){
		controle = controle<<1;
		if(controle>(1<<chOff)) controle=0x01;
	}else if(~PINC&(1<<botaoDW)){
		while(~PINC&(1<<botaoDW)){
			switch (controle)
			{
				case (1<<chType):
				updateVars(gType-1,gDutyCycle,gstatus,gfreq, gamplitude,goffset);
				break;
				case (1<<chDutyCycle):
				updateVars(gType,gDutyCycle-1,gstatus,gfreq, gamplitude,goffset);
				break;
				case (1<<chStatus):
				updateVars(gType,gDutyCycle,(gstatus ? 0:1),gfreq, gamplitude,goffset);
				break;
				case (1<<chFreq):
				updateVars(gType,gDutyCycle,gstatus,gfreq-1, gamplitude,goffset);
				break;
				case (1<<chAmpl):
				updateVars(gType,gDutyCycle,gstatus,gfreq, gamplitude-0.1F,goffset);
				break;
				case (1<<chOff):
				updateVars(gType,gDutyCycle,gstatus,gfreq, gamplitude,goffset-0.1F);
				break;
				default:
				break;
			}
			_delay_ms(250);
		}
	
	}
	
		delayMachine = delayfreqUs(0, gfreq);
	if(gType == quadrada_t) gen_square_wave(buffer, gamplitude, goffset, gDutyCycle);
	else if(gType == triangular_t) gen_triangular_wave(buffer, gamplitude, goffset, gDutyCycle);
	else if(gType == rampa_t) gen_rampa_wave(buffer, gamplitude, goffset);
	else if(gType == sen_t){ generate_sine_wave(buffer, gamplitude, goffset, 30, 1000);
		delayMachine = delayfreqUs(1, gfreq);
	}
	writeHeader(gType,gDutyCycle,gstatus,gfreq,gamplitude,goffset);
}