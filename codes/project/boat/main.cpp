#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "config.h"
#include "generic_functions.h"
#include "MPU_6500_functions.h"
#include "RF24.h"

//#define DEBUG_EN

RF24 radio(CE_RF, CSN_RF);
const uint8_t address[5] = {'B','O','A','T','1'};


//#define DISABLE_RF

#ifndef DISABLE_RF
  volatile uint8_t nrf_irq = false;

  void nrf24_irq_handler(uint gpio, uint32_t events) {
      if (gpio == IRQ_RF && (events & GPIO_IRQ_EDGE_FALL)) {
          nrf_irq = true;
      }
  }
#endif


int main() {

  #ifdef DEBUG_EN
    stdio_init_all();
    sleep_ms(5000);
    printf ("READY...\n");
  #endif

  spi_init (SPI_PORT_MPU, FREQ_SPI_MPU);

  spi_set_format(SPI_PORT_MPU, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

  gpio_set_function (MISO_MPU, GPIO_FUNC_SPI);
  gpio_set_function (SCK_MPU, GPIO_FUNC_SPI);
  gpio_set_function (MOSI_MPU, GPIO_FUNC_SPI);

  gpio_init (CSN_MPU);
  gpio_set_dir (CSN_MPU, true);
  gpio_put (CSN_MPU, 1);

  mpu6500_init();

  
  #ifndef DISABLE_RF
    gpio_init(IRQ_RF);
    gpio_set_dir(IRQ_RF, GPIO_IN);
    gpio_pull_up(IRQ_RF);
    gpio_set_irq_enabled_with_callback(
        IRQ_RF,
        GPIO_IRQ_EDGE_FALL,
        true,
        &nrf24_irq_handler
    );
  
  
    if (!radio.begin()) { // if the RF module is not connected
      #ifdef DEBUG_EN
        printf("NRF24 not found\n");
      #endif
      while (!radio.begin()) sleep_ms (1000);
    }

    #ifdef DEBUG_EN
      printf("NRF24 found!\n");
    #endif
    
    radio.setChannel(CHANNEL_RF);
    radio.setDataRate(RF24_250KBPS); // for RF24_PA_MAX set RF24_250KBPS, not RF24_1MBPS
    radio.setPALevel(RF24_PA_HIGH); // RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX

    radio.enableAckPayload();
    radio.enableDynamicPayloads();

    radio.openReadingPipe(1, address);
    radio.startListening();

    radio.maskIRQ(true, true, false); // IRQ only for RX_DR

    #ifdef DEBUG_EN
      printf("TX ready...\n");
    #endif

  #endif

  adc_init ();    

  // pin init
  adc_gpio_init (BATTERY_PIN);

  gpio_init (MOTOR_LEFT_DIRECTION_PIN);
  gpio_set_dir (MOTOR_LEFT_DIRECTION_PIN, GPIO_OUT);
  gpio_put (MOTOR_LEFT_DIRECTION_PIN, FORWARD_MOTOR_SPEED);
  gpio_init (MOTOR_RIGHT_DIRECTION_PIN);
  gpio_set_dir (MOTOR_RIGHT_DIRECTION_PIN, GPIO_OUT);
  gpio_put (MOTOR_RIGHT_DIRECTION_PIN, FORWARD_MOTOR_SPEED);

  gpio_set_function (MOTOR_LEFT_SPEED_PIN, GPIO_FUNC_PWM);
  uint slice_motor_left = pwm_gpio_to_slice_num (MOTOR_LEFT_SPEED_PIN);
  pwm_config config_motor_left = pwm_get_default_config ();
  pwm_config_set_clkdiv (&config_motor_left, 1.0f);
  pwm_config_set_wrap (&config_motor_left, PWM_WRAP);
  pwm_init (slice_motor_left, &config_motor_left, true);
  pwm_set_gpio_level (MOTOR_LEFT_SPEED_PIN, array_speed_value [SPEED_0_INDEX]);

  gpio_set_function (MOTOR_RIGHT_SPEED_PIN, GPIO_FUNC_PWM);
  uint slice_motor_right = pwm_gpio_to_slice_num (MOTOR_RIGHT_SPEED_PIN);
  pwm_config config_motor_right = pwm_get_default_config ();
  pwm_config_set_clkdiv (&config_motor_right, 1.0f);
  pwm_config_set_wrap (&config_motor_right, PWM_WRAP);
  pwm_init (slice_motor_right, &config_motor_right, true);
  pwm_set_gpio_level (MOTOR_RIGHT_SPEED_PIN, array_speed_value [SPEED_0_INDEX]);

  gpio_set_function (RUDDER_PIN, GPIO_FUNC_PWM);
  uint slice_rudder = pwm_gpio_to_slice_num (RUDDER_PIN);
  pwm_config config_rudder = pwm_get_default_config ();
  pwm_config_set_clkdiv (&config_rudder, 125.0f); // 125 MHz / 125 = 1 MHz
  pwm_config_set_wrap (&config_rudder, SERVO_PERIOD); // period = 20 ms → 20000 us
  pwm_init (slice_rudder, &config_rudder, true);
  pwm_set_gpio_level (RUDDER_PIN, array_angle_servo [INDEX_RUDDER_DO_0_DEG]);


  uint32_t data_sent = 0x000000; // i want to send 24 bits
  uint16_t data_received = 0;

  uint32_t current_time, last_time_battery_read, last_time_angle_read, last_time_motor_update;

  uint16_t angle_to_do, angle_to_do_set = 0;
  uint16_t angle_rudder = array_angle_servo [INDEX_RUDDER_DO_0_DEG], prev_angle_rudder = array_angle_servo [INDEX_RUDDER_DO_0_DEG];
  uint8_t speed_motor, direction_motor, common_diff, open_loop_servo, reset_compass, reset_compass_set = 0;

  uint16_t current_angle_boat, current_angle_boat_index, battery_boat;
  uint16_t motor_right_speed, prev_motor_right_speed = SPEED_0_INDEX, motor_right_direction, prev_motor_right_direction = FORWARD_MOTOR_SPEED;
  uint16_t motor_left_speed, prev_motor_left_speed = SPEED_0_INDEX, motor_left_direction, prev_motor_left_direction = FORWARD_MOTOR_SPEED;

  uint16_t prev_angle_boat; // debug

  // start tmp

  current_angle_boat = 0;
  current_angle_boat_index = 0;
  battery_boat = 63;
  motor_right_speed = 0;
  motor_right_direction = 0;
  motor_left_speed = 0;
  motor_left_direction = 0;

  prev_angle_boat = 0xFFFF; // debug

  // end tmp

  current_time = to_ms_since_boot (get_absolute_time ());

  for (;;) {

    if (data_received) {
        angle_to_do = (data_received & ANGLE_TO_DO_MASK) * ANGLE_RES;
        speed_motor = (data_received & SPEED_MASK) >> SPEED_SHIFT;
        direction_motor = (data_received & DIRECTION_MASK) >> DIRECTION_SHIFT;
        common_diff = (data_received & COMMON_DIFF_MASK) >> COMMON_DIFF_SHIFT;
        open_loop_servo = (data_received & OPEN_LOOP_SERVO_MASK) >> OPEN_LOOP_SERVO_SHIFT;
        reset_compass = (data_received & RESET_COMPASS_BOAT_MASK) >> RESET_COMPASS_BOAT_SHIFT;
        reset_compass_set = (reset_compass) ? reset_compass : reset_compass_set;
        angle_to_do_set = reset_compass ? angle_to_do : angle_to_do_set;
        data_received = 0;
    }
    /**/
    if ((to_ms_since_boot (get_absolute_time ()) - last_time_angle_read) >= TIME_UPDATE_ANGLE) {
      angleZ = (reset_compass_set) ? (angle_to_do_set * ANGLE_RES_FLOAT) : angleZ;
      current_angle_boat = get_angle_rotation (reset_compass_set);
      current_angle_boat_index = current_angle_boat / ANGLE_RES;
      reset_compass_set = 0;
      angle_to_do_set = 0;
      #ifdef DEBUG_EN
        if (prev_angle_boat != current_angle_boat) {
          printf ("current_angle_boat: %d\n", current_angle_boat);
          prev_angle_boat = current_angle_boat;
        }
      #endif
      last_time_angle_read = to_ms_since_boot (get_absolute_time ());
    }

    if ((to_ms_since_boot (get_absolute_time ()) - last_time_battery_read) >= TIME_UPDATE_BATTERY) {
      //battery_boat = (battery_boat == 63) ? 0 : (battery_boat + 1);
      last_time_battery_read = to_ms_since_boot (get_absolute_time ());
    }

    if ((to_ms_since_boot (get_absolute_time ()) - last_time_motor_update) >= TIME_UPDATE_MOTOR) {
      if (common_diff == COMMON_MODE) { // common mode on, the rudder follow the angle to do, in the range from -45° to 45°
        // COMMON MODE OK, CHECK THE ROTATION ON BOAT
        if (open_loop_servo == OPEN_LOOP_MODE) { 
          // in the open loop mode the angle of the rudder follow the value obtained from the RX side
          int16_t angle_to_do_in_deg = angle_to_do; // from 0° to 355°
          if (angle_to_do_in_deg < RUDDER_MAX_ANGLE_ROTATION);
            // if the angle to do is less than the max angle that could be done by the rudder set this angle to the angle of the rudder             
          else if (angle_to_do_in_deg < HALF_ROUND_ANGLE) {
            // if the angle to do is more than RUDDER_MAX_ANGLE_ROTATION but less than HALF_ROUND_ANGLE set the angle of the rudder as RUDDER_MAX_ANGLE_ROTATION
            angle_to_do_in_deg = RUDDER_MAX_ANGLE_ROTATION;
          }
          else if (angle_to_do_in_deg < (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION)) {
            // if the angle to do is more than HALF_ROUND_ANGLE but less than (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION) set the angle of the rudder as (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION)
            angle_to_do_in_deg = - RUDDER_MAX_ANGLE_ROTATION;
          }
          else {
            // if the angle to do is more than (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION) set the angle of the rudder as (ROUND_ANGLE - angle_to_do_in_deg)
            angle_to_do_in_deg = angle_to_do_in_deg - ROUND_ANGLE;
          }
          angle_to_do_in_deg = -angle_to_do_in_deg; // the angle rotation is the opposite thant the rudder rotation
          angle_to_do_in_deg += RUDDER_MAX_ANGLE_ROTATION;
          angle_to_do_in_deg = angle_to_do_in_deg / ANGLE_RES;
          angle_rudder = array_angle_servo [angle_to_do_in_deg];
        }
        else { // servo mode, the angle of the rudder is the difference between the angle to do and the angle of the boat
          int16_t delta_angle = angle_to_do - current_angle_boat;
          uint8_t turn_left = (delta_angle < 0) ? true : false;
          delta_angle = (delta_angle < 0) ? (ROUND_ANGLE + delta_angle) : delta_angle;
          if (delta_angle < RUDDER_MAX_ANGLE_ROTATION);
            // if the angle to do is less than the max angle that could be done by the rudder set this angle to the angle of the rudder             
          else if (delta_angle < HALF_ROUND_ANGLE) {
            // if the angle to do is more than RUDDER_MAX_ANGLE_ROTATION but less than HALF_ROUND_ANGLE set the angle of the rudder as RUDDER_MAX_ANGLE_ROTATION
            delta_angle = RUDDER_MAX_ANGLE_ROTATION;
          }
          else if (delta_angle < (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION)) {
            // if the angle to do is more than HALF_ROUND_ANGLE but less than (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION) set the angle of the rudder as (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION)
            delta_angle = - RUDDER_MAX_ANGLE_ROTATION;
          }
          else {
            // if the angle to do is more than (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION) set the angle of the rudder as (ROUND_ANGLE - angle_to_do_in_deg)
            delta_angle = delta_angle - ROUND_ANGLE;
          }
          delta_angle = -delta_angle; // the angle rotation is the opposite than the rudder rotation
          delta_angle += RUDDER_MAX_ANGLE_ROTATION;
          delta_angle = delta_angle / ANGLE_RES;
          angle_rudder = array_angle_servo [delta_angle];
        }
        if ((motor_left_direction != direction_motor) || (motor_right_direction != direction_motor)) {
          // if the speed motor has changed, first stop the motor
          if (motor_left_direction != direction_motor) {
            // the left motor has a different direction than the wanted direction, first stop the motor by step
            if (motor_left_speed != SPEED_0_INDEX) // decrease the speed of the motor left
              motor_left_speed--;
            else // change the motor left direction
              motor_left_direction = direction_motor;
          }
          if (motor_right_direction != direction_motor) {
            // the motor motor has a different direction than the wanted direction, first stop the motor by step
            if (motor_right_speed != SPEED_0_INDEX) // decrease the speed of the motor right
              motor_right_speed--;
            else // change the motor motor direction
              motor_right_direction = direction_motor;
          }
        }
        else {
          // the speed direction of the two motor is the same as wanted, increase the speed if neeeded and set the angle of the rudder
          if (motor_left_speed != speed_motor) {// the speed of the left motor is different than the wanted
              motor_left_speed = (motor_left_speed < speed_motor) ? (motor_left_speed + 1) : (motor_left_speed - 1);
          }
          if (motor_right_speed != speed_motor) {// the speed of the right motor is different than the wanted
              motor_right_speed = (motor_right_speed < speed_motor) ? (motor_right_speed + 1) : (motor_right_speed - 1);
          }
          motor_left_direction = direction_motor;
          motor_right_direction = direction_motor;
        }
      }   
      else { // differential mode on, the rudder are set to do 0°, the rotation will be perform with the different speed of motors
        uint16_t tmp_motor_left_speed, tmp_motor_right_speed, tmp_motor_left_direction, tmp_motor_right_direction;
        angle_rudder = array_angle_servo [INDEX_RUDDER_DO_0_DEG];
        if (open_loop_servo == OPEN_LOOP_MODE) { // OPEN LOOP MODE ON DIFFERENTIAL MODE OK
          // in the open loop mode the speed direction of the two motors is function of the angle to do
          tmp_motor_left_speed = speed_motor;
          tmp_motor_right_speed = speed_motor;
          tmp_motor_left_direction = direction_motor;
          tmp_motor_right_direction = direction_motor;
          int8_t angle_to_do_range = (angle_to_do > RUDDER_MAX_ANGLE_ROTATION) ? (angle_to_do - ROUND_ANGLE) : angle_to_do; // convert the angle from [315°, 45°] to [-45°, 45°]
          uint16_t offset_motor_speed = (angle_to_do_range < 0) ? (-angle_to_do_range / ANGLE_RES) : (angle_to_do_range / ANGLE_RES); // from 0 to 9
          if (angle_to_do_range < 0) { // turn left, the motor speed to change is the left
            tmp_motor_left_speed = (offset_motor_speed > tmp_motor_left_speed) ? 0 : (tmp_motor_left_speed - offset_motor_speed);
          }
          else { // turn right, the motor speed to change is the right
            tmp_motor_right_speed = (offset_motor_speed > tmp_motor_right_speed) ? 0 : (tmp_motor_right_speed - offset_motor_speed);
          }
        }
        else { // in the servo mode the speed direction of the two motors is function of the difference of the angle to do and the angle of the boat
          // OPEN LOOP MODE ON COMMON MODE OK
          int16_t delta_angle = angle_to_do - current_angle_boat;
          uint8_t turn_left = (delta_angle < 0) ? true : ((delta_angle >= HALF_ROUND_ANGLE) ? true : false);
          delta_angle = (delta_angle < 0) ? (-delta_angle) : delta_angle; // [0°, 355°]
          if (delta_angle < RUDDER_MAX_ANGLE_ROTATION);
            // if the angle to do is less than the max angle that could be done by the rudder set this angle to the angle of the rudder             
          else if (delta_angle < HALF_ROUND_ANGLE) {
            // if the angle to do is more than RUDDER_MAX_ANGLE_ROTATION but less than HALF_ROUND_ANGLE set the angle of the rudder as RUDDER_MAX_ANGLE_ROTATION
            delta_angle = RUDDER_MAX_ANGLE_ROTATION;
          }
          else if (delta_angle < (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION)) {
            // if the angle to do is more than HALF_ROUND_ANGLE but less than (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION) set the angle of the rudder as (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION)
            delta_angle = - RUDDER_MAX_ANGLE_ROTATION;
          }
          else {
            // if the angle to do is more than (ROUND_ANGLE - RUDDER_MAX_ANGLE_ROTATION) set the angle of the rudder as (ROUND_ANGLE - angle_to_do_in_deg)
            delta_angle = delta_angle - ROUND_ANGLE;
          }
          // delta_angle [-45°, 45°]
          delta_angle = delta_angle / ANGLE_RES; // [-9, 9]
          if (speed_motor == 0) { // when the speed motor is 0 the two motor speed will be set a different direction and same speed to make a fast turn
            delta_angle = (delta_angle > 0) ? ((delta_angle > SPEED_7_INDEX) ? SPEED_7_INDEX : delta_angle) : ((-delta_angle > SPEED_7_INDEX) ? -SPEED_7_INDEX : delta_angle); // [-7, 7]
            uint16_t tmp_speed_motor = (delta_angle > 0) ? delta_angle : (-delta_angle);
            tmp_motor_left_speed = tmp_speed_motor;
            tmp_motor_right_speed = tmp_speed_motor;
            if (turn_left) { // turn left, the direction of the left motor is back, the right motor is forward
              tmp_motor_left_direction = BACKWARD_MOTOR_SPEED;
              tmp_motor_right_direction = FORWARD_MOTOR_SPEED;
            }
            else { // turn right, the direction of the right motor is back, the left motor is forward
              tmp_motor_left_direction = FORWARD_MOTOR_SPEED;
              tmp_motor_right_direction = BACKWARD_MOTOR_SPEED;
            }
            motor_left_direction = tmp_motor_left_direction;
            motor_right_direction = tmp_motor_right_direction;
            motor_left_speed = tmp_motor_left_speed;
            motor_right_speed = tmp_motor_right_speed;
          }
          else {
            tmp_motor_left_direction = direction_motor;
            tmp_motor_right_direction = direction_motor;
            tmp_motor_left_speed = speed_motor;
            tmp_motor_right_speed = speed_motor;
            delta_angle =  (delta_angle > 0) ? delta_angle : (-delta_angle);          
            if (turn_left) { // turn left, the speed and direction of the left motor will change
              if (delta_angle > speed_motor) { // the direction of left motor will change
                tmp_motor_left_direction = (tmp_motor_left_direction == FORWARD_MOTOR_SPEED) ? BACKWARD_MOTOR_SPEED : FORWARD_MOTOR_SPEED; 
                tmp_motor_left_speed = ((delta_angle - speed_motor) > SPEED_7_INDEX) ? SPEED_7_INDEX : (delta_angle - speed_motor);
              }
              else {
                tmp_motor_left_speed = (speed_motor - delta_angle);
              }
            }
            else { // turn right, the speed and direction of the right motor will change
              if (delta_angle > speed_motor) { // the direction of right motor will change
                tmp_motor_right_direction = (tmp_motor_right_direction == FORWARD_MOTOR_SPEED) ? BACKWARD_MOTOR_SPEED : FORWARD_MOTOR_SPEED; 
                tmp_motor_right_speed = ((delta_angle - speed_motor) > SPEED_7_INDEX) ? SPEED_7_INDEX : (delta_angle - speed_motor);
              }
              else {
                tmp_motor_right_speed = (speed_motor - delta_angle);
              }
            }
          }
        }
        if ((motor_left_direction != tmp_motor_left_direction) || (motor_right_direction != tmp_motor_right_direction)) {
          // if the speed motor has changed, first stop the motor
          if (motor_left_direction != tmp_motor_left_direction) {
            // the left motor has a different direction than the wanted direction, first stop the motor by step
            if (motor_left_speed != SPEED_0_INDEX) // decrease the speed of the motor left
              motor_left_speed--;
            else // change the motor left direction
              motor_left_direction = tmp_motor_left_direction;
          }
          if (motor_right_direction != tmp_motor_right_direction) {
            // the motor motor has a different direction than the wanted direction, first stop the motor by step
            if (motor_right_speed != SPEED_0_INDEX) // decrease the speed of the motor right
              motor_right_speed--;
            else // change the motor motor direction
              motor_right_direction = tmp_motor_right_direction;
          }
        }
        else {
          // the speed direction of the two motor is the same as wanted, increase the speed if neeeded and set the angle of the rudder
          if (motor_left_speed != tmp_motor_left_speed) {// the speed of the left motor is different than the wanted
              motor_left_speed = (motor_left_speed < tmp_motor_left_speed) ? (motor_left_speed + 1) : (motor_left_speed - 1);
          }
          if (motor_right_speed != tmp_motor_right_speed) {// the speed of the right motor is different than the wanted
              motor_right_speed = (motor_right_speed < tmp_motor_right_speed) ? (motor_right_speed + 1) : (motor_right_speed - 1);
          }
          motor_left_direction = tmp_motor_left_direction;
          motor_right_direction = tmp_motor_right_direction;
        }
      }
      if (prev_angle_rudder != angle_rudder) {
        pwm_set_gpio_level (RUDDER_PIN, angle_rudder);
        prev_angle_rudder = angle_rudder;
      }
      if (prev_motor_right_direction != motor_right_direction) {
        // the direction is the opposite as the value of the MACROS
        gpio_put (MOTOR_RIGHT_DIRECTION_PIN, !motor_right_direction);
        prev_motor_right_direction = motor_right_direction;
      }
      if (prev_motor_left_direction != motor_left_direction) {
        gpio_put (MOTOR_RIGHT_DIRECTION_PIN, !motor_left_direction);
        prev_motor_left_direction = motor_left_direction;
      }
      if (prev_motor_right_speed != motor_right_speed) {
        pwm_set_gpio_level (MOTOR_RIGHT_SPEED_PIN, array_speed_value [motor_right_speed]);
        prev_motor_right_speed = motor_right_speed;
      }
      if (prev_motor_left_speed != motor_left_speed) {
        pwm_set_gpio_level (MOTOR_LEFT_SPEED_PIN, array_speed_value [motor_left_speed]);
        prev_motor_left_speed = motor_left_speed;
      }
      last_time_motor_update = to_ms_since_boot (get_absolute_time ());
    }
    /**/
    data_sent =
      DATA_VALID_TX | 
        ((motor_left_direction & MOTOR_LEFT_DIR_MASK) << MOTOR_LEFT_DIR_SHIFT) |
        ((motor_left_speed & MOTOR_LEFT_SPEED_MASK) << MOTOR_LEFT_SPEED_SHIFT) |
        ((motor_right_direction & MOTOR_RIGHT_DIR_MASK) << MOTOR_RIGHT_DIR_SHIFT) |
        ((motor_right_speed & MOTOR_RIGHT_SPEED_MASK) << MOTOR_RIGHT_SPEED_SHIFT) |
        ((battery_boat & BATTERY_BOAT_MASK) << BATTERY_BOAT_SHIFT) |
        (current_angle_boat_index & ANGLE_BOAT_MASK);

    #ifndef DISABLE_RF
      if (nrf_irq) {

          nrf_irq = false;

          bool tx_ok, tx_fail, rx_ready; // radio.whatHappened want only bool variable...
          radio.whatHappened(tx_ok, tx_fail, rx_ready);

          if (rx_ready) { // i've received a data
              radio.read(&data_received, sizeof(data_received));
              // ACK payload (4 byte)
              radio.writeAckPayload(1, &data_sent, sizeof(data_sent));
              sleep_ms(TIME_WAIT_TO_SYNC);
          }
      }
    #endif

  }
  
}


