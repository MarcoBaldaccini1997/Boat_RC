#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pico/stdlib.h"

/* ===== PINS ===== */

#define ILI9341_CS                      14
#define ILI9341_DC                      15
#define CSN_RF                          21
#define CE_RF                           20
#define IRQ_RF                          22
#define ILI9341_RST                     21
#define MOSI                            19
#define MISO                            16
#define SCK                             18
#define CONTROLLER_BATTERY_PIN          28 
#define ANALOG_SPEED_PIN                27
#define F_B_SELECT_PIN                  2
#define C_D_SELECT_PIN                  3
#define O_S_SELECT_PIN                  4
#define ENCODER_SW_PIN                  11
#define ENCODER_DT_PIN                  12
#define ENCODER_CLK_PIN                 13
#define PIEZO_PIN                       17

/* ===== COLOR PALETTE ===== */

#define ILI9341_BLACK                   0x0000
#define ILI9341_WHITE                   0xFFFF
#define ILI9341_RED                     0xf800
#define ILI9341_BLUE                    0x001f
#define ILI9341_PINK                    0xfcfb
#define ILI9341_YELLOW                  0xffe0
#define ILI9341_GREEN                   0x07e0
#define ILI9341_VIOLA                   0x5a2e
#define ILI9341_ORANGE                  0xfc80

/* ===== GENERIC MACROS ===== */

#define CHAR_WIDTH                      15
#define CHAR_HEIGHT                     21
#define INDEX_CHAR_F                    0
#define INDEX_CHAR_B                    1
#define INDEX_CHAR_C                    2
#define INDEX_CHAR_D                    3
#define INDEX_CHAR_O                    4
#define INDEX_CHAR_S                    5

#define SELECT_F_B                      0
#define SELECT_C_D                      4
#define SELECT_O_S                      8
#define SELECT_LEFT                     1
#define SELECT_RIGHT                    3
#define INDEX_F_B                       0
#define INDEX_C_D                       1
#define INDEX_O_S                       2

#define OFFSET_INDEX_LEVEL_SPEED_MOTOR  7

#define CONTROLLER_BATTERY_CHANNEL      2 //TODO check the current CHANNEL 
#define ANALOG_SPEED_CHANNEL            1 //TODO check the current CHANNEL

#define FREQ_SPI                        80 * 1000 * 1000

#define ROUND_ANGLE                     360
#define COEFF_SCALE_ANGLE               72 
#define ANGLE_RES                       5

/* ===== WIRELESS MACROS ===== */

#define CHANNEL_RF                      76 // this channel is far enough from the wifi band

// TX side

// data_valid (1 bit) open_loop_servo (1 bit), common_differential (1 bit), forward_backward (1 bit), velocity (3 bit), angle_to_do (7 bit) = 15 bit -> 16 bit

// #define EMPTY_MASK 0b0000000000000000

#define OPEN_LOOP_SERVO_MASK            0b0000000000000001
#define COMMON_DIFF_MASK                0b0000000000000001
#define DIRECTION_MASK                  0b0000000000000001
#define SPEED_MASK                      0b0000000000000111
#define ANGLE_TO_DO_MASK                0b0000000001111111

#define SPEED_SHIFT                     7
#define DIRECTION_SHIFT                 10
#define COMMON_DIFF_SHIFT               11
#define OPEN_LOOP_SERVO_SHIFT           12

#define DATA_VALID_TX                   0b1000000000000000

// RX side

/*
    data_valid (1 bit)
    motor_left_direction (1 bit), 
    motor_left _speed (3 bit), 
    motor_right_direction (1 bit), 
    motor_right_speed (3 bit), 
    battery_boat (6 bit), 
    current_angle_boat (7 bit)
    total = 22 bit -> 24 bit
*/

// #define EMPTY_MASK 0b00000000000000000000000000000000
#define MOTOR_LEFT_DIR_MASK             0b00000000000100000000000000000000
#define MOTOR_LEFT_SPEED_MASK           0b00000000000011100000000000000000
#define MOTOR_RIGHT_DIR_MASK            0b00000000000000010000000000000000
#define MOTOR_RIGHT_SPEED_MASK          0b00000000000000001110000000000000
#define BATTERY_BOAT_MASK               0b00000000000000000001111110000000
#define ANGLE_BOAT_MASK                 0b00000000000000000000000001111111

#define MOTOR_LEFT_DIR_SHIFT            18 
#define MOTOR_LEFT_SPEED_SHIFT          17 
#define MOTOR_RIGHT_DIR_SHIFT           14 
#define MOTOR_RIGHT_SPEED_SHIFT         13 
#define BATTERY_BOAT_SHIFT              7  

#define DATA_VALID_RX                   0b10000000000000000000000000000000

/* ===== SPEED RANGE MACROS ===== */

#define SPEED_7_7_MACROS                3920
#define SPEED_6_7_MACROS                3360
#define SPEED_5_7_MACROS                2800
#define SPEED_4_7_MACROS                2240
#define SPEED_3_7_MACROS                1680
#define SPEED_2_7_MACROS                1120
#define SPEED_1_7_MACROS                560
//#define SPEED_0_7_MACROS                0

/* ===== SPEED INDEX MACROS ===== */

#define SPEED_7_INDEX                   7
#define SPEED_6_INDEX                   6
#define SPEED_5_INDEX                   5
#define SPEED_4_INDEX                   4
#define SPEED_3_INDEX                   3
#define SPEED_2_INDEX                   2
#define SPEED_1_INDEX                   1
#define SPEED_0_INDEX                   0

/* ===== CONTROLLER BATTERY RANGE MACROS ===== */

#define CONTROLLER_RANGE_BATTERY_20     1680    // TODO check this range
#define CONTROLLER_RANGE_BATTERY_40     2240    // TODO check this range
#define CONTROLLER_RANGE_BATTERY_60     2800    // TODO check this range
#define CONTROLLER_RANGE_BATTERY_80     3360    // TODO check this range
#define CONTROLLER_RANGE_BATTERY_100    3920    // TODO check this range

/* ===== CONTROLLER BATTERY INDEX MACROS ===== */

#define CONTROLLER_LEVEL_BATTERY_20     3
#define CONTROLLER_LEVEL_BATTERY_40     4
#define CONTROLLER_LEVEL_BATTERY_60     5
#define CONTROLLER_LEVEL_BATTERY_80     6
#define CONTROLLER_LEVEL_BATTERY_100    7

/* ===== TIME UPDATE MACROS ===== */

#define TIME_UPDATE_CONTROLLER_BATTERY  (60 * 1000) // 1 min in millisec
#define TIME_UPDATE_TRANSMISSION        100 // 100 ms
#define TIME_CONTROLLER_BATTERY         60000 // 60000 ms = 60 s



#ifdef __cplusplus
}
#endif

#endif