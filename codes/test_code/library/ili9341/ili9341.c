#include "ili9341.h"
#include "pico/stdlib.h"

#define PIN_CS   17
#define PIN_DC   20

static void ili9341_set_addr_window(
    uint16_t x0, uint16_t y0,
    uint16_t x1, uint16_t y1
);

static ili9341_rotation_t _rotation = ILI9341_ROTATION_0;
static uint16_t _width  = ILI9341_WIDTH;
static uint16_t _height = ILI9341_HEIGHT;

// SPI e pin
static spi_inst_t *_spi;

uint8_t TFT_CS;
uint8_t TFT_DC;

static inline void cs_select()   { gpio_put(TFT_CS, 0); }
static inline void cs_deselect() { gpio_put(TFT_CS, 1); }
static inline void dc_command()  { gpio_put(TFT_DC, 0); }
static inline void dc_data()     { gpio_put(TFT_DC, 1); }

void write_cmd(uint8_t cmd) {
    cs_select();
    dc_command();
    spi_write_blocking(_spi, &cmd, 1);
    cs_deselect();
}

void write_data(uint8_t *data, size_t len) {
    cs_select();
    dc_data();
    spi_write_blocking(_spi, data, len);
    cs_deselect();
}

void ili9341_init(spi_inst_t *spi, uint8_t cs, uint8_t dc) {
    _spi = spi;
    TFT_CS  = cs;
    TFT_DC  = dc;

    // inizializza GPIO
    gpio_init(TFT_CS);
    gpio_set_dir(TFT_CS, GPIO_OUT);
    gpio_put(TFT_CS, 1);

    gpio_init(TFT_DC);
    gpio_set_dir(TFT_DC, GPIO_OUT);


    write_cmd(0x01); // Software reset
    sleep_ms(150);

    write_cmd(0x28); // Display OFF

    uint8_t data;

    write_cmd(0x3A); // Pixel format
    data = 0x55;     // 16-bit
    write_data(&data, 1);

    write_cmd(0x36); // Memory access control
    data = 0x48;
    write_data(&data, 1);

    write_cmd(0x11); // Sleep out
    sleep_ms(120);

    write_cmd(0x29); // Display ON
    ili9341_set_rotation(ILI9341_ROTATION_90);
}

void ili9341_fill_screen(uint16_t color) {
    ili9341_set_addr_window(0, 0, _width - 1, _height - 1);

    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    cs_select();
    dc_data();

    static uint8_t buf[64 * 2];
    for (int i = 0; i < 64; i++) {
        buf[i * 2]     = hi;
        buf[i * 2 + 1] = lo;
    }

    int pixels = _width * _height;
    while (pixels > 0) {
        int chunk = (pixels > 64) ? 64 : pixels;
        spi_write_blocking(_spi, buf, chunk * 2);
        pixels -= chunk;
    }

    cs_deselect();
}
/* Funzione per impostare la finestra di scrittura sul display */
static void ili9341_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t data[4];

    // Colonne
    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;
    write_cmd(0x2A); // CASET
    write_data(data, 4);

    // Righe
    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;
    write_cmd(0x2B); // RASET
    write_data(data, 4);

    // Memory write
    write_cmd(0x2C);
}

/* ==================== ili9341_draw_pixel ==================== */
void ili9341_draw_pixel(int16_t x, int16_t y, uint16_t color) {
    if (x < 0 || y < 0 || x >= _width || y >= _height)
        return;

    ili9341_set_addr_window(x, y, x, y);

    uint8_t data[2] = { color >> 8, color & 0xFF };
    write_data(data, 2);
}

void ili9341_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color) {
    if (y < 0 || y >= _height) return;

    if (x < 0) {
        w += x;
        x = 0;
    }

    if ((x + w) > _width)
        w = _width - x;

    if (w <= 0) return;

    ili9341_set_addr_window(x, y, x + w - 1, y);

    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    cs_select();
    dc_data();

    for (int i = 0; i < w; i++) {
        uint8_t px[2] = { hi, lo };
        spi_write_blocking(_spi, px, 2);
    }

    cs_deselect();
}


void ili9341_set_rotation(ili9341_rotation_t r) {
    uint8_t madctl = 0;

    _rotation = r;

    switch (r) {
    case ILI9341_ROTATION_0:
        madctl = 0x48; // MX, BGR
        _width  = 240;
        _height = 320;
        break;

    case ILI9341_ROTATION_90:
        madctl = 0x28; // MV, BGR
        _width  = 320;
        _height = 240;
        break;

    case ILI9341_ROTATION_180:
        madctl = 0x88; // MY, BGR
        _width  = 240;
        _height = 320;
        break;

    case ILI9341_ROTATION_270:
        madctl = 0xE8; // MX, MY, MV, BGR
        _width  = 320;
        _height = 240;
        break;
    }

    write_cmd(0x36);        // MADCTL
    write_data(&madctl, 1);
}


