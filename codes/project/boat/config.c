#include "config.h"

float angleZ = 0.0, biasZ = 0.0, alpha = 0.01;

const uint16_t array_angle_servo [19] = {
    TIME_US_45_DEG, 
    TIME_US_50_DEG, 
    TIME_US_55_DEG, 
    TIME_US_60_DEG, 
    TIME_US_65_DEG, 
    TIME_US_70_DEG, 
    TIME_US_75_DEG, 
    TIME_US_80_DEG, 
    TIME_US_85_DEG, 
    TIME_US_90_DEG, 
    TIME_US_95_DEG, 
    TIME_US_100_DEG,
    TIME_US_105_DEG,
    TIME_US_110_DEG,
    TIME_US_115_DEG,
    TIME_US_120_DEG,
    TIME_US_125_DEG,
    TIME_US_130_DEG,
    TIME_US_135_DEG
};

const uint16_t array_speed_value [8] = {
    SPEED_0_VALUE,
    SPEED_1_VALUE,
    SPEED_2_VALUE,
    SPEED_3_VALUE,
    SPEED_4_VALUE,
    SPEED_5_VALUE,
    SPEED_6_VALUE,
    SPEED_7_VALUE
};

uint32_t prev_time;