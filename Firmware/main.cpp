#include "lib/Serial.h"
#include "lib/IO.h"
#include "lib/I2C.h"
#include "lib/LiquidCrystal.h"
#include "lib/RotaryEncoder.h"
#include <util/delay.h>

const auto lcd = LiquidCrystal::Display(
    IO::Output(IO::Port::PortB, 0), // RS
    IO::Output(IO::Port::PortB, 1), // RW
    IO::Output(IO::Port::PortB, 2), // EN
    IO::Output(IO::Port::PortB, 3), // D4
    IO::Output(IO::Port::PortB, 4), // D5
    IO::Output(IO::Port::PortB, 5), // D6
    IO::Output(IO::Port::PortB, 6)  // D7
);
const auto lcd_backlight = IO::Output(IO::Port::PortD, 7);

const auto encoder = RotaryEncoder(
    IO::Input(IO::Port::PortB, 7, IO::PullupMode::Enable), // Encoder Pin A
    IO::Input(IO::Port::PortD, 1, IO::PullupMode::Enable), // Encoder Pin B
    IO::Input(IO::Port::PortD, 2, IO::PullupMode::Enable)  // Button
);
const auto led = IO::Output(IO::Port::PortB, 5);
void setup()
{
    Serial::UART::the.init(9600);
    I2C::HardwareMaster::the.init();
}

void loop()
{
    led.set();
    Serial::UART::the.puts("Hello World\n");
    _delay_ms(1000);
    led.clear();
    _delay_ms(1000);
}

int main()
{
    setup();
    while (1)
    {
        loop();
    }
}
