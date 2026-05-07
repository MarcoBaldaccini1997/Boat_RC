#ifndef GENERIC_FUNCTIONS_H
#define GENERIC_FUNCTIONS_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

void set_servo_angle (uint16_t);

uint16_t read_battery_level ();

#ifdef __cplusplus
}
#endif

#endif