#ifndef CONFIG_H
#define CONFIG_H

#include "pico/stdlib.h"

/* ===== PINS ===== */

#define MOTOR_LEFT_SPEED                12
#define MOTOR_RIGHT_SPEED               11
#define MOTOR_LEFT_DIR                  10
#define MOTOR_RIGHT__DIR                9

/* ===== SPEED MACROS ===== */

#define PWM_WRAP 1000
#define SPEED_0_7_INDEX                 0
#define SPEED_1_7_INDEX                 8
#define SPEED_2_7_INDEX                 9
#define SPEED_3_7_INDEX                 11
#define SPEED_4_7_INDEX                 12
#define SPEED_5_7_INDEX                 13
#define SPEED_6_7_INDEX                 15
#define SPEED_7_7_INDEX                 16
#define SPEED_DIR_FORWARD               true
#define SPEED_DIR_BACKWARD              false

uint16_t speed_range [] = {
    ((SPEED_0_7_INDEX * PWM_WRAP) / 16),
    ((SPEED_1_7_INDEX * PWM_WRAP) / 16),
    ((SPEED_2_7_INDEX * PWM_WRAP) / 16),
    ((SPEED_3_7_INDEX * PWM_WRAP) / 16),
    ((SPEED_4_7_INDEX * PWM_WRAP) / 16),
    ((SPEED_5_7_INDEX * PWM_WRAP) / 16),
    ((SPEED_6_7_INDEX * PWM_WRAP) / 16),
    ((SPEED_7_7_INDEX * PWM_WRAP) / 16)
};

#endif
