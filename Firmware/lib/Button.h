#pragma once

#include "IO.h"

class Button
{
public:
    Button(IO::Input &pin, bool active_high = false)
        : m_pin(pin), m_active_high(active_high) {}

    inline bool is_pressed()
    {
        return (!m_active_high) ^ m_pin.read();
    }

private:
    IO::Input &m_pin;
    bool m_active_high;
};
