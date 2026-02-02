#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "config.h"
#include "RF24.h"

RF24 radio(CE_RF, CSN_RF);
const uint8_t address[5] = {'B','O','A','T','1'};

//#define DEBUG_EN



volatile uint8_t nrf_irq = false;

void nrf24_irq_handler(uint gpio, uint32_t events) {
    if (gpio == IRQ_RF && (events & GPIO_IRQ_EDGE_FALL)) {
        nrf_irq = true;
    }
}



int main() {

  #ifdef DEBUG_EN
    stdio_init_all();
  #endif
  sleep_ms(5000);
  spi_init(SPI_PORT, FREQ_SPI);
  gpio_set_function(SCK,  GPIO_FUNC_SPI);
  gpio_set_function(MOSI, GPIO_FUNC_SPI);
  gpio_set_function(MISO, GPIO_FUNC_SPI);
  
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

  radio.openReadingPipe(1, address);
  radio.startListening();

  radio.maskIRQ(true, true, false); // IRQ only for RX_DR

  #ifdef DEBUG_EN
    printf("TX ready...\n");
  #endif

  uint32_t data_sent = 0x000000; // i want to send 24 bits
  uint16_t data_received = 0;

  uint32_t current_time, last_time_battery_read, last_time_angle_read, last_time_motor_update;

  uint8_t angle_to_do, speed_motor, direction_motor, common_diff, open_loop_servo;

  uint8_t current_angle_boat, battery_boat, motor_right_speed, motor_right_direction, motor_left_speed, motor_left_direction;

  // start tmp

  current_angle_boat = 0;
  battery_boat = 0;
  motor_right_speed = 0;
  motor_right_direction = 0;
  motor_left_speed = 0;
  motor_left_direction = 0;

  // end tmp

  current_time = to_ms_since_boot (get_absolute_time ());

  for (;;) {

    if (data_received) {
        angle_to_do = data_received & ANGLE_TO_DO_MASK;
        speed_motor = (data_received & SPEED_MASK) >> SPEED_SHIFT;
        direction_motor = (data_received & DIRECTION_MASK) >> DIRECTION_SHIFT;
        common_diff = (data_received & COMMON_DIFF_MASK) >> COMMON_DIFF_SHIFT;
        open_loop_servo = (data_received & OPEN_LOOP_SERVO_MASK) >> OPEN_LOOP_SERVO_SHIFT;
        #ifdef DEBUG_EN
          printf("angle: %d ", angle_to_do);
          printf("speed: %d ", speed_motor);
          printf("dir: %d ", direction_motor);
          printf("c/d: %d ", common_diff);
          printf("ol/s: %d\n", open_loop_servo);
        #endif
        data_received = 0;
    }

    if ((to_ms_since_boot (get_absolute_time ()) - last_time_angle_read) >= TIME_UPDATE_ANGLE) {
      current_angle_boat = (current_angle_boat == 71) ? 0 : (current_angle_boat + 1);
      last_time_angle_read = to_ms_since_boot (get_absolute_time ());
    }

    if ((to_ms_since_boot (get_absolute_time ()) - last_time_battery_read) >= TIME_UPDATE_BATTERY) {
      battery_boat = (battery_boat == 63) ? 0 : (battery_boat + 1);
      last_time_battery_read = to_ms_since_boot (get_absolute_time ());
    }

    if ((to_ms_since_boot (get_absolute_time ()) - last_time_motor_update) >= TIME_UPDATE_MOTOR) {
      motor_left_direction = (motor_left_speed == 7) ? ((motor_left_direction == 0) ? 1 : 0) : motor_left_direction;
      motor_right_direction = (motor_left_speed == 7) ? ((motor_right_direction == 0) ? 1 : 0) : motor_right_direction;
      motor_left_speed = (motor_left_speed == 7) ? 0 : (motor_left_speed + 1);
      motor_right_speed = (motor_right_speed == 7) ? 0 : (motor_right_speed + 1);
      last_time_motor_update = to_ms_since_boot (get_absolute_time ());
    }

    data_sent =
      DATA_VALID_RX |
      ((motor_left_direction & MOTOR_LEFT_DIR_MASK) << MOTOR_LEFT_DIR_SHIFT) |
      ((motor_left_speed & MOTOR_LEFT_SPEED_MASK) << MOTOR_LEFT_SPEED_SHIFT) |
      ((motor_right_direction & MOTOR_RIGHT_DIR_MASK) << MOTOR_RIGHT_DIR_SHIFT) |
      ((motor_right_speed & MOTOR_RIGHT_SPEED_MASK) << MOTOR_RIGHT_SPEED_SHIFT) |
      ((battery_boat & BATTERY_BOAT_MASK) << BATTERY_BOAT_SHIFT) |
      (current_angle_boat & ANGLE_BOAT_MASK);

    if (nrf_irq) {

          nrf_irq = false;

          bool tx_ok, tx_fail, rx_ready; // radio.whatHappened want only bool variable...
          radio.whatHappened(tx_ok, tx_fail, rx_ready);

          if (rx_ready) { // i've received a data
              radio.read(&data_received, sizeof(data_received));
              // ACK payload (4 byte)
              radio.writeAckPayload(1, &data_sent, sizeof(data_sent));
              data_sent = (data_sent == 0xABCDEF) ? 0x012345 : 0xABCDEF;
              sleep_ms(TIME_WAIT_TO_SYNC);
          }
      }
  }
  
}


