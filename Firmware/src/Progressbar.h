#pragma once

#include "inttypes.h"
#include "lib/LiquidCrystal.h"

void load_progressbar_chars(LiquidCrystal::Display &lcd);
void draw_progressbar(LiquidCrystal::Display &lcd, LiquidCrystal::Row row, uint8_t progress, uint8_t active);
