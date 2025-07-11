#include "i2c.h"
#include "mpu6050.h"

#define MPU6050_PWR_MGMT_1  0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B
#define INT16_TO_G_SCALE 16384
#define INT16_TO_DEGREE_SCALE 131
static uint8_t MPU_read_register(uint8_t reg) {
    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 0);
    TWI_write(reg);
    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 1);
    uint8_t data = TWI_read_nack();
    TWI_stop();
    return data;
}

static void MPU_write_register(uint8_t reg, uint8_t value) {
    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 0);
    TWI_write(reg);
    TWI_write(value);
    TWI_stop();
}

void MPU6050_init(void) {
    TWI_init();
    _delay_ms(100);
    MPU_write_register(MPU6050_PWR_MGMT_1, 0x00); // Wake up
    _delay_ms(100);
}

void MPU6050_read_raw(int16_t *accel_data, int16_t *gyro_data, float* temp) {
    uint8_t raw[14];

    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 0);             // Write mode
    TWI_write(MPU6050_ACCEL_XOUT_H);                // Start from ACCEL_XOUT_H
    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 1);             // Read mode

    for (uint8_t i = 0; i < 13; i++) {
        raw[i] = TWI_read_ack();
    }
    raw[13] = TWI_read_nack();                      // Last byte, issue NACK
    TWI_stop();

    // Combine high/low bytes into 16-bit integers
    accel_data[0] = (int16_t)(raw[0] << 8 | raw[1]);
    accel_data[1] = (int16_t)(raw[2] << 8 | raw[3]);
    accel_data[2] = (int16_t)(raw[4] << 8 | raw[5]);
    int16_t temp_raw = (int16_t)(raw[6] << 8 | raw[7]);
    *temp =  (temp_raw / 340.0f) + 36.53f;
    gyro_data[0] = (int16_t)(raw[8] << 8 | raw[9]);
    gyro_data[1] = (int16_t)(raw[10] << 8 | raw[11]);
    gyro_data[2] = (int16_t)(raw[12] << 8 | raw[13]);
}

void MPU6050_READ_SCALED(int16_t *accel, int16_t *gyro, float *temperatura, int16_t* euler){
    MPU6050_read_raw(accel, gyro, temperatura);
    for(uint8_t i=0; i<3; i++){
        accel[i] = accel[i]/INT16_TO_G_SCALE;
    }
    for(uint8_t i=0; i<3; i++){
        gyro[i] = gyro[i]/INT16_TO_DEGREE_SCALE;
    }
    for(uint8_t i=0; i<3; i++){
        euler[i] += gyro[i];
        if(euler[i] > 360 || euler[i] < -360) euler[i] = 0;
    }

}