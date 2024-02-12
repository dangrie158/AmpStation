#pragma once

#include <inttypes.h>

void Timer1OverflowInterruptHandler();

class Timer
{
public:
    static void init();
    inline static uint16_t now()
    {
        return s_millis;
    }
    friend void Timer1OverflowInterruptHandler();

private:
    static volatile uint16_t s_millis;
};
