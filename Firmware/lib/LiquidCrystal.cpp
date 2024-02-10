#include "LiquidCrystal.h"

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

void LiquidCrystal::Display::init()
{
    uint8_t displayfunction = LCD_4BITMODE | LCD_5x8DOTS | LCD_2LINE;

    _delay_ms(50);
    pin_rs.low();
    pin_en.low();
    pin_rw.low();
    // HD44780 datasheet p. 46
    // enable 4-bit mode
    write_4_bits(0x03);
    _delay_us(4500);
    write_4_bits(0x03);
    _delay_us(4500);
    write_4_bits(0x03);
    _delay_us(150);
    write_4_bits(0x02);

    send_command(LCD_FUNCTIONSET | displayfunction);

    m_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    clear();

    m_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    send_command(LCD_ENTRYMODESET | m_displaymode);
}

void LiquidCrystal::Display::clear()
{
    send_command(LCD_CLEARDISPLAY); // clear display, set cursor position to zero
    _delay_us(2000);                // this command takes a long time!
}

void LiquidCrystal::Display::home()
{
    send_command(LCD_RETURNHOME); // set cursor position to zero
    _delay_us(2000);              // this command takes a long time!
}

void LiquidCrystal::Display::setCursor(uint8_t col, LiquidCrystal::Row row)
{
    send_command(LCD_SETDDRAMADDR | (col + row));
}

void LiquidCrystal::Display::noDisplay()
{
    m_displaycontrol &= ~LCD_DISPLAYON;
    send_command(LCD_DISPLAYCONTROL | m_displaycontrol);
}

void LiquidCrystal::Display::display()
{
    m_displaycontrol |= LCD_DISPLAYON;
    send_command(LCD_DISPLAYCONTROL | m_displaycontrol);
}

void LiquidCrystal::Display::noCursor()
{
    m_displaycontrol &= ~LCD_CURSORON;
    send_command(LCD_DISPLAYCONTROL | m_displaycontrol);
}

void LiquidCrystal::Display::cursor()
{
    m_displaycontrol |= LCD_CURSORON;
    send_command(LCD_DISPLAYCONTROL | m_displaycontrol);
}

void LiquidCrystal::Display::noBlink()
{
    m_displaycontrol &= ~LCD_BLINKON;
    send_command(LCD_DISPLAYCONTROL | m_displaycontrol);
}

void LiquidCrystal::Display::blink()
{
    m_displaycontrol |= LCD_BLINKON;
    send_command(LCD_DISPLAYCONTROL | m_displaycontrol);
}

void LiquidCrystal::Display::scrollDisplayLeft(void)
{
    send_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LiquidCrystal::Display::scrollDisplayRight(void)
{
    send_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LiquidCrystal::Display::leftToRight(void)
{
    m_displaymode |= LCD_ENTRYLEFT;
    send_command(LCD_ENTRYMODESET | m_displaymode);
}

void LiquidCrystal::Display::rightToLeft(void)
{
    m_displaymode &= ~LCD_ENTRYLEFT;
    send_command(LCD_ENTRYMODESET | m_displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal::Display::autoscroll(void)
{
    m_displaymode |= LCD_ENTRYSHIFTINCREMENT;
    send_command(LCD_ENTRYMODESET | m_displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal::Display::noAutoscroll(void)
{
    m_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    send_command(LCD_ENTRYMODESET | m_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal::Display::createChar(uint8_t location, uint8_t charmap[])
{
    location &= 0x7; // we only have 8 locations 0-7
    send_command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++)
    {
        send_data(charmap[i]);
    }
}

void LiquidCrystal::Display::putc(const uint8_t data)
{
    send_data(data);
}

void LiquidCrystal::Display::puts(const char *s)
{
    while (*s)
    {
        putc(*s++);
    }
}

void LiquidCrystal::Display::puts_p(const char *s)
{
    while (pgm_read_byte(s))
    {
        putc(pgm_read_byte(s++));
    }
}

inline void LiquidCrystal::Display::send_command(uint8_t value)
{
    pin_rs.low();
    pin_rw.low();

    write_4_bits(value >> 4);
    write_4_bits(value);
}

void LiquidCrystal::Display::send_data(uint8_t value)
{
    pin_rs.high();
    pin_rw.low();

    write_4_bits(value >> 4);
    write_4_bits(value);
}

void LiquidCrystal::Display::pulseEnable(void)
{
    pin_en.low();
    _delay_us(1);
    pin_en.high();
    _delay_us(1);
    pin_en.low();
    _delay_us(100);
}

void LiquidCrystal::Display::write_4_bits(uint8_t value)
{
    pin_d4.write((value >> 0) & 1);
    pin_d5.write((value >> 1) & 1);
    pin_d6.write((value >> 2) & 1);
    pin_d7.write((value >> 3) & 1);

    pulseEnable();
}
