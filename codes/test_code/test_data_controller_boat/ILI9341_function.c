#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "ili9341.h"
#include "gfx.h"
#include "config.h"
#include "ILI9341_function.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void init_gui () {
    sleep_ms (200);
}

void draw_tachimeter_level (uint8_t new_level, uint8_t old_level, uint8_t speed_direction) { // 2 ms
    sleep_ms (2);
}

void draw_battery_level (uint8_t new_level, uint8_t old_level) { // 1 ms
    sleep_ms (1);
}

void draw_compass (uint8_t new_angle, uint8_t old_angle) { // 6 ms
    sleep_ms (1);    
}

void draw_setpoint (uint8_t new_angle, uint8_t old_angle) { // 0 ms
    sleep_ms (1);
}

void draw_select (uint8_t select_choosen, uint8_t select_value) { // 5 ms
    sleep_ms (5);
}

void draw_level_speed_motor_left (uint8_t new_level, uint8_t speed_direction) { // 2 ms
    sleep_ms (2);
}

void draw_level_speed_motor_right (uint8_t new_level, uint8_t speed_direction) { // 2 ms
    sleep_ms (2);
}

void draw_level_controller_battery (uint8_t current_level_controller_battery) { // 2 ms
    sleep_ms (2);
}

void draw_wireless_logo (uint8_t wireless_available) { // 165 ms
    sleep_ms (165);
} 

void demo () {
    sleep_ms (2000);
}

/*
draw_tachimeter_level = 2 ms
draw_setpoint = 0 ms
draw_select = 4 ms
draw_select = 5 ms
draw_select = 4 ms
draw_level_controller_battery = 2 ms
draw_wireless_logo = 165 ms (non used in sum of delay)
draw_battery_level = 1 ms
draw_compass = 6 ms
draw_level_speed_motor_left = 2 ms
draw_level_speed_motor_right = 2 ms

total (without draw_wireless_logo) ~ 30 ms
*/