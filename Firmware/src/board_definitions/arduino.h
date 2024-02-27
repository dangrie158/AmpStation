#pragma once

#include "lib/IO.h"
#include "lib/AdvancedIO.h"

auto rs = IO::Output(IO::Port::PortB, 0);
auto rw = IO::Output(IO::Port::PortB, 1);
auto en = IO::Output(IO::Port::PortB, 2);
auto d4 = IO::Output(IO::Port::PortD, 4);
auto d5 = IO::Output(IO::Port::PortD, 5);
auto d6 = IO::Output(IO::Port::PortD, 6);
auto d7 = IO::Output(IO::Port::PortD, 7);
const auto lcd_backlight = IO::Output(IO::Port::PortB, 5);

const auto ringlight_bright = IO::Output(IO::Port::PortC, 4);
const auto ringlight_dimmed = IO::Output(IO::Port::PortC, 4);
const auto amp_power = IO::Output(IO::Port::PortC, 2);
const auto audio_detect = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortC, 0, IO::PullupMode::Disable);

auto enc_a = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortC, 4, IO::PullupMode::Enable);
auto enc_b = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortC, 3, IO::PullupMode::Enable);
auto enc_button_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortC, 5, IO::PullupMode::Enable);

auto mute_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortC, 2, IO::PullupMode::Enable);
auto amp_fault_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortC, 1, IO::PullupMode::Enable);
auto amp_clip_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortC, 1, IO::PullupMode::Enable);
