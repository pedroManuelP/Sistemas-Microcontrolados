#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

#define MPU6050_ADDR 0x68

void MPU6050_init(void);
void MPU6050_read_raw(int16_t *accel_data, int16_t *gyro_data);
void MPU6050_READ_SCALED(int16_t *accel, int16_t *gyro);
#endif
