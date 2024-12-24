#pragma once

#include <inttypes.h>

#include "AdvancedIO.h"
#include "Timer.h"

namespace IR
{

    enum class Command : uint32_t
    {
        UNKNOWN = 0,
        MUTE = 0xF00F0707,
        VOLUME_UP = 0xF8070707,
        VOLUME_DOWN = 0xF40B0707,
    };

    class CommandListener
    {
    public:
        virtual void on_receive(Command command) = 0;
    };

    class Receiver : public IO::PinChangeListener
    {
    public:
        Receiver(const IO::PinChangeInterruptPin &pin) : m_pin(pin) {}

        void begin(CommandListener *callback);
        void call() override;

    private:
        void on_bit_received(uint32_t interval);

        IO::PinChangeInterruptPin m_pin;
        CommandListener *m_callback;

        uint32_t m_current_signal = 0;
        uint8_t m_current_bit = 0;
        bool m_signal_started = false;
        uint32_t m_last_change = Timer::now_micros();

        constexpr static uint16_t bit_width_us = 562; // 562.5µs actually 🤷‍♂️
        constexpr static uint16_t space_duration = Receiver::bit_width_us * 1;
        constexpr static uint16_t mark_duration = Receiver::bit_width_us * 3;
        constexpr static uint16_t start_duration = 4500;
        constexpr static uint16_t tolerance = Receiver::bit_width_us / 2;

        inline bool is_space(uint32_t duration)
        {
            return abs(duration - Receiver::space_duration) < Receiver::tolerance;
        }

        inline bool is_mark(uint32_t duration)
        {
            return abs(duration - Receiver::mark_duration) < Receiver::tolerance;
        }

        inline bool is_start(uint32_t duration)
        {
            return abs(duration - Receiver::start_duration) < Receiver::tolerance;
        }
    };

}
