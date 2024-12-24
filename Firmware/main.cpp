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
#include "lib/PT2258.h"
#include "lib/Timer.h"
#include "lib/IRReceiver.h"

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#include "src/Progressbar.h"
#include "src/ChannelSetting.h"

auto lcd = LiquidCrystal::Display(rs, rw, en, d4, d5, d6, d7);
auto pt2258 = PT2258(i2c_bus, 0x88);
auto enc_button = Button(enc_button_pin);
auto encoder = RotaryEncoder(enc_a, enc_b, encoder_stepsize);
auto mute_button = Button(mute_pin);
auto ir_receiver = IR::Receiver(ir_receiver_pin);

auto master_channel = MasterChannelSetting((uint8_t *)0);
auto toslink_channel = SingleChannelSetting("TOSLINK", PT2258::Channel::CHANNEL_1, PT2258::Channel::CHANNEL_2, (uint8_t *)1);
auto line1_channel = SingleChannelSetting("Line 1", PT2258::Channel::CHANNEL_5, PT2258::Channel::CHANNEL_6, (uint8_t *)2);
auto line2_channel = SingleChannelSetting("Line 2", PT2258::Channel::CHANNEL_3, PT2258::Channel::CHANNEL_4, (uint8_t *)3);
ChannelSetting *channels[] = {&master_channel, &toslink_channel, &line1_channel, &line2_channel};

uint8_t current_channel = 0;
bool channel_active = false;
bool is_muted = false;
volatile uint16_t seconds_until_standby = standby_delay_seconds;
volatile uint16_t seconds_until_lightsout = lightsout_delay_seconds;
volatile uint16_t last_mute_action = 0;
volatile bool needs_rerender = false;
volatile IR::Command next_ir_command = IR::Command::UNKNOWN;

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
    seconds_until_lightsout = lightsout_delay_seconds;
    last_mute_action = Timer::now();
}

void set_amp_state(AmpState new_state)
{
    switch (new_state)
    {
    case AmpState::POWERON:
        amp_power.high();
        pt2258.unmute();
        break;
    case AmpState::MUTED:
        amp_power.low();
        pt2258.mute();
        break;
    case AmpState::POWERDOWN:
        amp_power.low();
        pt2258.unmute(); // unmute so we can detect audio
        break;
    case AmpState::CLIP:
        amp_power.low();
        pt2258.mute();
        break;
    case AmpState::FAULT:
        amp_power.low();
        pt2258.mute();
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
        lcd.init();
        lcd.display();
        ringlight_bright.high();
        ringlight_dimmed.low();
        needs_rerender = true;
        break;
    case DisplayState::LIGHTSOUT:
        set_amp_state(AmpState::POWERON);
        ringlight_dimmed.high();
        if (display_state == DisplayState::POWERON)
        {
            ringlight_bright.low();
            lcd_backlight.low();
            lcd.no_display();
        }
        else if (display_state == DisplayState::STANDBY)
        {
            seconds_until_standby = standby_delay_seconds;
            Timer::start();
        }
        break;
    case DisplayState::STANDBY:
        if (amp_state != AmpState::CLIP || amp_state != AmpState::FAULT)
        {
            set_amp_state(AmpState::POWERDOWN);
        }
        ringlight_bright.low();
        ringlight_dimmed.low();
        lcd_backlight.low();
        lcd.no_display();

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
        if (audio_detect.read())
        {
            if (display_state == DisplayState::STANDBY)
            {
                set_display_state(DisplayState::LIGHTSOUT);
            }
            seconds_until_standby = standby_delay_seconds;
        }
    }
} audio_detect_listener;

class : public IR::CommandListener
{
    void on_receive(IR::Command command)
    {
        next_ir_command = command;
    }
} ir_receiver_listener;

void on_timer_ticked()
{
    if (seconds_until_lightsout > 0)
        if (seconds_until_lightsout > 0)
        {
            seconds_until_lightsout--;
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
    Serial::UART::the.init(115200, Serial::Direction::TX_ONLY);
    i2c_bus.init();
    lcd.init();
    load_progressbar_chars(lcd);
    encoder.init();

    amp_clip_pin.pc_port().attach(amp_clip_pin);
    amp_fault_pin.pc_port().attach(amp_fault_pin);
    amp_clip_pin.pc_port().attach_callback(&fault_listener);
    amp_fault_pin.pc_port().attach_callback(&fault_listener);

    audio_detect.pc_port().attach(audio_detect);
    audio_detect.pc_port().attach_callback(&audio_detect_listener);

    ir_receiver.begin(&ir_receiver_listener);

    Timer::init();
    Timer::every(1000, on_timer_ticked);

    for (auto &channel : channels)
    {
        channel->load_saved_volume(pt2258);
    }
    channels[current_channel]->render(lcd, false);
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
    if (encoder_delta != 0 || next_ir_command != IR::Command::UNKNOWN)
    {
        reset_timeout_timers();
        set_display_state(DisplayState::POWERON);
        needs_rerender = true;
    }

    // ignore mutliple mute actions within the same second
    bool mute_action_valid = last_mute_action + 1000 < Timer::now();
    if ((mute_button.was_pressed() || next_ir_command == IR::Command::MUTE) && mute_action_valid)
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
        if (!channel_active)
        {
            channels[current_channel]->save_volume();
        }
        needs_rerender = true;
    }

    if (!is_muted && next_ir_command == IR::Command::VOLUME_UP)
    {
        channels[current_channel]->offset_volume(pt2258, 1);
        needs_rerender = true;
    }

    if (!is_muted && next_ir_command == IR::Command::VOLUME_DOWN)
    {
        channels[current_channel]->offset_volume(pt2258, -1);
        needs_rerender = true;
    }

    if (!is_muted && encoder_delta != 0)
    {
        if (channel_active)
        {
            channels[current_channel]->offset_volume(pt2258, encoder_delta);
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
            lcd.set_cursor(0, LiquidCrystal::Row::Second);
            lcd.puts("\x06\x06   FAULT!   \x07\x07");
            return;
        }
        if (amp_state == AmpState::MUTED)
        {
            lcd.clear();
            lcd.set_cursor(0, LiquidCrystal::Row::Second);
            lcd.puts("\x06\x06    Mute    \x07\x07");
            return;
        }

        channels[current_channel]->render(lcd, channel_active);
    }

    next_ir_command = IR::Command::UNKNOWN;
    // sleep for 100 ms, which is a simple way to debounce some stuff
    // (e.g. the encoder updating the pt2258). Since we enter sleep mode
    // right after this, we're ready to wake up again as soon as something
    // (PCINT) happens.
    _delay_ms(100);
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
