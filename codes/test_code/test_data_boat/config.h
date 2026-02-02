#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pico/stdlib.h"

/* ===== PINS ===== */

#define SPI_PORT   spi0

#define CSN_RF                          21
#define CE_RF                           20
#define IRQ_RF                          22
#define MOSI                            19
#define MISO                            16
#define SCK                             18

/* ===== GENERIC MACROS ===== */

#define CHANNEL_RF                      76 // this channel is far enough from the wifi band

#define FREQ_SPI                        80 * 1000 * 1000

/* ===== TIMING MACROS ===== */

#define TIME_WAIT_TO_SYNC               10
#define TIME_UPDATE_ANGLE               100 // 100ms
#define TIME_UPDATE_MOTOR               1000 // 1000ms = 1s
#define TIME_UPDATE_BATTERY             2500 //(60 * 1000) // 60 * 1000 ms = 1 min

/* ===== WIRELESS MACROS ===== */

#define CHANNEL_RF                      76 // this channel is far enough from the wifi band

// TX side

// data_valid (1 bit) open_loop_servo (1 bit), common_differential (1 bit), forward_backward (1 bit), velocity (3 bit), angle_to_do (7 bit) = 14 bit -> 16 bit

// #define EMPTY_MASK 0b0000000000000000
#define OPEN_LOOP_SERVO_MASK            0b0001000000000000
#define COMMON_DIFF_MASK                0b0000100000000000
#define DIRECTION_MASK                  0b0000010000000000
#define SPEED_MASK                      0b0000001110000000
#define ANGLE_TO_DO_MASK                0b0000000001111111

#define SPEED_SHIFT                     7
#define DIRECTION_SHIFT                 10
#define COMMON_DIFF_SHIFT               11
#define OPEN_LOOP_SERVO_SHIFT           12

// RX side

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

#define MOTOR_LEFT_DIR_SHIFT            18 
#define MOTOR_LEFT_SPEED_SHIFT          17 
#define MOTOR_RIGHT_DIR_SHIFT           14 
#define MOTOR_RIGHT_SPEED_SHIFT         13 
#define BATTERY_BOAT_SHIFT              7  

#define DATA_VALID_RX                   0b10000000000000000000000000000000


#ifdef __cplusplus
}
#endif

#endif
