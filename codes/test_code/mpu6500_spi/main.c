#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "functions.h"
#include "config.h"
#include "macros.h"

int main() {
    stdio_init_all ();

    spi_init (spi0, 1000 * 1000); // 1 MHz
    gpio_set_function (PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function (PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function (PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init (PIN_CS_MPU);
    gpio_set_dir (PIN_CS_MPU, true);
    gpio_put (PIN_CS_MPU, 1);

    sleep_ms (100);

    mpu6500_init ();
    float temp;
    uint16_t my_angle = 0, prev_angle = 0;
    prev_time = to_ms_since_boot (get_absolute_time ());
    uint64_t current_time = prev_time;
    while (true) {
        uint8_t buffer[BIT_TO_READ_SENSOR];
        readRegs (ADDR_SENSOR_READ, buffer, BIT_TO_READ_SENSOR);  // GZ H+L 
        int16_t ax = (buffer[0] << 8) | buffer[1];
        int16_t ay = (buffer[2] << 8) | buffer[3];
        int16_t az = (buffer[4] << 8) | buffer[5];
        int16_t temp = (buffer[6] << 8) | buffer[7];
        //printf ("ax: %d,\t ay: %d,\t az: %d,\t temp: %d\n", ax, ay, az, temp);
        //sleep_ms (1000);
        current_time = to_ms_since_boot (get_absolute_time ());
        my_angle = ((current_time - prev_time) >= TIME_ANGLE_UPDATE) ? get_angle_rotation () : my_angle;
        prev_time = ((current_time - prev_time) >= TIME_ANGLE_UPDATE) ? current_time : prev_time;
        if (prev_angle != my_angle) {
            prev_angle = my_angle;
            printf ("angle rotation: %d\n", my_angle);
        }
    }
}
