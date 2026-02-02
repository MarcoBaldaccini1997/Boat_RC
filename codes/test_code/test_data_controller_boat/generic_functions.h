#ifndef GENERIC_FUNCTIONS_H
#define GENERIC_FUNCTIONS_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

uint16_t read_speed_level ();
uint16_t read_controller_battery_level ();

#ifdef __cplusplus
}
#endif

#endif