#pragma once
#include <inttypes.h>

#include "lib/PT2257.h"
#include "lib/LiquidCrystal.h"

class ChannelSetting
{
public:
    ChannelSetting(const char *name, PT2257::Channel channel_1, PT2257::Channel channel_2, uint8_t *address)
        : m_name(name), m_channel_1(channel_1), m_channel_2(channel_2), m_storage_address(address) {}

    void save_volume();
    void load_saved_volume(PT2257 &pt2257);
    void set_volume(PT2257 &pt2257, int8_t volume);
    inline void offset_volume(PT2257 &pt2257, int8_t delta) { set_volume(pt2257, m_volume + delta); }
    void render(LiquidCrystal::Display &lcd, bool active);

private:
    const char *m_name;
    PT2257::Channel m_channel_1;
    PT2257::Channel m_channel_2;
    uint8_t *m_storage_address;
    int8_t m_volume;
};
