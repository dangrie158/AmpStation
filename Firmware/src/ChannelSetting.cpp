#include "ChannelSetting.h"

#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>

#include "src/Progressbar.h"
void ChannelSetting::save_volume()
{
    eeprom_write_byte(m_storage_address, (uint8_t)m_value);
}

void ChannelSetting::load_saved_volume(PT2258 &pt2258)
{
    m_value = (int8_t)eeprom_read_byte(m_storage_address);
    set_value(pt2258, m_value);
}

void ChannelSetting::render(LiquidCrystal::Display &lcd, bool active)
{
    lcd.clear();
    uint8_t progress = get_progress();
    draw_progressbar(lcd, LiquidCrystal::Row::First, progress, active);
    lcd.set_cursor(1, LiquidCrystal::Row::Second);
    lcd.puts(m_name);
    char volume_str[17];
    snprintf(volume_str, 16, "%02d%s", m_value, m_unit);

    lcd.set_cursor(lcd.width() - strnlen(volume_str, 16) - 1, LiquidCrystal::Row::Second);
    lcd.puts(volume_str);
}

void SingleChannelSetting::set_value(PT2258 &pt2258, int8_t attenuation)
{

    m_value = attenuation > 0     ? 0
              : attenuation < -79 ? -79
                                  : attenuation;
    pt2258.set_attenuation(m_channel_1, -m_value);
    pt2258.set_attenuation(m_channel_2, -m_value);
}

void MasterChannelSetting::set_value(PT2258 &pt2258, int8_t volume)
{
    m_value = volume > 100 ? 100
              : volume < 0 ? 0
                           : volume;
    pt2258.set_global_volume(m_value);
}
