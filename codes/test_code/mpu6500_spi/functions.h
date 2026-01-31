#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>

uint8_t mpu6500_read(uint8_t);
void mpu6500_init();
void writeReg (uint8_t, uint8_t);
void readRegs (uint8_t, uint8_t*, uint8_t);
uint16_t get_angle_rotation ();

#endif