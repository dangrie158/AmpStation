#pragma once
#include <inttypes.h>

#include "lib/PT2258.h"
#include "lib/LiquidCrystal.h"

class ChannelSetting
{
public:
    ChannelSetting(const char *name, const char *unit, uint8_t *address)
        : m_name(name), m_unit(unit), m_storage_address(address) {}

    inline void offset_volume(PT2258 &pt2258, int8_t delta) { set_value(pt2258, m_value + delta); }
    void save_volume();
    void load_saved_volume(PT2258 &pt2258);
    void render(LiquidCrystal::Display &lcd, bool active);

    virtual void set_value(PT2258 &pt2258, int8_t value) = 0;
    virtual uint8_t get_progress() = 0;

protected:
    int8_t m_value = 0;

private:
    const char *m_name;
    const char *m_unit;
    uint8_t *m_storage_address;
};
class SingleChannelSetting : public ChannelSetting
{
public:
    SingleChannelSetting(const char *name, PT2258::Channel channel_1, PT2258::Channel channel_2, uint8_t *address)
        : ChannelSetting(name, "dB", address), m_channel_1(channel_1), m_channel_2(channel_2) {}

    void set_value(PT2258 &pt2258, int8_t attenuation);
    inline uint8_t get_progress() { return (m_value + PT2258::max_attenuation) * 100 / PT2258::max_attenuation; };

private:
    PT2258::Channel m_channel_1;
    PT2258::Channel m_channel_2;
};

class MasterChannelSetting : public ChannelSetting
{
public:
    MasterChannelSetting(uint8_t *address)
        : ChannelSetting("Master", "", address) {}

    void set_value(PT2258 &pt2258, int8_t attenuation);
    inline uint8_t get_progress() { return m_value; };
};
