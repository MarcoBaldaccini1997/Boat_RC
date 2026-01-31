#ifndef MACROS_H
#define MACROS_H

// --- SPI pin ---

#define SPI_PORT                    spi0
#define PIN_MISO                    16
#define PIN_CS_MPU                  17
#define PIN_SCK                     18
#define PIN_MOSI                    19


// --- MPU6500 regs --- 
#define MPU_REG_WHO_AM_I            0x75
#define ADDR_WAKE_UP                0x6B
#define VALUE_WRITE_INIT            0x00
#define ADDR_CONFIG                 0x1A
#define ADDR_GYRO_SCALE             0x1B
#define ADDR_ACCEL_SCALE            0x1C
#define VALUE_CONFIG                0x03
#define ADDR_SENSOR_READ            0x3B
#define BIT_TO_READ_SENSOR          14
#define BIT_TRANSMISSION            8
#define VALUE_WRITEBACK_ON_READ     0x80
#define VALUE_WRITE                 0x7F
#define MPU_ID                      0x70

#define FREQ_SPI                    8000000


// --- Angle constants ---
#define ANGLE_RES                   5
#define ROUND_ANGLE                 360
#define GYRO_SCALE                  (1.0f / 131.0f)  // ±250°/s


// --- Timing constanst ---
#define TIME_ANGLE_UPDATE           250

#endif