#pragma once

#include <inttypes.h>
#include "I2C.h"

class PT2257
{
public:
    constexpr static uint8_t min_volume = 0;
    constexpr static uint8_t max_volume = 79;

    enum Channel
    {
        CHANNEL_MASTER = 0,
        CHANNEL_1 = 1,
        CHANNEL_2 = 2,
        CHANNEL_3 = 3,
        CHANNEL_4 = 4,
        CHANNEL_5 = 5,
        CHANNEL_6 = 6,
    };
    const uint8_t CMD_MUTE = 0b11111001;
    const uint8_t CMD_UNMUTE = 0b11111000;

    static const uint8_t channel_registers[7][2];

    PT2257(const I2C::HardwareMaster &bus, uint8_t address) : m_bus(bus),
                                                              m_address(address) {}

    void set_volume(Channel channel, uint8_t volume);
    void mute();
    void unmute();

private:
    const I2C::HardwareMaster &m_bus;
    uint8_t m_address;
};
