#pragma once

#include "IO.h"
#include "Serial.h"
#include "Timer.h"
class Button : public IO::PinChangeListener
{
public:
    static constexpr uint16_t debounce_time_ms = 200;

    Button(IO::PinChangeInterruptPin &pin, bool active_high = false)
        : m_pin(pin), m_active_high(active_high)
    {
        pin.pc_port().attach(pin);
        pin.pc_port().attach_callback(this);
    }

    Button(IO::Input &pin, bool active_high = false)
        : m_pin(pin), m_active_high(active_high) {}

    inline bool is_pressed()
    {
        return (!m_active_high) ^ m_pin.read();
    }

    inline bool was_pressed()
    {
        if (m_was_clicked)
        {
            m_was_clicked = false;
            return true;
        }
        return false;
    }

    void call() override
    {
        auto current_state = is_pressed();
        auto now = Timer::now();

        if (current_state && (now - m_last_pressed) > debounce_time_ms && !m_was_pressed)
        {
            m_was_pressed = true;
            m_last_pressed = now;
            m_was_clicked = true;
        }
        else if (!current_state)
        {
            m_was_pressed = false;
        }
    }

private:
    IO::Input &m_pin;
    bool m_active_high;

    volatile uint16_t m_last_pressed = 0;
    volatile bool m_was_clicked = false;
    volatile bool m_was_pressed = false;
};
