#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "config.h"
#include "RF24.h"

RF24 radio(CE_RF, CSN_RF);
const uint8_t address[5] = {'B','O','A','T','1'};

#define DEBUG_EN
//#define SHOW_ONLY_TX_FAIL // comment this define if you want only see when the transmission fail

int main() {

    #ifdef DEBUG_EN
      stdio_init_all();
    #endif
    sleep_ms(2000);

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

    uint32_t current_time = to_ms_since_boot (get_absolute_time ());

    for (;;) {

      // draw_setpoint = 0ms
      sleep_ms(2); // draw_tachimeter_level = 2 ms
      sleep_ms(5); // draw_select = 5 ms
      sleep_ms(5); // draw_select = 5 ms
      sleep_ms(5); // draw_select = 5 ms
      sleep_ms(2); // draw_level_controller_battery = 2 ms
      sleep_ms(10); // debuff time

      uint8_t success_sent = radio.write(&data_sent, sizeof(data_sent));
      if (success_sent) {
          if (radio.isAckPayloadAvailable()) {
            radio.read(&data_received, sizeof(data_received));
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
          printf("ACK: 0x%06X\n", data_received);
        #endif
      #endif
      while ((to_ms_since_boot (get_absolute_time ()) - current_time) < TIME_UPDATE_TRANSMISSION); // wait until 100ms left
      current_time = to_ms_since_boot (get_absolute_time ());

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
