#pragma once
#include "IO.h"
#include "AdvancedIO.h"
#include "Button.h"

class RotaryEncoder : public Button, IO::PinChangeListener
{
public:
    RotaryEncoder(IO::PinChangeInterruptPin &pin_a, IO::PinChangeInterruptPin &pin_b, IO::Input &pin_button)
        : Button(pin_button), m_pin_a(pin_a), m_pin_b(pin_b) {}

    void init();
    void call() override;
    inline uint8_t delta()
    {
        cli();
        uint8_t value = m_delta;
        m_delta = 0;
        sei();
        return value;
    }

private:
    IO::PinChangeInterruptPin &m_pin_a;
    IO::PinChangeInterruptPin &m_pin_b;

    int8_t m_last_state = 0;
    volatile int8_t m_delta = 0;
};
