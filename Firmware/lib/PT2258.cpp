#include "PT2258.h"

const uint8_t PT2258::channel_registers[7][2] = {
    {0b11100000, 0b11010000},
    {0b10010000, 0b10000000},
    {0b01010000, 0b01000000},
    {0b00010000, 0b00000000},
    {0b00110000, 0b00100000},
    {0b01110000, 0b01100000},
    {0b10110000, 0b10100000},
};

void PT2258::begin()
{
    m_bus.start(m_address, I2C::Direction::Write);
    m_bus.write(CMD_CLEAR_REGISTER);
    m_bus.stop();
}

void PT2258::set_attenuation(Channel channel, uint8_t attenuation)
{
    m_attenuation[channel] = attenuation;

    uint8_t additional_attenuation = ((max_attenuation - attenuation) * (100 - m_volume)) / 100;
    uint8_t final_attenuation = attenuation + additional_attenuation;

    m_bus.start(m_address, I2C::Direction::Write);
    m_bus.write(channel_registers[channel][1] | (final_attenuation / 10));
    m_bus.write(channel_registers[channel][0] | (final_attenuation % 10));
    m_bus.stop();
}

void PT2258::mute()
{
    m_bus.start(m_address, I2C::Direction::Write);
    m_bus.write(CMD_MUTE);
    m_bus.stop();
}

void PT2258::unmute()
{
    m_bus.start(m_address, I2C::Direction::Write);
    m_bus.write(CMD_UNMUTE);
    m_bus.stop();
}

void PT2258::set_global_volume(uint8_t volume)
{
    m_volume = volume > 100 ? 100 : volume;
    for (uint8_t i = CHANNEL_1; i < CHANNEL_6; i++)
    {
        set_attenuation((Channel)i, m_attenuation[i]);
    }
}
