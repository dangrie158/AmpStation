#pragma once
#include "IO.h"
#include "AdvancedIO.h"
#include "Button.h"

class RotaryEncoder : IO::PinChangeListener
{
public:
    RotaryEncoder(IO::PinChangeInterruptPin &pin_a, IO::PinChangeInterruptPin &pin_b, uint8_t stepsize = 1)
        : m_pin_a(pin_a), m_pin_b(pin_b), m_stepsize(stepsize) {}

    void init();
    void call() override;
    inline int8_t delta()
    {
        cli();
        int8_t value = m_delta / m_stepsize;
        if (value != 0)
        {
            m_delta = 0;
        }
        sei();
        return value;
    }

private:
    IO::PinChangeInterruptPin &m_pin_a;
    IO::PinChangeInterruptPin &m_pin_b;
    int8_t m_stepsize;

    int8_t m_last_state = 0;
    volatile int8_t m_delta = 0;
};
