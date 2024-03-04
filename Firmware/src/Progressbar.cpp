#include "Progressbar.h"

constexpr uint8_t progressbar_chars[10][8] = {{0b00000,
                                               0b00000,
                                               0b00000,
                                               0b00000,
                                               0b00000,
                                               0b00000,
                                               0b00000,
                                               0b00000},
                                              {0b10000,
                                               0b10000,
                                               0b10000,
                                               0b10000,
                                               0b10000,
                                               0b10000,
                                               0b10000,
                                               0b10000},
                                              {0b11000,
                                               0b11000,
                                               0b11000,
                                               0b11000,
                                               0b11000,
                                               0b11000,
                                               0b11000,
                                               0b11000},
                                              {0b11100,
                                               0b11100,
                                               0b11100,
                                               0b11100,
                                               0b11100,
                                               0b11100,
                                               0b11100,
                                               0b11100},
                                              {0b11110,
                                               0b11110,
                                               0b11110,
                                               0b11110,
                                               0b11110,
                                               0b11110,
                                               0b11110,
                                               0b11110},
                                              {0b11111,
                                               0b11111,
                                               0b11111,
                                               0b11111,
                                               0b11111,
                                               0b11111,
                                               0b11111,
                                               0b11111},
                                              {0b00000,   //
                                               0b10000,   // █
                                               0b11000,   // ██
                                               0b11100,   // ███
                                               0b11110,   // ████
                                               0b11100,   // ███
                                               0b11000,   // ██
                                               0b10000},  // █
                                              {0b00000,   //
                                               0b00001,   //     █
                                               0b00011,   //    ██
                                               0b00111,   //   ███
                                               0b01111,   //  ████
                                               0b00111,   //   ███
                                               0b00011,   //    ██
                                               0b00001}}; //     █

void load_progressbar_chars(LiquidCrystal::Display &lcd)
{
    for (uint8_t i = 0; i < sizeof(progressbar_chars) / sizeof(progressbar_chars[0]); i++)
    {
        lcd.create_char(i, progressbar_chars[i]);
    }
}

void draw_progressbar(LiquidCrystal::Display &lcd, LiquidCrystal::Row row, uint8_t progress, uint8_t active)
{
    uint8_t progress_chars = lcd.width() - 2;
    uint8_t scaled_progress = ((uint16_t)progress * (progress_chars * 5)) / 100;
    lcd.set_cursor(0, row);
    active ? lcd.putc(0x06) : lcd.putc(0);
    for (uint8_t i = 0; i < progress_chars; i++)
    {
        uint8_t bar_width = scaled_progress > 5 ? 5 : scaled_progress;
        lcd.putc(bar_width);
        scaled_progress -= bar_width;
    }
    active ? lcd.putc(0x07) : lcd.putc(0);
}
