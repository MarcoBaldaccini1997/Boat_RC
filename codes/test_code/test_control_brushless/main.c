// This code is a test for the control of brushless motor, typing the char showed in the printf function the motor speed will be set
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "config.h"
#include "hardware/pwm.h"

#define SERIAL_MODE

#ifdef SERIAL_MODE
  void control_motor (uint8_t*, uint8_t*, int16_t, uint8_t);

  const char string_to_print[] =
  "Press the following char to drive the brushless:\n"
  "o: speed value: 0/7\n"
  "'a': speed direction -> forward; speed value: 1/7\n"
  "'b': speed direction -> forward; speed value: 2/7\n"
  "'c': speed direction -> forward; speed value: 3/7\n"
  "'d': speed direction -> forward; speed value: 4/7\n"
  "'e': speed direction -> forward; speed value: 5/7\n"
  "'f': speed direction -> forward; speed value: 6/7\n"
  "'g': speed direction -> forward; speed value: 7/7\n"
  "'A': speed direction -> backward; speed value: 1/7\n"
  "'B': speed direction -> backward; speed value: 2/7\n"
  "'C': speed direction -> backward; speed value: 3/7\n"
  "'D': speed direction -> backward; speed value: 4/7\n"
  "'E': speed direction -> backward; speed value: 5/7\n"
  "'F': speed direction -> backward; speed value: 6/7\n"
  "'G': speed direction -> backward; speed value: 7/7\n";
#endif

int main() {

  #ifdef SERIAL_MODE
    stdio_init_all();
  #endif

  sleep_ms (5000);

  // init all pin in the config file as output pwm (speed pin) and simple output (direction pins) and init all as low

  gpio_init (MOTOR_LEFT_SPEED);
  gpio_set_function (MOTOR_LEFT_SPEED, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(MOTOR_LEFT_SPEED);
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 12500.0f); // Divider del clock (125 MHz / divider) (125 MHz / 12500 = 10 KHz)
  pwm_config_set_wrap(&config, 1000 - 1);
  pwm_init(slice_num, &config, true);
  pwm_set_gpio_level(MOTOR_LEFT_SPEED, 0); // pwm off

  //gpio_init (MOTOR_RIGHT_SPEED);
  //gpio_set_function (MOTOR_RIGHT_SPEED, GPIO_FUNC_PWM);
  //uint slice_num = pwm_gpio_to_slice_num(MOTOR_LEFT_SPEED);
  //pwm_config config = pwm_get_default_config();
  //pwm_config_set_clkdiv(&config, 125.0f); // Divider del clock (125 MHz / divider) (125 MHz / 125 = 1 MHz)
  //pwm_config_set_wrap(&config, 1000 - 1);
  //pwm_init(slice_num, &config, true);
  //pwm_set_gpio_level(MOTOR_LEFT_SPEED, 0); // pwm off
  
  gpio_init (MOTOR_LEFT_DIR);
  gpio_set_dir (MOTOR_LEFT_DIR, GPIO_OUT);
  gpio_put (MOTOR_LEFT_DIR, false);

  //gpio_init (MOTOR_RIGHT__DIR);
  //gpio_set_dir (MOTOR_RIGHT__DIR, GPIO_OUT);
  //gpio_put (MOTOR_RIGHT__DIR, false);
  #ifdef SERIAL_MODE
    printf("%s", string_to_print);
  #endif

  uint8_t current_speed = SPEED_0_7_INDEX;
  uint8_t current_direction = SPEED_DIR_FORWARD;
  gpio_put (MOTOR_LEFT_DIR, current_direction);
  sleep_ms (1000);

  for (;;) {

    #ifdef SERIAL_MODE
      char c;
      scanf(" %c", &c);
      switch (c) {
        case 'o':
          printf ("Set the speed value on 0/7\n");
          pwm_set_gpio_level(MOTOR_LEFT_SPEED, speed_range [SPEED_0_7_INDEX]); // pwm off
          current_speed = 0;
          break;
        case 'a':
          printf ("Set the speed value on 1/7 (forward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_1_7_INDEX, SPEED_DIR_FORWARD);
          break;
        case 'b':
          printf ("Set the speed value on 2/7 (forward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_2_7_INDEX, SPEED_DIR_FORWARD);
          break;
        case 'c':
          printf ("Set the speed value on 3/7 (forward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_3_7_INDEX, SPEED_DIR_FORWARD);
          break;
        case 'd':
          printf ("Set the speed value on 4/7 (forward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_4_7_INDEX, SPEED_DIR_FORWARD);
          break;
        case 'e':
          printf ("Set the speed value on 5/7 (forward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_5_7_INDEX, SPEED_DIR_FORWARD);
          break;
        case 'f':
          printf ("Set the speed value on 6/7 (forward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_6_7_INDEX, SPEED_DIR_FORWARD);
          break;
        case 'g':
          printf ("Set the speed value on 7/7 (forward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_7_7_INDEX, SPEED_DIR_FORWARD);
          break;
        case 'A':
          printf ("Set the speed value on 1/7 (backward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_1_7_INDEX, SPEED_DIR_BACKWARD);
          break;
        case 'B':
          printf ("Set the speed value on 2/7 (backward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_2_7_INDEX, SPEED_DIR_BACKWARD);
          break;
        case 'C':
          printf ("Set the speed value on 3/7 (backward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_3_7_INDEX, SPEED_DIR_BACKWARD);
          break;
        case 'D':
          printf ("Set the speed value on 4/7 (backward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_4_7_INDEX, SPEED_DIR_BACKWARD);
          break;
        case 'E':
          printf ("Set the speed value on 5/7 (backward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_5_7_INDEX, SPEED_DIR_BACKWARD);
          break;
        case 'F':
          printf ("Set the speed value on 6/7 (backward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_6_7_INDEX, SPEED_DIR_BACKWARD);
          break;
        case 'G':
          printf ("Set the speed value on 7/7 (backward mode)\n");
          control_motor (&current_speed, &current_direction, SPEED_7_7_INDEX, SPEED_DIR_BACKWARD);
          break;
        default:
          printf ("Error, the char \'%c\' is not valid\n", c);
      }
    #endif

    #ifndef SERIAL_MODE
      for (int8_t i = 0; i <= SPEED_7_7_INDEX; i++) {
        pwm_set_gpio_level(MOTOR_LEFT_SPEED, speed_range [i]);
        sleep_ms (1000);
      }
      sleep_ms (1000);
      for (int8_t i = SPEED_0_7_INDEX; i >= SPEED_0_7_INDEX; i++) {
        pwm_set_gpio_level(MOTOR_LEFT_SPEED, speed_range [i]);
        sleep_ms (1000);
      }
      current_direction = (current_direction == SPEED_DIR_FORWARD) ? SPEED_DIR_BACKWARD : SPEED_DIR_FORWARD;
      gpio_put (MOTOR_LEFT_DIR, current_direction);
      sleep_ms (1000);
    #endif

  }
  
}

#ifdef SERIAL_MODE
  void control_motor (uint8_t *prev_speed_level, uint8_t *prev_dir, int16_t speed_to_do, uint8_t dir_to_do) {
    if (speed_to_do == SPEED_0_7_INDEX) return;
    if (*prev_dir != dir_to_do) { // i want to change the direction of rotation
      for (int8_t i = *prev_speed_level; i > 0; i--) { // first stop the motor by step
        pwm_set_gpio_level(MOTOR_LEFT_SPEED, speed_range [i]);
        printf ("set_speed to: %d", i);
        sleep_ms (1000);
      }
      gpio_put (MOTOR_LEFT_DIR, dir_to_do); // set the correct direction
      sleep_ms (10);
      for (int8_t i = 0; i <= speed_to_do; i++) { // now the motor is stopped, i increase the speed by step
        pwm_set_gpio_level(MOTOR_LEFT_SPEED, speed_range [i]);
        printf ("set_speed to: %d", i);
        sleep_ms (1000);
      }
    }
    else if (*prev_speed_level != speed_to_do) { // i want to change only the speed, not the rotation
      if (speed_to_do < *prev_speed_level) { // i need to reduce the velocity
        for (int8_t i = *prev_speed_level; i >= speed_to_do; i--) {
          pwm_set_gpio_level(MOTOR_LEFT_SPEED, speed_range [i]);
          printf ("set_speed to: %d", i);
          sleep_ms (1000);
        }
      }
      else { // i need to increase the velocity
        for (int8_t i = *prev_speed_level; i <= speed_to_do; i++) {
          pwm_set_gpio_level(MOTOR_LEFT_SPEED, speed_range [i]);
          printf ("set_speed to: %d", i);
          sleep_ms (1000);
        }
      }
    }
    *prev_speed_level = speed_to_do;
    *prev_dir = dir_to_do;
  }
#endif