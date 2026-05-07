#ifndef GFX_H
#define GFX_H

#include <stdint.h>
#include <math.h>

void gfx_fill_rect(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    uint16_t color
);

void gfx_fill_circle(
    int16_t x0,
    int16_t y0,
    int16_t r,
    uint16_t color
);

void gfx_fill_triangle(
    int16_t x0, int16_t y0,
    int16_t x1, int16_t y1,
    int16_t x2, int16_t y2,
    uint16_t color
);

void gfx_fill_round_rect(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    int16_t r,
    uint16_t color
);

void gfx_fill_arc(
    int16_t cx, int16_t cy,
    int16_t r,
    float a0,
    float a1,
    uint16_t color
);

#endif

