#include "pico/stdlib.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "hardware/adc.h"

uint16_t read_speed_level () {
    adc_select_input (ANALOG_SPEED_CHANNEL);
    sleep_ms (1);  
    uint16_t analog_speed_raw = adc_read ();
    sleep_ms (1);
    if (analog_speed_raw <= SPEED_1_7_MACROS)
        return SPEED_0_INDEX;
    else if (analog_speed_raw <= SPEED_2_7_MACROS)
        return SPEED_1_INDEX;
    else if (analog_speed_raw <= SPEED_3_7_MACROS)
        return SPEED_2_INDEX;
    else if (analog_speed_raw <= SPEED_4_7_MACROS)
        return SPEED_3_INDEX;
    else if (analog_speed_raw <= SPEED_5_7_MACROS)
        return SPEED_4_INDEX;
    else if (analog_speed_raw <= SPEED_6_7_MACROS)
        return SPEED_5_INDEX;
    else if (analog_speed_raw <= SPEED_7_7_MACROS)
        return SPEED_6_INDEX;
    else
        return SPEED_7_INDEX;
}

uint16_t read_controller_battery_level () {
    adc_select_input (CONTROLLER_BATTERY_CHANNEL);
    sleep_ms (1);  
    uint16_t controller_battery_raw = adc_read ();
    sleep_ms (1);
    if (controller_battery_raw <= CONTROLLER_RANGE_BATTERY_20) // 20%
        return CONTROLLER_LEVEL_BATTERY_20;
    else if (controller_battery_raw <= CONTROLLER_RANGE_BATTERY_40) // 40%
        return CONTROLLER_LEVEL_BATTERY_40;
    else if (controller_battery_raw <= CONTROLLER_RANGE_BATTERY_60) // 60%
        return CONTROLLER_LEVEL_BATTERY_60;
    else if (controller_battery_raw <= CONTROLLER_RANGE_BATTERY_80) // 80%
        return CONTROLLER_LEVEL_BATTERY_80;
    else  // 100%
        return CONTROLLER_LEVEL_BATTERY_100;
}
