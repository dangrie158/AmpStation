#include "ChannelSetting.h"

#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>

#include "src/Progressbar.h"
void ChannelSetting::save_volume()
{
    eeprom_write_byte(m_storage_address, (uint8_t)m_attenuation);
}

void ChannelSetting::load_saved_volume(PT2258 &pt2258)
{
    m_attenuation = (int8_t)eeprom_read_byte(m_storage_address);
    set_attenuation(pt2258, m_attenuation);
}

void ChannelSetting::set_attenuation(PT2258 &pt2258, int8_t attenuation)
{

    m_attenuation = attenuation > 0     ? 0
                    : attenuation < -79 ? -79
                                        : attenuation;
    pt2258.set_attenuation(m_channel_1, -m_attenuation);
    pt2258.set_attenuation(m_channel_2, -m_attenuation);
}

void ChannelSetting::render(LiquidCrystal::Display &lcd, bool active)
{
    lcd.clear();
    uint8_t progress = (m_attenuation + PT2258::max_volume) * 100 / PT2258::max_volume;
    draw_progressbar(lcd, LiquidCrystal::Row::First, progress, active);
    lcd.setCursor(1, LiquidCrystal::Row::Second);
    lcd.puts(m_name);
    char volume_str[16];
    sprintf(volume_str, "%02ddB", m_attenuation);

    lcd.setCursor(lcd.width() - strnlen(volume_str, 16) - 1, LiquidCrystal::Row::Second);
    lcd.puts(volume_str);
}
