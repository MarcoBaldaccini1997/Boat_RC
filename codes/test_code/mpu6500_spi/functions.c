#include "macros.h"
#include "functions.h"
#include "config.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

uint8_t mpu6500_read(uint8_t reg) {
    uint8_t tx = reg | 0x80;
    uint8_t rx;
    gpio_put(PIN_CS_MPU, 0);
    spi_write_blocking(spi0, &tx, 1);
    spi_read_blocking(spi0, 0x00, &rx, 1);
    gpio_put(PIN_CS_MPU, 1);
    return rx;
}

void mpu6500_init() {
    // wake up
    writeReg (ADDR_WAKE_UP, VALUE_WRITE_INIT);
    sleep_ms (100);

    // check WHO_AM_I
    uint8_t who = mpu6500_read (MPU_REG_WHO_AM_I);
    if (who != MPU_ID) {
        printf ("MPU6500 not found! WHO_AM_I=0x%02X\n", who);
    } else {
        printf ("MPU6500 detected!\n");
    }

    // Configure accelerometer ±2g
    writeReg (ADDR_ACCEL_SCALE, VALUE_WRITE_INIT); 

    // Configure gyro ±250 dps
    writeReg (ADDR_GYRO_SCALE, VALUE_WRITE_INIT); 
}

void writeReg (uint8_t reg, uint8_t data) { 
    uint8_t buf[2] = { reg & 0x7F, data };
    gpio_put(PIN_CS_MPU, 0);
    spi_write_blocking(spi0, buf, 2);
    gpio_put(PIN_CS_MPU, 1);
}


void readRegs (uint8_t reg, uint8_t* buf, uint8_t len) {
    uint8_t tx = reg | 0x80;
    gpio_put (PIN_CS_MPU, 0);
    spi_write_blocking (spi0, &tx, 1);
    spi_read_blocking (spi0, 0x00, buf, len);
    gpio_put (PIN_CS_MPU, 1);
}

uint16_t get_angle_rotation () {
    uint8_t buffer[BIT_TO_READ_SENSOR];
    readRegs (ADDR_SENSOR_READ, buffer, BIT_TO_READ_SENSOR);  // GZ H+L 
    int16_t ax = (buffer[0] << 8) | buffer[1];
    int16_t ay = (buffer[2] << 8) | buffer[3];
    int16_t az = (buffer[4] << 8) | buffer[5];
    //int16_t gx = (buffer[8] << 8) | buffer[9];
    //int16_t gy = (buffer[10] << 8) | buffer[11];
    int16_t gz_raw = (buffer[12] << 8) | buffer[13];    
    float gz = gz_raw * GYRO_SCALE;  // °/s 
    float gz_corrected = gz - biasZ;
    // integraton
    uint64_t now = to_ms_since_boot(get_absolute_time());
    float dt = (now - prev_time) / 1000.0;  // secondi
    prev_time = now;    
    angleZ += gz_corrected * dt;    
    float axf = ax / 16384.0;
    float ayf = ay / 16384.0;
    float azf = az / 16384.0;
    float a_total = sqrt (axf * axf + ayf * ayf + azf * azf);
    bool fermo = abs (gz) < 0.5 && abs (a_total - 1.0) < 0.05;  
    if (fermo) biasZ = (1 - alpha) * biasZ + alpha * gz; 
    int16_t angle_integer_raw = -round (angleZ);
    int16_t angle_adjust = ((angle_integer_raw % 5) >= 4) ? 1 : 0;
    angle_integer_raw = angle_integer_raw / ANGLE_RES;
    int16_t angle_integer = (angle_integer_raw + angle_adjust) * ANGLE_RES;
    angle_integer %= ROUND_ANGLE;
    uint16_t angle_integer_abs = (angle_integer < 0) ? (ROUND_ANGLE + angle_integer) : angle_integer;
    return angle_integer_abs;
}
