#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "config.h"
#include "generic_functions.h"
#include "ILI9341_function.h"
#include "RF24.h"

RF24 radio(CE_RF, CSN_RF);
const uint8_t address[5] = {'B','O','A','T','1'};

#define DEBUG_EN
//#define SHOW_ONLY_TX_FAIL // comment this define if you want only see when the transmission fail

volatile int32_t encoderCount = 0;
volatile int8_t encoderDirection = 0;
volatile uint8_t lastEncoded = 0;
volatile bool speed_direction = false;

void encoder_isr(uint gpio, uint32_t events) {
    if (speed_direction) return;

    uint8_t MSB = gpio_get(ENCODER_CLK_PIN);
    uint8_t LSB = gpio_get(ENCODER_DT_PIN);

    uint8_t encoded = (MSB << 1) | LSB;
    uint8_t sum = (lastEncoded << 2) | encoded;

    switch (sum) {
        case 0b0001:
        case 0b0111:
        case 0b1110:
        case 0b1000:
            encoderCount--;
            encoderDirection = -1;
            break;

        case 0b0010:
        case 0b1011:
        case 0b1101:
        case 0b0100:
            encoderCount++;
            encoderDirection = +1;
            break;

        default:
            encoderDirection = 0;
    }

    lastEncoded = encoded;
}

int main() {

    #ifdef DEBUG_EN
      stdio_init_all();
    #endif
    sleep_ms(2000);

    gpio_init (F_B_SELECT_PIN);
    gpio_set_dir (F_B_SELECT_PIN, GPIO_IN);
    gpio_pull_up (F_B_SELECT_PIN);
    gpio_init (C_D_SELECT_PIN);
    gpio_set_dir (C_D_SELECT_PIN, GPIO_IN);
    gpio_pull_up (C_D_SELECT_PIN);
    gpio_init (O_S_SELECT_PIN);
    gpio_set_dir (O_S_SELECT_PIN, GPIO_IN);
    gpio_pull_up (O_S_SELECT_PIN);
    gpio_init (ENCODER_SW_PIN);
    gpio_set_dir (ENCODER_SW_PIN, GPIO_IN);
    gpio_init (ENCODER_DT_PIN);
    gpio_set_dir (ENCODER_DT_PIN, GPIO_IN);
    gpio_init (ENCODER_CLK_PIN);
    gpio_set_dir (ENCODER_CLK_PIN, GPIO_IN);

    lastEncoded = (gpio_get (ENCODER_CLK_PIN) << 1) | gpio_get (ENCODER_DT_PIN);

    gpio_set_irq_enabled_with_callback (
        ENCODER_CLK_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &encoder_isr
    );

    gpio_set_irq_enabled (
        ENCODER_DT_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true
    );

    if (!radio.begin()) { // if the RF module is not connected
      #ifdef DEBUG_EN
        printf("NRF24 not found\n");
      #endif
      while (1);
    }

    #ifdef DEBUG_EN
      printf("NRF24 found!\n");
    #endif

    radio.setChannel(CHANNEL_RF);
    radio.setDataRate(RF24_250KBPS); // for RF24_PA_MAX set RF24_250KBPS, not RF24_1MBPS
    radio.setPALevel(RF24_PA_MAX); // RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX

    radio.enableAckPayload();
    radio.enableDynamicPayloads();

    radio.openWritingPipe(address);
    radio.stopListening();

    #ifdef DEBUG_EN
      printf("TX ready...\n");
    #endif

    uint16_t data_sent = 0xABCD;
    uint32_t data_received = 0x000000;

    int32_t countSnapshot, last_value_encoder = 0;

    uint32_t current_time, last_time_controller_battery_read;

    // RX side
    uint8_t current_angle_boat = 0, previous_angle_boat = 0;
    uint8_t current_battery_boat_level = 0, previous_battery_boat_level = 0;
    uint8_t current_level_speed_motor_left [2] = {0, 0}, previous_level_speed_motor_left [2] = {0, 0}; // level, direction
    uint8_t current_level_speed_motor_right [2] = {0, 0}, previous_level_speed_motor_right [2] = {0, 0}; // level, direction

    uint8_t current_speed_level, previous_speed_level, speed_direction;
    uint8_t current_angle_setpoint = 0, previous_angle_setpoint = 0;
    uint8_t current_select_value [3], previous_select_value [3]; // SELECT_F_B; SELECT_C_D; SELECT_O_S;
    uint8_t current_level_controller_battery, previous_level_controller_battery;
    uint8_t wireless_status, connection_attempts = 0;

    // start tmp

    current_speed_level = SPEED_0_INDEX;
    current_level_controller_battery = 0;
    previous_level_controller_battery = 0;

    // end tmp

    /* ===== Start set initial value controller side ===== */

    /*/
    // read the initial speed value
    previous_speed_level = 0;
    current_speed_level = read_speed_level ();
    if (previous_speed_level != current_speed_level) {
        previous_speed_level = current_speed_level;
        draw_tachimeter_level (current_speed_level, previous_speed_level, speed_direction);
    }/**/

    // read the initial setpoint value
    previous_select_value [INDEX_F_B] = SELECT_LEFT;
    current_select_value [INDEX_F_B] = gpio_get (F_B_SELECT_PIN);
    previous_select_value [INDEX_C_D] = SELECT_LEFT;
    current_select_value [INDEX_C_D] = gpio_get (C_D_SELECT_PIN);
    previous_select_value [INDEX_O_S] = SELECT_LEFT;
    current_select_value [INDEX_O_S] = gpio_get (O_S_SELECT_PIN);
    if (previous_select_value [INDEX_F_B] != current_select_value [INDEX_F_B]) {
        previous_select_value [INDEX_F_B] = current_select_value [INDEX_F_B];
        draw_select (SELECT_F_B, current_select_value [INDEX_F_B]);
    }
    if (previous_select_value [INDEX_C_D] != current_select_value [INDEX_C_D]) {
        previous_select_value [INDEX_C_D] = current_select_value [INDEX_C_D];
        draw_select (SELECT_C_D, current_select_value [INDEX_C_D]);
    }
    if (previous_select_value [INDEX_O_S] != current_select_value [INDEX_O_S]) {
        previous_select_value [INDEX_O_S] = current_select_value [INDEX_O_S];
        draw_select (SELECT_O_S, current_select_value [INDEX_O_S]);
    }

    /**
    // read the battery level
    previous_level_controller_battery = read_controller_battery_level ();
    last_time_controller_battery_read = to_ms_since_boot (get_absolute_time ());
    draw_level_controller_battery (previous_level_controller_battery);
    /**/
    /* ===== End set initial value controller side ===== */


    current_time = to_ms_since_boot (get_absolute_time ());

    for (;;) {

      // draw_setpoint = 0ms
      sleep_ms(2); // draw_tachimeter_level = 2 ms
      
      /* ===== Start read select value ===== */

      current_select_value [INDEX_F_B] = gpio_get (F_B_SELECT_PIN);
      current_select_value [INDEX_C_D] = gpio_get (C_D_SELECT_PIN);
      current_select_value [INDEX_O_S] = gpio_get (O_S_SELECT_PIN);
      if ((previous_select_value [INDEX_F_B] != current_select_value [INDEX_F_B]) && (current_speed_level == SPEED_0_INDEX)) {
          uint8_t value_select_index = (current_select_value [INDEX_F_B] == 0) ? SELECT_LEFT : SELECT_RIGHT;
          draw_select (SELECT_F_B, current_select_value [INDEX_F_B]);
          previous_select_value [INDEX_F_B] = current_select_value [INDEX_F_B];
      }
      if ((previous_select_value [INDEX_C_D] != current_select_value [INDEX_C_D]) && (current_speed_level == SPEED_0_INDEX)) {
          uint8_t value_select_index = (current_select_value [INDEX_C_D] == 0) ? SELECT_LEFT : SELECT_RIGHT;
          draw_select (SELECT_C_D, value_select_index);
          previous_select_value [INDEX_C_D] = current_select_value [INDEX_C_D];
      }
      if ((previous_select_value [INDEX_O_S] != current_select_value [INDEX_O_S]) && (current_speed_level == SPEED_0_INDEX)) {
          uint8_t value_select_index = (current_select_value [INDEX_O_S] == 0) ? SELECT_LEFT : SELECT_RIGHT;
          draw_select (SELECT_O_S, value_select_index);
          previous_select_value [INDEX_O_S] = current_select_value [INDEX_O_S];
      }

      /* ===== End read select value ===== */

      /* ===== Start read setpoint value ===== */

      if (gpio_get(ENCODER_SW_PIN) == false) { // reset encoderCount if (gpio_get(ENCODER_CLK_PIN) == false) { 
          encoderCount = 0;
          lastEncoded = 0;
      }

      countSnapshot = (encoderCount >> 2); // encoderCount / 4
      countSnapshot = ((abs (encoderCount) % 4) >= 2) ? ((encoderCount > 0) ? (countSnapshot + 1) : (countSnapshot - 1)) : countSnapshot;
      countSnapshot %= COEFF_SCALE_ANGLE;
      countSnapshot = (countSnapshot < 0) ? (COEFF_SCALE_ANGLE + countSnapshot) : countSnapshot;
      current_angle_setpoint = countSnapshot;

      if (previous_angle_setpoint != current_angle_setpoint) {
          draw_setpoint (current_angle_setpoint, previous_angle_setpoint);
          previous_angle_setpoint = current_angle_setpoint;
      }
                
      /* ===== End read setpoint value ===== */

      sleep_ms(2); // draw_level_controller_battery = 2 ms
      sleep_ms(10); // debuff time

      /**/

      data_sent = 
            DATA_VALID_TX |
            ((current_select_value [2] & OPEN_LOOP_SERVO_MASK) << OPEN_LOOP_SERVO_SHIFT) |
            ((current_select_value [1] & COMMON_DIFF_MASK) << COMMON_DIFF_SHIFT) |
            ((current_select_value [0] & DIRECTION_MASK) << DIRECTION_SHIFT) |
            ((current_speed_level & SPEED_MASK) << SPEED_SHIFT) |
            (current_angle_setpoint & ANGLE_TO_DO_MASK);

      uint8_t success_sent = radio.write(&data_sent, sizeof(data_sent));
      if (success_sent) {
          if (radio.isAckPayloadAvailable()) {
            radio.read(&data_received, sizeof(data_received));
            current_angle_boat = (data_received & ANGLE_BOAT_MASK);
            current_battery_boat_level = ((data_received & BATTERY_BOAT_MASK) >> BATTERY_BOAT_SHIFT);
            current_level_speed_motor_left [0] = ((data_received & MOTOR_LEFT_SPEED_MASK) >> MOTOR_LEFT_SPEED_SHIFT);
            current_level_speed_motor_left [1] = ((data_received & MOTOR_LEFT_DIR_MASK) >> MOTOR_LEFT_DIR_SHIFT);
            current_level_speed_motor_right [0] = ((data_received & MOTOR_RIGHT_SPEED_MASK) >> MOTOR_RIGHT_SPEED_SHIFT);
            current_level_speed_motor_right [1] = ((data_received & MOTOR_RIGHT_DIR_MASK) >> MOTOR_RIGHT_DIR_SHIFT);
            sleep_ms(1); // draw_battery_level = 1 ms
            sleep_ms(6); // draw_compass = 6 ms
            sleep_ms(2); // draw_level_speed_motor_left = 2 ms
            sleep_ms(2); // draw_level_speed_motor_left = 2 ms
            sleep_ms(10); // debuff time
          }
        } 
        else {
          #ifdef DEBUG_EN
            printf("TX FAIL (MAX_RT)\n");
          #endif
          data_received = 0xFFFF;
          radio.flush_tx();
      }

      #ifdef DEBUG_EN 
        #ifndef SHOW_ONLY_TX_FAIL
            /*/
            if (previous_angle_boat != current_angle_boat) {
                printf ("***\n");
                printf("angle: %d\n", current_angle_boat);
                printf ("***\n\n\n");
                previous_angle_boat = current_angle_boat;
            }
            /**/
            /*/
            if (previous_battery_boat_level != current_battery_boat_level) {
                printf ("***\n");
                printf("batt: %d\n", current_battery_boat_level);
                printf ("***\n\n\n");
                previous_battery_boat_level = current_battery_boat_level;
            }
            /**/
            /*/
            if (previous_level_speed_motor_left [0] != current_level_speed_motor_left [0]) {
                printf ("***\n");
                printf("speed_l: %d\n", current_level_speed_motor_left [0]);
                printf ("***\n\n\n");
                previous_level_speed_motor_left [0] = current_level_speed_motor_left [0];
            }
            /**/
            /**/
            if (previous_level_speed_motor_left [1] != current_level_speed_motor_left [1]) {
                printf ("***\n");
                printf("dir_l: %d\n", current_level_speed_motor_left [1]);
                printf ("***\n\n\n");
                previous_level_speed_motor_left [1] = current_level_speed_motor_left [1];
            }
            /**/
            /*/
            if (previous_level_speed_motor_right [0] != current_level_speed_motor_right [0]) {
                printf ("***\n");
                printf("speed_r: %d\n", current_level_speed_motor_right [0]);
                printf ("***\n\n\n");
                previous_level_speed_motor_right [0] = current_level_speed_motor_right [0];
            }
            /**/
            /**/
            if (previous_level_speed_motor_right [1] != current_level_speed_motor_right [1]) {
                printf ("***\n");
                printf("speed_r: %d\n", current_level_speed_motor_right [1]);
                printf ("***\n\n\n");
                previous_level_speed_motor_right [1] = current_level_speed_motor_right [1];
            }
            /**/
        #endif
      #endif

      while ((to_ms_since_boot (get_absolute_time ()) - current_time) < TIME_UPDATE_TRANSMISSION); // wait until 100ms left
      current_time = to_ms_since_boot (get_absolute_time ());

      /**/

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

/*
        current_angle_boat = (data_received & ANGLE_BOAT_MASK);
        current_battery_boat_level = (data_received & BATTERY_BOAT_MASK) >> BATTERY_BOAT_SHIFT;
        current_level_speed_motor_right [0] = (data_received & MOTOR_RIGHT_SPEED_MASK) >> MOTOR_RIGHT_SPEED_SHIFT;
        current_level_speed_motor_right [1] = (data_received & MOTOR_RIGHT_DIR_MASK) >> MOTOR_RIGHT_DIR_SHIFT;
        current_level_speed_motor_left [0] = (data_received & MOTOR_LEFT_SPEED_MASK) >> MOTOR_LEFT_SPEED_SHIFT;
        current_level_speed_motor_left [1] = (data_received & MOTOR_LEFT_DIR_MASK) >> MOTOR_LEFT_DIR_SHIFT;

        data_to_send = 
            ((current_select_value [2] & OPEN_LOOP_SERVO_MASK) << OPEN_LOOP_SERVO_SHIFT) |
            ((current_select_value [1] & COMMON_DIFF_MASK) << COMMON_DIFF_SHIFT) |
            ((current_select_value [0] & DIRECTION_MASK) << DIRECTION_SHIFT) |
            ((current_speed_level & SPEDD_MASK) << SPEDD_SHIFT) |
            (current_angle_setpoint & ANGLE_TO_DO_MASK);
    */
