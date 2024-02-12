#include <util/delay.h>

#include "lib/Serial.h"
#include "lib/IO.h"
#include "lib/AdvancedIO.h"
#include "lib/I2C.h"
#include "lib/LiquidCrystal.h"
#include "lib/RotaryEncoder.h"
#include "lib/PT2257.h"
#include "lib/Timer1.h"

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "src/Progressbar.h"
#include "src/ChannelSetting.h"

auto rs = IO::Output(IO::Port::PortB, 0);
auto rw = IO::Output(IO::Port::PortB, 1);
auto en = IO::Output(IO::Port::PortB, 2);
auto d4 = IO::Output(IO::Port::PortB, 3);
auto d5 = IO::Output(IO::Port::PortB, 4);
auto d6 = IO::Output(IO::Port::PortB, 5);
// TODO: should be PB6 in the real board
auto d7 = IO::Output(IO::Port::PortD, 6);
auto lcd = LiquidCrystal::Display(rs, rw, en, d4, d5, d6, d7);
const auto lcd_backlight = IO::Output(IO::Port::PortD, 7);

auto pt2257 = PT2257(I2C::HardwareMaster::the, 0x88);

// TODO: change this to PB7 in the real hardware
auto enc_a = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortD, 4, IO::PullupMode::Enable);
auto enc_b = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortD, 3, IO::PullupMode::Enable);
auto enc_button_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortD, 5, IO::PullupMode::Enable);
auto enc_button = Button(enc_button_pin);
auto encoder = RotaryEncoder(enc_a, enc_b, 2);

// TODO: change this to PD3 in the real hardware
auto mute_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortD, 2, IO::PullupMode::Enable);
auto mute_button = Button(mute_pin);

ChannelSetting channels[] = {
    {"Master", PT2257::Channel::CHANNEL_MASTER, PT2257::Channel::CHANNEL_MASTER, (uint8_t *)0},
    {"TOSLINK", PT2257::Channel::CHANNEL_1, PT2257::Channel::CHANNEL_2, (uint8_t *)1},
    {"Line 1", PT2257::Channel::CHANNEL_3, PT2257::Channel::CHANNEL_4, (uint8_t *)2},
    {"Line 2", PT2257::Channel::CHANNEL_5, PT2257::Channel::CHANNEL_6, (uint8_t *)3},
};

uint8_t current_channel = 0;
bool channel_active = false;
bool is_muted = false;

void setup()
{
    Serial::UART::the.init(9600);
    I2C::HardwareMaster::the.init();
    lcd.init();
    load_progressbar_chars(lcd);
    encoder.init();
    Timer::init();

    for (auto &channel : channels)
    {
        channel.load_saved_volume(pt2257);
    }
    channels[current_channel].render(lcd, false);
}

void loop()
{

    bool needs_rerender = false;
    int8_t encoder_delta = encoder.delta();

    if (mute_button.was_pressed())
    {
        is_muted = !is_muted;
        needs_rerender = true;
    }

    if (enc_button.was_pressed())
    {
        channel_active = !channel_active;
        needs_rerender = true;
    }

    if (!is_muted && encoder_delta != 0)
    {
        if (channel_active)
        {
            channels[current_channel].offset_volume(pt2257, encoder_delta);
        }
        else
        {
            uint8_t num_channels = sizeof(channels) / sizeof(channels[0]);
            int new_channel = current_channel + encoder_delta;
            if (new_channel < 0)
            {
                new_channel += num_channels;
            }
            current_channel = new_channel % num_channels;
        }
        needs_rerender = true;
    }

    if (needs_rerender)
    {
        if (is_muted)
        {
            pt2257.mute();
            lcd.clear();
            lcd.puts("Muted");
            return;
        }

        channels[current_channel].render(lcd, channel_active);
    }
    needs_rerender = false;
}

int main()
{
    setup();
    while (1)
    {
        loop();
    }
}
