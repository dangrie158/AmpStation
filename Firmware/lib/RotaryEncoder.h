#pragma once
#include "IO.h"
#include "Button.h"

class RotaryEncoder : Button
{
public:
    RotaryEncoder(IO::Input pin_a, IO::Input pin_b, IO::Input pin_button)
        : Button(pin_button), m_pin_a(pin_a), m_pin_b(pin_b) {}

private:
    IO::Input m_pin_a;
    IO::Input m_pin_b;
};
