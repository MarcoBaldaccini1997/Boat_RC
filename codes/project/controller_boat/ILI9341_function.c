#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "ili9341.h"
#include "gfx.h"
#include "config.h"
#include "ILI9341_function.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void init_gui (spi_inst_t* spi, uint32_t freq, uint sck, uint mosi, uint cs, uint dc) {

    spi_init (spi, freq);
    gpio_set_function (sck, GPIO_FUNC_SPI); // SCK
    gpio_set_function (mosi, GPIO_FUNC_SPI); // MOSI

    ili9341_init (spi, cs, dc);

    ili9341_fill_screen (ILI9341_WHITE);

    sleep_ms (500);

    /* ===== START COMPASS DRAW ===== */
    gfx_fill_circle (compass_position [0], compass_position [1], compass_radius [0], compass_color [0]);
    gfx_fill_circle (compass_position [0], compass_position [1], compass_radius [1], compass_color [1]);
    
    gfx_fill_triangle (
        tringle_compass_position [0][0], 
        tringle_compass_position [0][1], 
        tringle_compass_position [0][2], 
        tringle_compass_position [0][3], 
        tringle_compass_position [0][6], 
        tringle_compass_position [0][7], 
        triangle_compass_color [1]
    );

    gfx_fill_triangle (
        tringle_compass_position [0][0], 
        tringle_compass_position [0][1], 
        tringle_compass_position [0][2], 
        tringle_compass_position [0][3], 
        tringle_compass_position [0][4], 
        tringle_compass_position [0][5], 
        triangle_compass_color [0]
    );
    /* ===== END COMPASS DRAW ===== */
    
    /* ===== START SETPOINT DRAW ===== */
    gfx_fill_circle (setpoint_position [0][0], setpoint_position [0][1], setpoint_radius, setpoint_color);
    /* ===== END SETPOINT DRAW ===== */

    /* ===== START BATTERY LEVEL DRAW ===== */
    gfx_fill_arc (battery_info [0][0], battery_info [0][1], battery_info [0][2], battery_info_radius [0][0], battery_info_radius [0][1], arc_color [0]);
    gfx_fill_arc (battery_info [2][0], battery_info [2][1], battery_info [2][2], battery_info_radius [2][0], battery_info_radius [2][1], arc_color [2]);
    gfx_fill_arc (battery_info [3][0], battery_info [3][1], battery_info [3][2], battery_info_radius [3][0], battery_info_radius [3][1], arc_color [3]);
    gfx_fill_arc (battery_info [4][0], battery_info [4][1], battery_info [4][2], battery_info_radius [4][0], battery_info_radius [4][1], arc_color [4]);
    gfx_fill_arc (battery_info [1][0], battery_info [1][1], battery_info [1][2], battery_info_radius [1][0], battery_info_radius [1][1], arc_color [1]);
    gfx_fill_circle (battery_info [5][0], battery_info [5][1], battery_info [5][2], needle_color [0]);
    gfx_fill_triangle (
        needle_battery_status [0][0], 
        needle_battery_status [0][1], 
        needle_battery_status [0][2], 
        needle_battery_status [0][3], 
        needle_battery_status [0][4], 
        needle_battery_status [0][5], 
        needle_color [0]
    );
    /* ===== END BATTERY LEVEL DRAW ===== */

    /* ===== START SPEED LEVEL DRAW ===== */
    gfx_fill_arc (tachimeter_info [0][0], tachimeter_info [0][1], tachimeter_info [0][2], (M_PI), (2 * M_PI), arc_color [0]);
    gfx_fill_arc (tachimeter_info [1][0], tachimeter_info [1][1], tachimeter_info [1][2], (M_PI), (2 * M_PI), arc_color [1]);
    gfx_fill_circle (tachimeter_info [2][0], tachimeter_info [2][1], tachimeter_info [2][2], needle_color [0]);
    gfx_fill_triangle (
        needle_speed_status [0][0], 
        needle_speed_status [0][1], 
        needle_speed_status [0][2], 
        needle_speed_status [0][3], 
        needle_speed_status [0][4], 
        needle_speed_status [0][5], 
        needle_color [0]
    );
    /* ===== END SPEED LEVEL DRAW ===== */

    /* ===== START MOTOR SPEED LEFT DRAW ===== */
    gfx_fill_rect (dimension_speed_motor [0][0], dimension_speed_motor [0][1], dimension_speed_motor [0][2], dimension_speed_motor [0][3], dimension_speed_motor [0][4]);
    gfx_fill_rect (dimension_speed_motor [1][0], dimension_speed_motor [1][1], dimension_speed_motor [1][2], dimension_speed_motor [1][3], dimension_speed_motor [1][4]);
    /* ===== END MOTOR SPEED LEFT DRAW ===== */

    /* ===== START MOTOR SPEED RIGHT DRAW ===== */
    gfx_fill_rect (dimension_speed_motor [2][0], dimension_speed_motor [2][1], dimension_speed_motor [2][2], dimension_speed_motor [2][3], dimension_speed_motor [2][4]);
    gfx_fill_rect (dimension_speed_motor [3][0], dimension_speed_motor [3][1], dimension_speed_motor [3][2], dimension_speed_motor [3][3], dimension_speed_motor [3][4]);
    /* ===== END MOTOR SPEED RIGHT DRAW ===== */

    /* ===== START CHAR DRAW ===== */
    for (uint16_t row = 0; row < CHAR_HEIGHT; row++) {
        for (uint16_t col = 0; col < CHAR_WIDTH; col++) {
            ili9341_draw_pixel ((col + char_position [INDEX_CHAR_F][1]), (row + char_position [INDEX_CHAR_F][0]), FONT_F [row][col]); // draw F char
            ili9341_draw_pixel ((col + char_position [INDEX_CHAR_B][1]), (row + char_position [INDEX_CHAR_B][0]), FONT_B [row][col]); // draw B char
            ili9341_draw_pixel ((col + char_position [INDEX_CHAR_C][1]), (row + char_position [INDEX_CHAR_C][0]), FONT_C [row][col]); // draw C char
            ili9341_draw_pixel ((col + char_position [INDEX_CHAR_D][1]), (row + char_position [INDEX_CHAR_D][0]), FONT_D [row][col]); // draw D char
            ili9341_draw_pixel ((col + char_position [INDEX_CHAR_O][1]), (row + char_position [INDEX_CHAR_O][0]), FONT_O [row][col]); // draw O char
            ili9341_draw_pixel ((col + char_position [INDEX_CHAR_S][1]), (row + char_position [INDEX_CHAR_S][0]), FONT_S [row][col]); // draw S char
        }
    }
    /* ===== END CHAR DRAW ===== */

    /* ===== START SELECT DRAW ===== */
    gfx_fill_circle (
        select_circle_position [0][0], 
        select_circle_position [0][1], 
        select_circle_position [0][2], 
        color_select [1]
    );
    gfx_fill_circle (
        select_circle_position [2][0], 
        select_circle_position [2][1], 
        select_circle_position [2][2], 
        color_select [1]
    );
    gfx_fill_rect (select_rect_position [0][0], select_rect_position [0][1], select_rect_position [0][2], select_rect_position [0][3], color_select [1]);
    gfx_fill_circle (
        (select_circle_position [1][0]), 
        (select_circle_position [1][1]), 
        select_circle_position [1][2], 
        color_select [0]
    );
    gfx_fill_circle (
        select_circle_position [4][0], 
        select_circle_position [4][1], 
        select_circle_position [4][2], 
        color_select [1]
    );
    gfx_fill_circle (
        select_circle_position [6][0], 
        select_circle_position [6][1], 
        select_circle_position [6][2], 
        color_select [1]
    );
    gfx_fill_rect (select_rect_position [1][0], select_rect_position [1][1], select_rect_position [1][2], select_rect_position [1][3], color_select [1]);
    gfx_fill_circle (
        (select_circle_position [5][0]), 
        (select_circle_position [5][1]), 
        select_circle_position [5][2], 
        color_select [0]
    );
    gfx_fill_circle (
        select_circle_position [8][0], 
        select_circle_position [8][1], 
        select_circle_position [8][2], 
        color_select [1]
    );
    gfx_fill_circle (
        select_circle_position [10][0], 
        select_circle_position [10][1], 
        select_circle_position [10][2], 
        color_select [1]
    );
    gfx_fill_rect (select_rect_position [2][0], select_rect_position [2][1], select_rect_position [2][2], select_rect_position [2][3], color_select [1]);
    gfx_fill_circle (
        (select_circle_position [9][0]), 
        (select_circle_position [9][1]), 
        select_circle_position [9][2], 
        color_select [0]
    );
    /* ===== END SELECT DRAW ===== */

    /* ===== START CONTROLLER BATTERY DRAW ===== */
    gfx_fill_rect (controller_battery [0][0], controller_battery [0][1], controller_battery [0][2], controller_battery [0][3], color_controller_battery [0]);
    gfx_fill_rect (controller_battery [1][0], controller_battery [1][1], controller_battery [1][2], controller_battery [1][3], color_controller_battery [0]);
    gfx_fill_rect (controller_battery [2][0], controller_battery [2][1], controller_battery [2][2], controller_battery [2][3], color_controller_battery [1]);
    gfx_fill_rect (controller_battery [3][0], controller_battery [3][1], controller_battery [3][2], controller_battery [3][3], color_controller_battery [0]);
    gfx_fill_rect (controller_battery [4][0], controller_battery [4][1], controller_battery [4][2], controller_battery [4][3], color_controller_battery [0]);
    gfx_fill_rect (controller_battery [5][0], controller_battery [5][1], controller_battery [5][2], controller_battery [5][3], color_controller_battery [0]);
    gfx_fill_rect (controller_battery [6][0], controller_battery [6][1], controller_battery [6][2], controller_battery [6][3], color_controller_battery [0]);
    gfx_fill_rect (controller_battery [7][0], controller_battery [7][1], controller_battery [7][2], controller_battery [7][3], color_controller_battery [0]);
    /* ===== END CONTROLLER BATTERY DRAW ===== */

    /* ===== START WIRELESS LOGO DRAW ===== */
    gfx_fill_arc (wireless_logo [0][0], wireless_logo [0][1], wireless_logo [0][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]);
    gfx_fill_arc (wireless_logo [1][0], wireless_logo [1][1], wireless_logo [1][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]); // signal
    gfx_fill_arc (wireless_logo [2][0], wireless_logo [2][1], wireless_logo [2][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]);
    gfx_fill_arc (wireless_logo [3][0], wireless_logo [3][1], wireless_logo [3][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [0]);
    gfx_fill_arc (wireless_logo [4][0], wireless_logo [4][1], wireless_logo [4][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]);
    gfx_fill_arc (wireless_logo [5][0], wireless_logo [5][1], wireless_logo [5][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]); // signal
    gfx_fill_arc (wireless_logo [6][0], wireless_logo [6][1], wireless_logo [6][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]);
    gfx_fill_arc (wireless_logo [7][0], wireless_logo [7][1], wireless_logo [7][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [0]);
    gfx_fill_arc (wireless_logo [8][0], wireless_logo [8][1], wireless_logo [8][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]);
    gfx_fill_arc (wireless_logo [9][0], wireless_logo [9][1], wireless_logo [9][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]); // signal
    /* ===== END WIRELESS LOGO DRAW ===== */
}

void draw_tachimeter_level (uint8_t new_level, uint8_t old_level, uint8_t speed_direction) { // 2 ms
    // speed_direction = 0 if forward, 1 if backward
    uint16_t needle_color_select = needle_color [speed_direction]; 
    gfx_fill_circle (tachimeter_info [2][0], tachimeter_info [2][1], tachimeter_info [2][2], ILI9341_WHITE);
    gfx_fill_triangle (
        needle_speed_status [old_level][0], 
        needle_speed_status [old_level][1], 
        needle_speed_status [old_level][2], 
        needle_speed_status [old_level][3], 
        needle_speed_status [old_level][4], 
        needle_speed_status [old_level][5], 
        ILI9341_WHITE
    );
    gfx_fill_circle (tachimeter_info [2][0], tachimeter_info [2][1], tachimeter_info [2][2], needle_color_select);
    gfx_fill_triangle (
        needle_speed_status [new_level][0], 
        needle_speed_status [new_level][1], 
        needle_speed_status [new_level][2], 
        needle_speed_status [new_level][3], 
        needle_speed_status [new_level][4], 
        needle_speed_status [new_level][5], 
        needle_color_select
    );
}

void draw_battery_level (uint8_t new_level, uint8_t old_level) { // 1 ms
    gfx_fill_circle (battery_info [5][0], battery_info [5][1], battery_info [5][2], ILI9341_WHITE);
    gfx_fill_triangle (
        needle_battery_status [old_level][0], 
        needle_battery_status [old_level][1], 
        needle_battery_status [old_level][2], 
        needle_battery_status [old_level][3], 
        needle_battery_status [old_level][4], 
        needle_battery_status [old_level][5], 
        ILI9341_WHITE
    );
    gfx_fill_circle (battery_info [5][0], battery_info [5][1], battery_info [5][2], needle_color [0]);
    gfx_fill_triangle (
        needle_battery_status [new_level][0], 
        needle_battery_status [new_level][1], 
        needle_battery_status [new_level][2], 
        needle_battery_status [new_level][3], 
        needle_battery_status [new_level][4], 
        needle_battery_status [new_level][5], 
        needle_color [0]
    );
}

void draw_compass (uint8_t new_angle, uint8_t old_angle) { // 6 ms
    gfx_fill_triangle (
        tringle_compass_position [old_angle][0], 
        tringle_compass_position [old_angle][1], 
        tringle_compass_position [old_angle][2], 
        tringle_compass_position [old_angle][3], 
        tringle_compass_position [old_angle][6], 
        tringle_compass_position [old_angle][7], 
        ILI9341_WHITE
    );

    gfx_fill_triangle (
        tringle_compass_position [old_angle][0], 
        tringle_compass_position [old_angle][1], 
        tringle_compass_position [old_angle][2], 
        tringle_compass_position [old_angle][3], 
        tringle_compass_position [old_angle][4], 
        tringle_compass_position [old_angle][5], 
        ILI9341_WHITE
    );
    gfx_fill_triangle (
        tringle_compass_position [new_angle][0], 
        tringle_compass_position [new_angle][1], 
        tringle_compass_position [new_angle][2], 
        tringle_compass_position [new_angle][3], 
        tringle_compass_position [new_angle][6], 
        tringle_compass_position [new_angle][7], 
        triangle_compass_color [1]
    );

    gfx_fill_triangle (
        tringle_compass_position [new_angle][0], 
        tringle_compass_position [new_angle][1], 
        tringle_compass_position [new_angle][2], 
        tringle_compass_position [new_angle][3], 
        tringle_compass_position [new_angle][4], 
        tringle_compass_position [new_angle][5], 
        triangle_compass_color [0]
    );
    
}

void draw_setpoint (uint8_t new_angle, uint8_t old_angle, uint8_t reset_compass) { // 0 ms
    uint16_t color_selected = (reset_compass == false) ? setpoint_color : setpoint_color_reset;
    gfx_fill_circle (setpoint_position [old_angle][0], setpoint_position [old_angle][1], setpoint_radius, ILI9341_WHITE);
    gfx_fill_circle (setpoint_position [new_angle][0], setpoint_position [new_angle][1], setpoint_radius, color_selected);
}

void draw_select (uint8_t select_choosen, uint8_t select_value) { // 5 ms
    // select_choosen = 0 SELECT_F_B; 4 SELECT_C_D; 8 SELECT_O_S; 
    // select_value = 1 left; 3 right
    uint16_t color_select_choosen = (select_value == SELECT_LEFT) ? 1 : 2;
    uint8_t select_rect = (select_choosen == SELECT_F_B) ? 0 : ((select_choosen == SELECT_C_D) ? 1 : 2);
    gfx_fill_circle (
        select_circle_position [select_choosen][0], 
        select_circle_position [select_choosen][1], 
        select_circle_position [select_choosen][2], 
        color_select [color_select_choosen]
    );
    gfx_fill_circle (
        select_circle_position [select_choosen + 2][0], 
        select_circle_position [select_choosen + 2][1], 
        select_circle_position [select_choosen + 2][2], 
        color_select [color_select_choosen]
    );
    gfx_fill_rect (
        select_rect_position [select_rect][0], 
        select_rect_position [select_rect][1], 
        select_rect_position [select_rect][2], 
        select_rect_position [select_rect][3], 
        color_select [color_select_choosen]
    );
    gfx_fill_circle (
        (select_circle_position [(select_choosen + select_value)][0]), 
        (select_circle_position [(select_choosen + select_value)][1]), 
        select_circle_position [(select_choosen + select_value)][2], 
        color_select [0]
    );
}

void draw_level_speed_motor_left (uint8_t new_level, uint8_t speed_direction) { // 2 ms
    // speed_direction = 0 if forward, 1 if backward
    uint8_t level_index = (new_level == 0) ? new_level : ((speed_direction == 0) ? (new_level) : (new_level + OFFSET_INDEX_LEVEL_SPEED_MOTOR));
    gfx_fill_rect (dimension_speed_motor [1][0], dimension_speed_motor [1][1], dimension_speed_motor [1][2], dimension_speed_motor [1][3], dimension_speed_motor [1][4]);
    gfx_fill_rect (
        level_speed_motor_left [level_index][0], 
        level_speed_motor_left [level_index][1], 
        level_speed_motor_left [level_index][2], 
        level_speed_motor_left [level_index][3], 
        needle_color [speed_direction]
    );
}

void draw_level_speed_motor_right (uint8_t new_level, uint8_t speed_direction) { // 2 ms
    // speed_direction = 0 if forward, 1 if backward
    uint8_t level_index = (new_level == 0) ? new_level : ((speed_direction == 0) ? (new_level) : (new_level + OFFSET_INDEX_LEVEL_SPEED_MOTOR));
    gfx_fill_rect (dimension_speed_motor [3][0], dimension_speed_motor [3][1], dimension_speed_motor [3][2], dimension_speed_motor [3][3], dimension_speed_motor [3][4]);
    gfx_fill_rect (
        level_speed_motor_right [level_index][0], 
        level_speed_motor_right [level_index][1], 
        level_speed_motor_right [level_index][2], 
        level_speed_motor_right [level_index][3], 
        needle_color [speed_direction]
    );
}

void draw_level_controller_battery (uint8_t current_level_controller_battery) { // 2 ms
    gfx_fill_rect (controller_battery [2][0], controller_battery [2][1], controller_battery [2][2], controller_battery [2][3], color_controller_battery [1]); // clear
    for (uint8_t i = CONTROLLER_LEVEL_BATTERY_20; i <= current_level_controller_battery; i++)
        gfx_fill_rect (controller_battery [i][0], controller_battery [i][1], controller_battery [i][2], controller_battery [i][3], color_controller_battery [0]);
}

void draw_wireless_logo (uint8_t wireless_available) { // 165 ms
    // wireless_available = 1 available, 0 not available
    gfx_fill_arc (wireless_logo [0][0], wireless_logo [0][1], wireless_logo [0][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]);
    gfx_fill_arc (wireless_logo [1][0], wireless_logo [1][1], wireless_logo [1][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [wireless_available]); // signal
    gfx_fill_arc (wireless_logo [2][0], wireless_logo [2][1], wireless_logo [2][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]);
    gfx_fill_arc (wireless_logo [3][0], wireless_logo [3][1], wireless_logo [3][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [0]);
    gfx_fill_arc (wireless_logo [4][0], wireless_logo [4][1], wireless_logo [4][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]);
    gfx_fill_arc (wireless_logo [5][0], wireless_logo [5][1], wireless_logo [5][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [wireless_available]); // signal
    gfx_fill_arc (wireless_logo [6][0], wireless_logo [6][1], wireless_logo [6][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]);
    gfx_fill_arc (wireless_logo [7][0], wireless_logo [7][1], wireless_logo [7][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [0]);
    gfx_fill_arc (wireless_logo [8][0], wireless_logo [8][1], wireless_logo [8][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [1]);
    gfx_fill_arc (wireless_logo [9][0], wireless_logo [9][1], wireless_logo [9][2], wireless_logo_angle [0], wireless_logo_angle [1], color_wireless_logo [wireless_available]); // signal
} 

void demo () {
    for (uint8_t i = 1; i < 8; i++) {
        draw_tachimeter_level (i, (i - 1), 0);
        sleep_ms (50);
    }
    sleep_ms (500);
    draw_tachimeter_level (7, 7, 1);
    sleep_ms (500);
    for (int8_t i = 6; i >= 0; i--) {
        draw_tachimeter_level (i, (i + 1), 1);
        sleep_ms (50);
    }
    sleep_ms (500);
    draw_tachimeter_level (0, 0, 0);
    sleep_ms (500);
    for (uint8_t i = 1; i < 64; i++) {
        draw_battery_level (i, (i - 1));
        sleep_ms (25);
    }
    sleep_ms (500);
    for (int8_t i = 62; i >= 0; i--) {
        draw_battery_level (i, (i + 1));
        sleep_ms (25);
    }
    sleep_ms (500);
    for (uint8_t i = 1; i < 72; i++) {
        uint8_t j = (i == 1) ? 73 : i;
        draw_compass (i, (i - 1));
        draw_setpoint ((72 - i), (72 + 1 - j), false);
        sleep_ms (25);
    }
    draw_compass (0, 71);
    draw_setpoint (0, 1, false);
    sleep_ms (500);
    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < 2; j++) {
            for (uint8_t k = 0; k < 2; k++) {
                uint8_t select_fb_value = (i) ? SELECT_LEFT : SELECT_RIGHT;
                uint8_t select_cd_value = (j) ? SELECT_LEFT : SELECT_RIGHT;
                uint8_t select_os_value = (k) ? SELECT_LEFT : SELECT_RIGHT;
                draw_select (SELECT_F_B, select_fb_value);
                sleep_ms (100);
                draw_select (SELECT_C_D, select_cd_value);
                sleep_ms (100);
                draw_select (SELECT_O_S, select_os_value);
                sleep_ms (100);
            }
        }
    }
    sleep_ms (500);
    for (uint8_t i = 1; i < 5; i++) {
        draw_level_speed_motor_left (i, 0);
        draw_level_speed_motor_right (i, 0);
        sleep_ms (100);
    }
    for (uint8_t i = 5; i < 8; i++) {
        draw_level_speed_motor_left ((8 - i), 0);
        draw_level_speed_motor_right (i, 0);
        sleep_ms (100);
    }
    draw_level_speed_motor_left (0, 0);
    sleep_ms (100);
    for (uint8_t i = 1; i < 8; i++) {
        draw_level_speed_motor_left (i, 1);
        draw_level_speed_motor_right ((8 - i - 1), 0);
        sleep_ms (100);
    }
    for (uint8_t i = 1; i < 8; i++) {
        draw_level_speed_motor_right (i, 1);
        sleep_ms (100);
    }
    for (uint8_t i = 7; i > 0; i--) {
        draw_level_speed_motor_left (i, 1);
        draw_level_speed_motor_right (i, 1);
        sleep_ms (100);
    }
    draw_level_speed_motor_left (0, 0);
    draw_level_speed_motor_right (0, 0);
    sleep_ms (500);
    for (uint8_t i = CONTROLLER_LEVEL_BATTERY_100; i >= CONTROLLER_LEVEL_BATTERY_20; i--) {
        draw_level_controller_battery (i);
        sleep_ms (100);
    }
    for (uint8_t i = CONTROLLER_LEVEL_BATTERY_20; i <= CONTROLLER_LEVEL_BATTERY_100; i++) {
        draw_level_controller_battery (i);
        sleep_ms (100);
    }
    sleep_ms (500);
    for (uint8_t i = 0; i < 3; i++) {
        draw_wireless_logo (0);
        sleep_ms (500);
        draw_wireless_logo (1);
        sleep_ms (500);
    }
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