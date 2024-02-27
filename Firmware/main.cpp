#ifndef BOARD_DEFINITION
#error "BOARD_DEFINITION not defined"
#else
#include BOARD_DEFINITION
#endif

#include "lib/Serial.h"
#include "lib/IO.h"
#include "lib/AdvancedIO.h"
#include "lib/I2C.h"
#include "lib/LiquidCrystal.h"
#include "lib/RotaryEncoder.h"
#include "lib/PT2257.h"
#include "lib/Timer.h"

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "src/Progressbar.h"
#include "src/ChannelSetting.h"

auto lcd = LiquidCrystal::Display(rs, rw, en, d4, d5, d6, d7);
auto pt2257 = PT2257(I2C::HardwareMaster::the, 0x88);
auto enc_button = Button(enc_button_pin);
auto encoder = RotaryEncoder(enc_a, enc_b, 2);
auto mute_button = Button(mute_pin);

ChannelSetting channels[] = {
    {"Master", PT2257::Channel::CHANNEL_MASTER, PT2257::Channel::CHANNEL_MASTER, (uint8_t *)0},
    {"TOSLINK", PT2257::Channel::CHANNEL_1, PT2257::Channel::CHANNEL_2, (uint8_t *)1},
    {"Line 1", PT2257::Channel::CHANNEL_3, PT2257::Channel::CHANNEL_4, (uint8_t *)2},
    {"Line 2", PT2257::Channel::CHANNEL_5, PT2257::Channel::CHANNEL_6, (uint8_t *)3},
};

// todo: change to 120s
constexpr uint16_t standby_delay_seconds = 30;
constexpr uint16_t lightsout_delay_seconds = 5;

uint8_t current_channel = 0;
bool channel_active = false;
bool is_muted = false;
volatile uint16_t seconds_until_standby = standby_delay_seconds;
volatile uint16_t seconds_until_lightsout = lightsout_delay_seconds;
volatile bool needs_rerender = false;

enum class DisplayState
{
    POWERON,
    STANDBY,
    LIGHTSOUT
};

enum class AmpState
{
    POWERON,
    FAULT,
    CLIP,
    MUTED,
    POWERDOWN
};

volatile DisplayState display_state = DisplayState::LIGHTSOUT;
volatile AmpState amp_state = AmpState::POWERDOWN;

void reset_timeout_timers()
{
    seconds_until_standby = standby_delay_seconds;
    seconds_until_lightsout = lightsout_delay_seconds;
}

void set_amp_state(AmpState new_state)
{
    switch (new_state)
    {
    case AmpState::POWERON:
        amp_power.high();
        pt2257.unmute();
        break;
    case AmpState::MUTED:
        amp_power.high();
        pt2257.mute();
        break;
    case AmpState::POWERDOWN:
        amp_power.low();
        pt2257.mute();
        break;
    case AmpState::CLIP:
        amp_power.low();
        pt2257.mute();
        break;
    case AmpState::FAULT:
        amp_power.low();
        pt2257.mute();
        break;
    }
    amp_state = new_state;
}

void set_display_state(DisplayState new_state)
{
    if (display_state == new_state)
    {
        return;
    }

    switch (new_state)
    {
    case DisplayState::POWERON:
        set_amp_state(AmpState::POWERON);
        lcd_backlight.high();
        ringlight_bright.high();
        ringlight_dimmed.low();
        Serial::UART::the.puts("Power On\r\n");
        needs_rerender = true;
        break;
    case DisplayState::LIGHTSOUT:
        set_amp_state(AmpState::POWERON);
        Serial::UART::the.puts("Enter lightsout\r\n");
        ringlight_dimmed.high();
        if (display_state == DisplayState::POWERON)
        {
            ringlight_bright.low();
            lcd_backlight.low();
        }
        else if (display_state == DisplayState::STANDBY)
        {
            Serial::UART::the.puts("Exiting standby\r\n");
            seconds_until_standby = standby_delay_seconds;
            Timer::start();
        }
        break;
    case DisplayState::STANDBY:
        if (amp_state != AmpState::CLIP || amp_state != AmpState::FAULT)
        {
            set_amp_state(AmpState::POWERDOWN);
        }
        Serial::UART::the.puts("Enter standby\r\n");
        ringlight_bright.low();
        ringlight_dimmed.low();
        lcd_backlight.low();

        // Timer::stop();
        // set_sleep_mode(SLEEP_MODE_PWR_DOWN);

        // don't call sleep_mode() here, it will be called in the loop so we can be
        // sure to not enter sleep mode while we're rendering the display
        break;
    }
    display_state = new_state;
}
class : public IO::PinChangeListener
{
    void call() override
    {
        uint8_t has_fault = !amp_fault_pin.read();
        uint8_t is_clipping = !amp_clip_pin.read();
        if (has_fault)
        {
            set_amp_state(AmpState::FAULT);
            needs_rerender = true;
        }
        if (is_clipping)
        {
            set_amp_state(AmpState::CLIP);
            needs_rerender = true;
        }
        if (!has_fault && !is_clipping &&
            (amp_state == AmpState::FAULT || amp_state == AmpState::CLIP))
        {
            auto next_amp_state = is_muted ? AmpState::MUTED : AmpState::POWERON;
            set_amp_state(seconds_until_standby == 0 ? AmpState::POWERDOWN
                                                     : next_amp_state);
            needs_rerender = true;
        }
    }
} fault_listener;

class : public IO::PinChangeListener
{
    void call() override
    {
        if (audio_detect.read() && display_state == DisplayState::STANDBY)
        {
            set_display_state(DisplayState::LIGHTSOUT);
        }
    }
} audio_detect_listener;

void on_timer_ticked()
{
    Serial::UART::the.puts(".");

    if (seconds_until_lightsout > 0)
    {
        seconds_until_lightsout--;
    }
    else if (seconds_until_standby > 0)
    {
        set_display_state(DisplayState::LIGHTSOUT);

        if (!audio_detect.read())
        {
            seconds_until_standby--;
        }
    }
    else
    {
        set_display_state(DisplayState::STANDBY);
    }
}

void setup()
{
    Serial::UART::the.init(9600);
    I2C::HardwareMaster::the.init();
    lcd.init();
    load_progressbar_chars(lcd);
    encoder.init();

    amp_clip_pin.pc_port().attach(amp_clip_pin);
    amp_fault_pin.pc_port().attach(amp_fault_pin);
    amp_clip_pin.pc_port().attach_callback(&fault_listener);
    amp_fault_pin.pc_port().attach_callback(&fault_listener);

    audio_detect.pc_port().attach(audio_detect);
    audio_detect.pc_port().attach_callback(&audio_detect_listener);

    Timer::init();
    Timer::every(1000, on_timer_ticked);

    for (auto &channel : channels)
    {
        channel.load_saved_volume(pt2257);
    }
    channels[current_channel].render(lcd, false);
    set_amp_state(AmpState::POWERON);
    set_display_state(DisplayState::POWERON);
}

void loop()
{
    // always make sure the timer is running when we start the loop since we
    // may come from power down mode
    Timer::start();
    // set the sleep mode. this may be overridden while the loop runs if we call
    // enter_standby in the timer callback/
    // SLEEP_MODE_IDLE is the only mode that allows the timer to run
    set_sleep_mode(SLEEP_MODE_IDLE);

    int8_t encoder_delta = encoder.delta();
    if (encoder_delta != 0)
    {
        reset_timeout_timers();
        set_display_state(DisplayState::POWERON);
        needs_rerender = true;
    }

    if (mute_button.was_pressed())
    {
        reset_timeout_timers();
        set_display_state(DisplayState::POWERON);
        is_muted = !is_muted;
        if (is_muted)
        {
            set_amp_state(AmpState::MUTED);
        }
        else
        {
            set_amp_state(AmpState::POWERON);
        }
        needs_rerender = true;
    }

    if (enc_button.was_pressed())
    {
        reset_timeout_timers();
        set_display_state(DisplayState::POWERON);
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
    }

    if (needs_rerender)
    {
        needs_rerender = false;
        if (amp_state == AmpState::FAULT || amp_state == AmpState::CLIP)
        {
            lcd.clear();
            lcd.setCursor(0, LiquidCrystal::Row::Second);
            lcd.puts("\x06\x06   FAULT!   \x07\x07");
            return;
        }
        if (amp_state == AmpState::MUTED)
        {
            lcd.clear();
            lcd.setCursor(0, LiquidCrystal::Row::Second);
            lcd.puts("\x06\x06    Mute    \x07\x07");
            return;
        }

        channels[current_channel].render(lcd, channel_active);
    }
    sleep_mode();
}

int main()
{
    setup();
    while (1)
    {
        loop();
    }
}
