#ifndef MPU_6500_FUNCTIONS_H
#define MPU_6500_FUNCTIONS_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

uint8_t mpu6500_read(uint8_t);
void mpu6500_init();
void writeReg (uint8_t, uint8_t);
void readRegs (uint8_t, uint8_t*, uint8_t);
uint16_t get_angle_rotation (uint8_t);

#ifdef __cplusplus
}
#endif

#endif