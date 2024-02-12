#include "ChannelSetting.h"

#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>

#include "src/Progressbar.h"
void ChannelSetting::save_volume()
{
    eeprom_write_byte(m_storage_address, (uint8_t)m_volume);
}

void ChannelSetting::load_saved_volume(PT2257 &pt2257)
{
    m_volume = (int8_t)eeprom_read_byte(m_storage_address);
    set_volume(pt2257, m_volume);
}

void ChannelSetting::set_volume(PT2257 &pt2257, int8_t volume)
{

    m_volume = volume > 0     ? 0
               : volume < -79 ? -79
                              : volume;
    pt2257.set_volume(m_channel_1, m_volume);
    pt2257.set_volume(m_channel_2, m_volume);
}

void ChannelSetting::render(LiquidCrystal::Display &lcd, bool active)
{
    lcd.clear();
    uint8_t progress = (m_volume + PT2257::max_volume) * 100 / PT2257::max_volume;
    draw_progressbar(lcd, LiquidCrystal::Row::First, progress, active);
    lcd.setCursor(1, LiquidCrystal::Row::Second);
    lcd.puts(m_name);
    char volume_str[16];
    sprintf(volume_str, "%02ddB", m_volume);

    lcd.setCursor(lcd.width() - strnlen(volume_str, 16) - 1, LiquidCrystal::Row::Second);
    lcd.puts(volume_str);
}
