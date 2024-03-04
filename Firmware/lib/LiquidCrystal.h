/**
 * Driver for a 20x2 LCD, mainly copied and adapted code from
 * https://github.com/arduino-libraries/LiquidCrystal
 */

#pragma once

#include <inttypes.h>
#include "IO.h"

namespace LiquidCrystal
{
    using namespace IO;

    constexpr uint8_t LCD_CLEARDISPLAY = 0x01;
    constexpr uint8_t LCD_RETURNHOME = 0x02;
    constexpr uint8_t LCD_ENTRYMODESET = 0x04;
    constexpr uint8_t LCD_DISPLAYCONTROL = 0x08;
    constexpr uint8_t LCD_CURSORSHIFT = 0x10;
    constexpr uint8_t LCD_FUNCTIONSET = 0x20;
    constexpr uint8_t LCD_SETCGRAMADDR = 0x40;
    constexpr uint8_t LCD_SETDDRAMADDR = 0x80;

    // flags for display entry mode
    constexpr uint8_t LCD_ENTRYRIGHT = 0x00;
    constexpr uint8_t LCD_ENTRYLEFT = 0x02;
    constexpr uint8_t LCD_ENTRYSHIFTINCREMENT = 0x01;
    constexpr uint8_t LCD_ENTRYSHIFTDECREMENT = 0x00;

    // flags for display on/off control
    constexpr uint8_t LCD_DISPLAYON = 0x04;
    constexpr uint8_t LCD_DISPLAYOFF = 0x00;
    constexpr uint8_t LCD_CURSORON = 0x02;
    constexpr uint8_t LCD_CURSOROFF = 0x00;
    constexpr uint8_t LCD_BLINKON = 0x01;
    constexpr uint8_t LCD_BLINKOFF = 0x00;

    // flags for display/cursor shift
    constexpr uint8_t LCD_DISPLAYMOVE = 0x08;
    constexpr uint8_t LCD_CURSORMOVE = 0x00;
    constexpr uint8_t LCD_MOVERIGHT = 0x04;
    constexpr uint8_t LCD_MOVELEFT = 0x00;

    // flags for function set
    constexpr uint8_t LCD_4BITMODE = 0x00;
    constexpr uint8_t LCD_2LINE = 0x08;
    constexpr uint8_t LCD_5x8DOTS = 0x00;

    enum Row
    {
        First = 0x00,
        Second = 0x40
    };

    class Display
    {
    public:
        Display(Output &rs, Output &rw, Output &en, Output &d4, Output &d5, Output &d6, Output &d7)
            : pin_rs(rs), pin_rw(rw), pin_en(en), pin_d4(d4), pin_d5(d5), pin_d6(d6), pin_d7(d7) {}

        void init();

        void clear();
        void home();

        void no_display();
        void display();
        void no_blink();
        void blink();
        void no_cursor();
        void cursor();
        void scrol_display_left();
        void scroll_display_right();
        void left_to_right();
        void right_to_left();
        void autoscroll();
        void no_autoscroll();

        void create_char(uint8_t, const uint8_t[]);
        void set_cursor(uint8_t, Row);

        void putc(const uint8_t data);
        void puts(const char *s);
        void puts_p(const char *s);
        inline uint8_t width() const { return 16; }

    private:
        void send_data(uint8_t);
        void send_command(uint8_t);
        void write_4_bits(uint8_t);
        void pulseEnable();

        IO::Output &pin_rs;
        IO::Output &pin_rw;
        IO::Output &pin_en;
        IO::Output &pin_d4;
        IO::Output &pin_d5;
        IO::Output &pin_d6;
        IO::Output &pin_d7;

        uint8_t m_displaycontrol = 0b00000000;
        uint8_t m_displaymode = 0b00000000;
    };
};
