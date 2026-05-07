#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pico/stdlib.h"
#include <stdint.h>

//#define DEBUG_EN

/* ===== PINS ===== */

#define SPI_PORT_RF                     spi1

#define CSN_RF                          13
#define CE_RF                           15
#define IRQ_RF                          14
#define MOSI_RF                         11
#define MISO_RF                         12
#define SCK_RF                          10

#define SPI_PORT_MPU                    spi0

#define CSN_MPU                         17
#define MOSI_MPU                        7
#define MISO_MPU                        16
#define SCK_MPU                         6

#define BATTERY_PIN                     0

#define MOTOR_LEFT_DIRECTION_PIN        3
#define MOTOR_RIGHT_DIRECTION_PIN       2
#define MOTOR_LEFT_SPEED_PIN            5
#define MOTOR_RIGHT_SPEED_PIN           4

#define RUDDER_PIN                      0

/* ===== GENERIC MACROS ===== */

#define CHANNEL_RF                      76 // this channel is far enough from the wifi band

#define FREQ_SPI_RF                     40 * 1000 * 1000
#define FREQ_SPI_MPU                    40 * 1000 * 1000

#define COMMON_MODE                     0
#define DIFFERENTIAL_MODE               1
#define OPEN_LOOP_MODE                  0
#define SERVO_MODE                      1

#define SPEED_7_INDEX                   7
#define SPEED_6_INDEX                   6
#define SPEED_5_INDEX                   5
#define SPEED_4_INDEX                   4
#define SPEED_3_INDEX                   3
#define SPEED_2_INDEX                   2
#define SPEED_1_INDEX                   1
#define SPEED_0_INDEX                   0

#define SPEED_7_VALUE                   6249
#define SPEED_6_VALUE                   5729
#define SPEED_5_VALUE                   5205
#define SPEED_4_VALUE                   4686
#define SPEED_3_VALUE                   4168
#define SPEED_2_VALUE                   3643
#define SPEED_1_VALUE                   3124
#define SPEED_0_VALUE                   0.0f  

#define FORWARD_MOTOR_SPEED             0
#define BACKWARD_MOTOR_SPEED            1

#define PWM_WRAP                        6249  // 20 kHz


/* ===== TIMING MACROS ===== */

#define TIME_WAIT_TO_SYNC               10
#define TIME_UPDATE_ANGLE               100 // 100ms
#define TIME_UPDATE_MOTOR               1000 // 1000ms = 1s
#define TIME_UPDATE_BATTERY             (60 * 1000) // 60 * 1000 ms = 1 min
#define SERVO_PERIOD                    20000

/* ===== MPU6500 MACROS ===== */

#define MPU_REG_WHO_AM_I            0x75
#define ADDR_WAKE_UP                0x6B
#define VALUE_WRITE_INIT            0x00
#define ADDR_CONFIG                 0x1A
#define ADDR_GYRO_SCALE             0x1B
#define ADDR_ACCEL_SCALE            0x1C
#define VALUE_CONFIG                0x03
#define ADDR_SENSOR_READ            0x3B
#define BIT_TO_READ_SENSOR          14
#define BIT_TRANSMISSION            8
#define VALUE_WRITEBACK_ON_READ     0x80
#define VALUE_WRITE                 0x7F
#define MPU_ID                      0x70

/* ===== ANGLE CONSTANTS ===== */

#define ANGLE_RES                   5
#define ANGLE_RES_FLOAT             5.0f
#define ROUND_ANGLE                 360
#define HALF_ROUND_ANGLE            180
#define GYRO_SCALE                  (1.0f / 131.0f)  // ±250°/s

/* ===== WIRELESS MACROS ===== */

#define CHANNEL_RF                      76 // this channel is far enough from the wifi band

// RX side

// data_valid (1 bit) open_loop_servo (1 bit), common_differential (1 bit), forward_backward (1 bit), velocity (3 bit), angle_to_do (7 bit) = 14 bit -> 16 bit

#define DATA_VALID_RX                   0b1000000000000000

#define EMPTY_MASK                      0b0000000000000000
#define RESET_COMPASS_BOAT_MASK         0b0010000000000000
#define OPEN_LOOP_SERVO_MASK            0b0001000000000000
#define COMMON_DIFF_MASK                0b0000100000000000
#define DIRECTION_MASK                  0b0000010000000000
#define SPEED_MASK                      0b0000001110000000
#define ANGLE_TO_DO_MASK                0b0000000001111111

#define SPEED_SHIFT                     7
#define DIRECTION_SHIFT                 10
#define COMMON_DIFF_SHIFT               11
#define OPEN_LOOP_SERVO_SHIFT           12
#define RESET_COMPASS_BOAT_SHIFT        13

// TX side

/*
    data_valid (1 bit)
    motor_left_direction (1 bit), 
    motor_left_speed (3 bit), 
    motor_right_direction (1 bit), 
    motor_right_speed (3 bit), 
    battery_boat (6 bit), 
    current_angle_boat (7 bit)
    total = 22 bit -> 24 bit
*/

// #define EMPTY_MASK 0b00000000000000000000000000000000
#define MOTOR_LEFT_DIR_MASK             0b00000000000000000000000000000001
#define MOTOR_LEFT_SPEED_MASK           0b00000000000000000000000000000111
#define MOTOR_RIGHT_DIR_MASK            0b00000000000000000000000000000001
#define MOTOR_RIGHT_SPEED_MASK          0b00000000000000000000000000000111
#define BATTERY_BOAT_MASK               0b00000000000000000000000000111111
#define ANGLE_BOAT_MASK                 0b00000000000000000000000001111111

#define MOTOR_LEFT_DIR_SHIFT            20 
#define MOTOR_LEFT_SPEED_SHIFT          17 
#define MOTOR_RIGHT_DIR_SHIFT           16 
#define MOTOR_RIGHT_SPEED_SHIFT         13 
#define BATTERY_BOAT_SHIFT              7  

#define DATA_VALID_TX                   0b10000000000000000000000000000000

/* ===== SERVO MACROS ===== */

// this angle have an offset of 90°

#define TIME_US_45_DEG                  1250 // -45°
#define TIME_US_50_DEG                  1278 // -40°
#define TIME_US_55_DEG                  1306 // -35°
#define TIME_US_60_DEG                  1333 // -30°
#define TIME_US_65_DEG                  1361 // -25°
#define TIME_US_70_DEG                  1389 // -20°
#define TIME_US_75_DEG                  1417 // -15°
#define TIME_US_80_DEG                  1444 // -10°
#define TIME_US_85_DEG                  1472 // -5°
#define TIME_US_90_DEG                  1500 // 0°
#define TIME_US_95_DEG                  1528 // 5°
#define TIME_US_100_DEG                 1556 // 10°
#define TIME_US_105_DEG                 1583 // 15°
#define TIME_US_110_DEG                 1611 // 20°
#define TIME_US_115_DEG                 1639 // 25°
#define TIME_US_120_DEG                 1667 // 30°
#define TIME_US_125_DEG                 1694 // 35°
#define TIME_US_130_DEG                 1722 // 40°
#define TIME_US_135_DEG                 1750 // 45°

#define INDEX_RUDDER_DO_MINUS_45_DEG    0          
#define INDEX_RUDDER_DO_MINUS_40_DEG    1  
#define INDEX_RUDDER_DO_MINUS_35_DEG    2          
#define INDEX_RUDDER_DO_MINUS_30_DEG    3  
#define INDEX_RUDDER_DO_MINUS_25_DEG    4          
#define INDEX_RUDDER_DO_MINUS_20_DEG    5  
#define INDEX_RUDDER_DO_MINUS_15_DEG    6         
#define INDEX_RUDDER_DO_MINUS_10_DEG    7  
#define INDEX_RUDDER_DO_MINUS_5_DEG     8          
#define INDEX_RUDDER_DO_0_DEG           9  
#define INDEX_RUDDER_DO_PLUS_10_DEG     10          
#define INDEX_RUDDER_DO_PLUS_15_DEG     11
#define INDEX_RUDDER_DO_PLUS_20_DEG     12          
#define INDEX_RUDDER_DO_PLUS_25_DEG     13 
#define INDEX_RUDDER_DO_PLUS_30_DEG     14          
#define INDEX_RUDDER_DO_PLUS_35_DEG     15 
#define INDEX_RUDDER_DO_PLUS_40_DEG     16          
#define INDEX_RUDDER_DO_PLUS_45_DEG     17 

#define RUDDER_MAX_ANGLE_ROTATION       45
#define OFFSET_TO_INDEX_DO_0_DEG        9


extern float angleZ;
extern float biasZ;
extern float alpha;

extern const uint16_t array_angle_servo [19];

extern const uint16_t array_speed_value [8];

extern uint32_t prev_time;

#ifdef __cplusplus
}
#endif

#endif
