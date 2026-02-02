#ifndef ILI9341_FUNCTIONS_H
#define ILI9341_FUNCTIONS_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

void init_gui ();
void draw_tachimeter_level (uint8_t, uint8_t, uint8_t); 
void draw_battery_level (uint8_t, uint8_t);
void draw_compass (uint8_t, uint8_t);
void draw_setpoint (uint8_t, uint8_t);
void draw_select (uint8_t, uint8_t);
void draw_level_speed_motor_left (uint8_t, uint8_t); 
void draw_level_speed_motor_right (uint8_t, uint8_t); 
void draw_level_controller_battery (uint8_t);
void draw_wireless_logo (uint8_t);
void demo ();

#ifdef __cplusplus
}
#endif

#endif