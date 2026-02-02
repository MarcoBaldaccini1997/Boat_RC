#ifndef ILI9341_H
#define ILI9341_H

#include "hardware/spi.h"
#include <stdint.h>

#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320

extern uint8_t TFT_CS;
extern uint8_t TFT_DC;


void ili9341_init(spi_inst_t *spi, uint8_t cs, uint8_t dc);
void ili9341_fill_screen(uint16_t color);
void ili9341_draw_pixel(int16_t x, int16_t y, uint16_t color);
void ili9341_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color);

typedef enum {
    ILI9341_ROTATION_0 = 0,
    ILI9341_ROTATION_90,
    ILI9341_ROTATION_180,
    ILI9341_ROTATION_270
} ili9341_rotation_t;

void ili9341_set_rotation(ili9341_rotation_t r);

#endif

