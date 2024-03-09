#pragma once

#include <inttypes.h>
#include "I2C.h"

class PT2258
{
public:
    constexpr static uint8_t min_attenuation = 0;
    constexpr static uint8_t max_attenuation = 79;

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
    const uint8_t CMD_CLEAR_REGISTER = 0b11000000;

    static const uint8_t channel_registers[7][2];

    PT2258(const I2C::Master &bus, uint8_t address) : m_bus(bus),
                                                      m_address(address) {}

    void begin();
    void set_attenuation(Channel, uint8_t attenuation);
    void mute();
    void unmute();
    void set_global_volume(uint8_t);

private:
    const I2C::Master &m_bus;
    uint8_t m_address;

    uint8_t m_volume = 0;
    uint8_t m_attenuation[7] = {0};
};
