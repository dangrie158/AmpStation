#include "PT2257.h"

const uint8_t PT2257::channel_registers[7][2] = {
    {0b11100000, 0b11010000},
    {0b10010000, 0b10000000},
    {0b01010000, 0b01000000},
    {0b00010000, 0b00000000},
    {0b00110000, 0b00100000},
    {0b01110000, 0b01100000},
    {0b10110000, 0b10100000},
};

void PT2257::set_volume(Channel channel, uint8_t volume)
{
    // m_bus.start(m_address, I2C::Direction::Write);
    // m_bus.write(channel_registers[channel][0] | (volume / 10));
    // m_bus.write(channel_registers[channel][1] | (volume % 10));
    // m_bus.stop();
}

void PT2257::mute()
{
    // m_bus.start(m_address, I2C::Direction::Write);
    // m_bus.write(CMD_MUTE);
    // m_bus.stop();
}

void PT2257::unmute()
{
    // m_bus.start(m_address, I2C::Direction::Write);
    // m_bus.write(CMD_UNMUTE);
    // m_bus.stop();
}
