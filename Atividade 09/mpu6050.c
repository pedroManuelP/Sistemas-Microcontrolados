#include "i2c.h"
#include "mpu6050.h"

#define MPU6050_PWR_MGMT_1  0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B

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

void MPU6050_read_raw(int16_t *accel_data, int16_t *gyro_data) {
    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 0);
    TWI_write(MPU6050_ACCEL_XOUT_H);
    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 1);

    for (int i = 0; i < 6; ++i) {
        uint8_t high = (i == 5) ? TWI_read_nack() : TWI_read_ack();
        uint8_t low  = TWI_read_ack();
        accel_data[i / 2] = (int16_t)((high << 8) | low);
    }

    for (int i = 0; i < 6; ++i) {
        uint8_t high = (i == 5) ? TWI_read_nack() : TWI_read_ack();
        uint8_t low  = TWI_read_ack();
        gyro_data[i / 2] = (int16_t)((high << 8) | low);
    }

    TWI_stop();
}
