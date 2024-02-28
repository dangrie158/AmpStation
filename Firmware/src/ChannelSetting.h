#pragma once
#include <inttypes.h>

#include "lib/PT2258.h"
#include "lib/LiquidCrystal.h"

class ChannelSetting
{
public:
    ChannelSetting(const char *name, PT2258::Channel channel_1, PT2258::Channel channel_2, uint8_t *address)
        : m_name(name), m_channel_1(channel_1), m_channel_2(channel_2), m_storage_address(address) {}

    void save_volume();
    void load_saved_volume(PT2258 &pt2258);
    void set_attenuation(PT2258 &pt2258, int8_t attenuation);
    inline void offset_volume(PT2258 &pt2258, int8_t delta) { set_attenuation(pt2258, m_attenuation + delta); }
    void render(LiquidCrystal::Display &lcd, bool active);

private:
    const char *m_name;
    PT2258::Channel m_channel_1;
    PT2258::Channel m_channel_2;
    uint8_t *m_storage_address;
    int8_t m_attenuation;
};
