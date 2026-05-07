#include "pico/stdlib.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "hardware/adc.h"

void set_servo_angle (uint16_t angle) {
    
}

uint16_t read_battery_level () {
    //adc_select_input (CONTROLLER_BATTERY_CHANNEL);  
    //uint16_t controller_battery_raw = adc_read ();
    //if (controller_battery_raw <= CONTROLLER_RANGE_BATTERY_20) // 20%
    //    return CONTROLLER_LEVEL_BATTERY_20;
    //else if (controller_battery_raw <= CONTROLLER_RANGE_BATTERY_40) // 40%
    //    return CONTROLLER_LEVEL_BATTERY_40;
    //else if (controller_battery_raw <= CONTROLLER_RANGE_BATTERY_60) // 60%
    //    return CONTROLLER_LEVEL_BATTERY_60;
    //else if (controller_battery_raw <= CONTROLLER_RANGE_BATTERY_80) // 80%
    //    return CONTROLLER_LEVEL_BATTERY_80;
    //else  // 100%
        return 63;
}