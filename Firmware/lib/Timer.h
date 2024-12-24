#pragma once

#include <inttypes.h>

#include <lib/List.h>

void Timer1OverflowInterruptHandler();

class Timer
{
public:
    Timer(uint16_t period, void (*callback)())
        : m_period(period), m_next_scheduled(now() + m_period), m_callback(callback) {}

    static void init();
    static void stop();
    static void start();
    static void every(uint16_t period, void (*callback)());

    inline static uint16_t now()
    {
        return s_millis;
    }

    inline static uint32_t now_micros()
    {
        return s_tenth * 100UL + s_millis * 1000UL;
    }

    friend void Timer1OverflowInterruptHandler();

private:
    uint16_t m_period;
    volatile uint16_t m_next_scheduled = 0;
    void (*m_callback)();

    static volatile uint16_t s_millis;
    static volatile uint16_t s_tenth;
    static List<Timer *> s_timers;
};
