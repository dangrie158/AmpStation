#include "IRReceiver.h"
#include "Timer.h"
#include "Serial.h"
#include <math.h>
#include <avr/pgmspace.h>

namespace IR
{
    void Receiver::begin(CommandListener *callback)
    {
        m_callback = callback;
        m_pin.pc_port().attach_callback(this);
        m_pin.pc_port().attach(m_pin);
    }

    void Receiver::on_bit_received(uint32_t interval)
    {
        if (is_start(interval) && !m_signal_started)
        {
            m_current_bit = 0;
            m_current_signal = 0;
            m_signal_started = true;
        }
        else if (is_space(interval) && m_signal_started)
        {
            m_current_bit++;
        }
        else if (is_mark(interval) && m_signal_started)
        {
            m_current_signal |= 1UL << m_current_bit++;
        }

        if (m_current_bit == 32 && m_signal_started)
        {
            m_signal_started = false;
            switch (m_current_signal)
            {
            case (uint32_t)Command::MUTE:
                this->m_callback->on_receive(Command::MUTE);
                break;
            case (uint32_t)Command::VOLUME_UP:
                this->m_callback->on_receive(Command::VOLUME_UP);
                break;
            case (uint32_t)Command::VOLUME_DOWN:
                this->m_callback->on_receive(Command::VOLUME_DOWN);
                break;
            }
        }

        // Command command = Command::UNKNOWN;
        // if (m_callback)
        // {
        //     m_callback->on_receive(command);
        // }
    }

    void Receiver::call()
    {
        uint8_t state = m_pin.read();
        uint32_t now = Timer::now_micros();
        // bits are detemined by the duration between rising and falling edge
        if (state == 0)
        {
            uint32_t diff = now - m_last_change;
            Receiver::on_bit_received(diff);
        }
        m_last_change = now;
    }
}
