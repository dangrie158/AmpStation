#include "Timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t Timer::s_millis = 0;

void Timer::init()
{
    // Set CTC mode
    TCCR1B |= (1 << WGM12);

    // Set compare value
    OCR1A = (F_CPU / 64 / 1000);

    // Set prescaler to 64
    TCCR1B |= (1 << CS11) | (1 << CS10);

    // Enable compare interrupt
    TIMSK1 |= (1 << OCIE1A);
};

void Timer1OverflowInterruptHandler()
{
    Timer::s_millis++;
}

ISR(TIMER1_COMPA_vect)
{
    Timer1OverflowInterruptHandler();
}
