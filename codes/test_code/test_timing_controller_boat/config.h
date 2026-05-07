#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pico/stdlib.h"

/* ===== PINS ===== */

#define CSN_RF                          13
#define CE_RF                           14
//#define IRQ_RF                          22
#define MOSI                            15
#define MISO                            12
#define SCK                             10

/* ===== GENERIC MACROS ===== */

#define CHANNEL_RF                      76 // this channel is far enough from the wifi band

#define SPI_PORT                        spi1

#define FREQ_SPI                        80 * 1000 * 1000

#define TIME_UPDATE_TRANSMISSION        100 // 100 ms


#ifdef __cplusplus
}
#endif

#endif