/*
 * Project 09.c
 *
 * Created: 08/07/2025 17:05:51
 * Author : chamo
 */ 
#include "defs_principais_AVR.h"
#include "lcd.h"
#include "twi_avr.h"

#define S0 PC0
#define S1 PC1
#define S2 PC2
#define S3 PC3

#define SMPRT_DIV_addr 0x19
#define GYRO_CONFIG_addr 0x1B
#define	ACCEL_CONFIG_addr 0x1C
#define PWR_MGMT_1addr 0x6B

#define ACCEL_XOUT_H_addr 0x3B
#define ACCEL_XOUT_L_addr 0x3C
#define ACCEL_YOUT_H_addr 0x3D
#define ACCEL_YOUT_L_addr 0x3E
#define ACCEL_ZOUT_H_addr 0x3F
#define ACCEL_ZOUT_L_addr 0x40

#define TEMP_OUT_H_addr 0x41
#define TEMP_OUT_L_addr 0x42

#define GYRO_XOUT_H_addr 0x43
#define GYRO_XOUT_L_addr 0x44
#define GYRO_YOUT_H_addr 0x45
#define GYRO_YOUT_L_addr 0x46
#define GYRO_ZOUT_H_addr 0x47
#define GYRO_ZOUT_L_addr 0x48

void mde();
void numIntoString(char *str, int start_pos,uint16_t num);
void mpu_init();
uint8_t mpu_read(uint8_t addr);

uint8_t state = 0;
uint8_t updateLCD = 1;
uint16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

//Endereco I2C do MPU6050
const int MPU=0x68;

char bottom_line[17] = "                \0";
int main(void)
{
	DDRC = 0x00;
	
	PCICR = 0x02;
	PCMSK1 = 0x0E;
	sei();
	
	lcd_init();
	lcd_goto(0,0);
    while (1) 
    {
		mde();
    }
}

void mde(){
	switch(state){
		//----------ACELEROMETRO----------
		case 0:
		if(updateLCD){
			lcd_clear();
			lcd_print("x(g) y(g) z(g)  ");
			lcd_goto(1,0);
			numIntoString(bottom_line,3,AcX);
			numIntoString(bottom_line,8,AcY);
			numIntoString(bottom_line,13,AcZ);
			lcd_print(bottom_line);
			updateLCD = 0;
		}
		break;
		//----------ACELEROMETRO----------
		
		//----------GIROSCOPIO----------
		case 1:
		if(updateLCD){
			lcd_clear();
			lcd_print("x(d) y(d) z(d)  ");
			lcd_goto(1,0);
			numIntoString(bottom_line,3,GyX);
			numIntoString(bottom_line,8,GyY);
			numIntoString(bottom_line,13,GyZ);
			lcd_print(bottom_line);
			updateLCD = 0;
		}
		break;
		//----------GIROSCOPIO----------
		
		//----------TEMP_E_MODULO_DE_EULER----------
		case 2:
		if(updateLCD){
			lcd_clear();
			lcd_print("x    y    z     ");
			lcd_goto(1,0);
			/*
			numIntoString();
			numIntoString();
			numIntoString();
			*/
			lcd_print("PENDENTE........");
			updateLCD = 0;
		}
		break;
		//----------TEMP_E_MODULO_DE_EULER----------
		default:
		break;
	}
}

void numIntoString(char *str, int start_pos,uint16_t num) {
	str[start_pos-3] = ' ';
	str[start_pos-2] = ' ';
	str[start_pos-1] = ' ';
	str[start_pos] = ' ';

	if(num >= 1000){
		str[start_pos-3] = (num/1000)%10 + '0';
	}
	if(num >= 100){
		str[start_pos-2] = (num/100)%10 + '0';
	}
	if(num >= 10){
		str[start_pos-1] = (num/10)%10 + '0';
	}
	str[start_pos] = num%10 + '0';
}

void mpu_init(){

}

ISR(PCINT1_vect){
	_delay_ms(10);//debounce
	
	if(tst_bit(PINC,PC1) == 0x00) state = 0; updateLCD = 1;
	if(tst_bit(PINC,PC2) == 0x00) state = 1; updateLCD = 1;
	if(tst_bit(PINC,PC3) == 0x00) state = 2; updateLCD = 1;
}
