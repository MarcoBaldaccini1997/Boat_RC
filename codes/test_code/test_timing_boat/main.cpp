#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "config.h"
#include "RF24.h"

RF24 radio(CE_RF, CSN_RF);
const uint8_t address[5] = {'B','O','A','T','1'};

#define DEBUG_EN



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

  uint32_t data_sent = 0xABCDEF; // i want to send 24 bits
  uint16_t data_received = 0;
  uint32_t current_time = to_ms_since_boot (get_absolute_time ());

  for (;;) {

    #ifdef DEBUG_EN
      if (data_received) {
          printf("ACK: 0x%04X\n", data_received);
          data_received = 0;
      }
      else sleep_ms (10);
    #endif

    #ifndef DEBUG_EN
      sleep_ms (10);
    #endif

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
