/**
 * Driver for a 20x2 LCD, mainly copied and adapted code from
 * https://github.com/arduino-libraries/LiquidCrystal
 */

#pragma once

#include <inttypes.h>
#include "IO.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x8DOTS 0x00

namespace LiquidCrystal
{
    using namespace IO;
    enum Row
    {
        First = 0x00,
        Second = 0x40
    };

    class Display
    {
    public:
        Display(Output rs, Output rw, Output en, Output d4, Output d5, Output d6, Output d7)
            : pin_rs(rs), pin_rw(rw), pin_en(en), pin_d4(d4), pin_d5(d5), pin_d6(d6), pin_d7(d7) {}

        void begin();

        void clear();
        void home();

        void noDisplay();
        void display();
        void noBlink();
        void blink();
        void noCursor();
        void cursor();
        void scrollDisplayLeft();
        void scrollDisplayRight();
        void leftToRight();
        void rightToLeft();
        void autoscroll();
        void noAutoscroll();

        void createChar(uint8_t, uint8_t[]);
        void setCursor(uint8_t, Row);
        void send_command(uint8_t);

    private:
        void send_data(uint8_t);
        void write4bits(uint8_t);
        void pulseEnable();

        IO::Output pin_rs;
        IO::Output pin_rw;
        IO::Output pin_en;
        IO::Output pin_d4;
        IO::Output pin_d5;
        IO::Output pin_d6;
        IO::Output pin_d7;

        uint8_t m_displayfunction;
        uint8_t m_displaycontrol;
        uint8_t m_displaymode;
    };
};
