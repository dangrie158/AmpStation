#include "Timer.h"

#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Serial.h"

volatile uint16_t Timer::s_millis = 0;
List<Timer *> Timer::s_timers = {};

constexpr uint8_t clock_setting = (1 << CS11) | (1 << CS10);
void Timer::init()
{
    // Set CTC mode
    TCCR1B |= (1 << WGM12);

    // Set compare value
    OCR1A = (F_CPU / 64 / 1000);

    // Enable compare interrupt
    TIMSK1 |= (1 << OCIE1A);

    start();
};

void Timer::stop()
{
    // stop the clock
    TCCR1B &= ~clock_setting;
}

void Timer::start()
{
    // Set prescaler to 64 to start the clock
    TCCR1B |= clock_setting;
}

void Timer::every(uint16_t period, void (*callback)())
{
    struct Timer *new_timer = (Timer *)malloc(sizeof(class Timer));
    new_timer->m_period = period;
    new_timer->m_next_scheduled = now() + period;
    new_timer->m_callback = callback;
    Timer::s_timers.push(new_timer);
}

void Timer1OverflowInterruptHandler()
{
    ++Timer::s_millis;

    Timer::s_timers.for_each([](Timer *timer)
                             {
                                auto now = Timer::s_millis;
                                if(now == timer->m_next_scheduled) {
                                    timer->m_next_scheduled = now + timer->m_period;
                                    timer->m_callback();
                                } });
}

ISR(TIMER1_COMPA_vect)
{
    Timer1OverflowInterruptHandler();
}
