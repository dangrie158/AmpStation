#include "RotaryEncoder.h"
#include <avr/pgmspace.h>
#include "lib/Serial.h"

const int8_t state_decoding_table[16] PROGMEM = {0, 0, -1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 0, 0};

void RotaryEncoder::init()
{
    m_pin_a.pc_port().attach(m_pin_a);
    m_pin_a.pc_port().attach_callback(this);
    m_pin_b.pc_port().attach(m_pin_b);
    if (&m_pin_a.pc_port() != &m_pin_b.pc_port())
    {
        m_pin_b.pc_port().attach_callback(this);
    }
}

void RotaryEncoder::call()
{
    m_last_state = (m_last_state << 2) & 0x0F;
    if (m_pin_a.read())
    {
        m_last_state |= 2;
    }

    if (m_pin_b.read())
    {
        m_last_state |= 1;
    }
    m_delta += pgm_read_byte(&state_decoding_table[m_last_state]);
}
