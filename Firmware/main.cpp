#include <util/delay.h>

#include "lib/Serial.h"
#include "lib/IO.h"
#include "lib/AdvancedIO.h"
#include "lib/I2C.h"
#include "lib/LiquidCrystal.h"
#include "lib/RotaryEncoder.h"

#include "Progressbar.h"
auto rs = IO::Output(IO::Port::PortB, 0); // RS
auto rw = IO::Output(IO::Port::PortB, 1); // RW
auto en = IO::Output(IO::Port::PortB, 2); // EN
auto d4 = IO::Output(IO::Port::PortB, 3); // D4
auto d5 = IO::Output(IO::Port::PortB, 4); // D5
auto d6 = IO::Output(IO::Port::PortB, 5); // D6
// TODO: should be PB6 in the real board
auto d7 = IO::Output(IO::Port::PortD, 6); // D7
auto lcd = LiquidCrystal::Display(rs, rw, en, d4, d5, d6, d7);
const auto lcd_backlight = IO::Output(IO::Port::PortD, 7);

// TODO: change this to PB7 in the real hardware
auto enc_a = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortD, 3, IO::PullupMode::Enable);
auto enc_b = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortD, 4, IO::PullupMode::Enable);
auto enc_button = IO::Input(IO::Port::PortD, 5, IO::PullupMode::Enable);
auto encoder = RotaryEncoder(enc_a, enc_b, enc_button);

void setup()
{
    // Serial::UART::the.init(9600);
    // I2C::HardwareMaster::the.init();
    lcd.init();
    load_progressbar_chars(lcd);
    encoder.init();

    // IO::PinChangeInterruptPort::PCIPortD.attach(IO::Input(IO::Port::PortD, 2, IO::PullupMode::Enable));
    // IO::PinChangeInterruptPort::PCIPortD.attach_callback([]()
    //                                                      { Serial::UART::the.puts("Hello World\n"); });
}
int16_t progress = 0;
void loop()
{
    // Serial::UART::the.puts("Hello World\n");
    // _delay_ms(1000);

    progress += encoder.delta();
    draw_progressbar(lcd, LiquidCrystal::Row::First, progress, true);
    // for (uint8_t progress = 0; progress < 100; progress++)
    // {
    //     draw_progressbar(lcd, LiquidCrystal::Row::First, progress, true);
    //     _delay_ms(100);
    // }

    // _delay_ms(1000);
    // lcd.setCursor(0, LiquidCrystal::Row::Second);
    // for (int thisChar = 0; thisChar < 10; thisChar++)
    // {
    //     lcd.putc(thisChar + '0');
    // }
    // _delay_ms(1000);
    // lcd.clear();
}

int main()
{
    setup();
    while (1)
    {
        loop();
    }
}
