#include "gfx.h"
#include "ili9341.h"
#include <stdbool.h>

/* ================= UTILITIES ================= */

static inline void swap_int16(int16_t *a, int16_t *b) {
    int16_t t = *a;
    *a = *b;
    *b = t;
}

/* ============== FILLED RECT ================== */

void gfx_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    for (int16_t j = y; j < y + h; j++) {
        ili9341_draw_fast_hline(x, j, w, color);
    }
}

/* ============== FILLED CIRCLE ================ */
/* Midpoint circle algorithm */

void gfx_fill_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t x = 0;
    int16_t y = r;
    int16_t d = 1 - r;

    while (y >= x) {
        // Usa fast hline invece di pixel singoli
        ili9341_draw_fast_hline(x0 - x, y0 + y, 2 * x + 1, color);
        ili9341_draw_fast_hline(x0 - x, y0 - y, 2 * x + 1, color);
        ili9341_draw_fast_hline(x0 - y, y0 + x, 2 * y + 1, color);
        ili9341_draw_fast_hline(x0 - y, y0 - x, 2 * y + 1, color);

        if (d < 0) d += 2 * x + 3;
        else { d += 2 * (x - y) + 5; y--; }
        x++;
    }
}

/* ============== FILLED TRIANGLE =============== */
/* Scanline rasterization (Adafruit-style) */

void gfx_fill_triangle(
    int16_t x0, int16_t y0,
    int16_t x1, int16_t y1,
    int16_t x2, int16_t y2,
    uint16_t color
) {
    if (y0 > y1) { swap_int16(&y0, &y1); swap_int16(&x0, &x1); }
    if (y1 > y2) { swap_int16(&y1, &y2); swap_int16(&x1, &x2); }
    if (y0 > y1) { swap_int16(&y0, &y1); swap_int16(&x0, &x1); }

    if (y0 == y2) { // tri piatto
        int16_t a = x0, b = x0;
        if (x1 < a) a = x1; else if (x1 > b) b = x1;
        if (x2 < a) a = x2; else if (x2 > b) b = x2;
        ili9341_draw_fast_hline(a, y0, b - a + 1, color);
        return;
    }

    int32_t sa = 0;
    int32_t sb = 0;
    int16_t y, last;
    if (y1 == y2) last = y1;
    else last = y1 - 1;

    for (y = y0; y <= last; y++) {
        int16_t a = x0 + sa / (y1 - y0);
        int16_t b = x0 + sb / (y2 - y0);
        sa += (x1 - x0);
        sb += (x2 - x0);
        if (a > b) swap_int16(&a, &b);
        ili9341_draw_fast_hline(a, y, b - a + 1, color);
    }

    sa = (x2 - x1) * (y - y1);
    sb = (x2 - x0) * (y - y0);
    for (; y <= y2; y++) {
        int16_t a = x1 + sa / (y2 - y1);
        int16_t b = x0 + sb / (y2 - y0);
        sa += (x2 - x1);
        sb += (x2 - x0);
        if (a > b) swap_int16(&a, &b);
        ili9341_draw_fast_hline(a, y, b - a + 1, color);
    }
}

/* ============ FILLED ROUND RECT =============== */

void gfx_fill_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    if (r > w / 2) r = w / 2;
    if (r > h / 2) r = h / 2;

    // rettangolo centrale
    gfx_fill_rect(x + r, y, w - 2 * r, h, color);

    // angoli arrotondati
    gfx_fill_circle(x + r, y + r, r, color);
    gfx_fill_circle(x + w - r - 1, y + r, r, color);
    gfx_fill_circle(x + r, y + h - r - 1, r, color);
    gfx_fill_circle(x + w - r - 1, y + h - r - 1, r, color);
}

void gfx_fill_arc(
    int16_t cx, int16_t cy,
    int16_t r,
    float a0,
    float a1,
    uint16_t color
) {
    if (a0 > a1) {
        float t = a0; a0 = a1; a1 = t;
    }

    for (int16_t y = -r; y <= r; y++) {
        int16_t yy = cy + y;
        int16_t dx = sqrtf(r * r - y * y);

        int16_t xl = cx - dx;
        int16_t xr = cx + dx;

        int16_t start = -1;

        for (int16_t x = xl; x <= xr; x++) {
            float a = atan2f(y, x - cx);
            if (a < 0) a += 2 * M_PI;

            if (a >= a0 && a <= a1) {
                if (start < 0) start = x;
            } else {
                if (start >= 0) {
                    ili9341_draw_fast_hline(start, yy, x - start, color);
                    start = -1;
                }
            }
        }

        if (start >= 0) {
            ili9341_draw_fast_hline(start, yy, xr - start + 1, color);
        }
    }
}

