#pragma once

#include "lib/IO.h"
#include "lib/AdvancedIO.h"
#include "lib/I2C.h"

auto rs = IO::Output(IO::Port::PortB, 0);
auto rw = IO::Output(IO::Port::PortB, 1);
auto en = IO::Output(IO::Port::PortB, 2);
auto d4 = IO::Output(IO::Port::PortB, 3);
auto d5 = IO::Output(IO::Port::PortB, 4);
auto d6 = IO::Output(IO::Port::PortB, 5);
auto d7 = IO::Output(IO::Port::PortB, 6);
const auto lcd_backlight = IO::Output(IO::Port::PortD, 7);

const auto ringlight_bright = IO::Output(IO::Port::PortD, 6);
const auto ringlight_dimmed = IO::Output(IO::Port::PortD, 5);
const auto amp_power = IO::Output(IO::Port::PortC, 2);
const auto audio_detect = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortC, 3, IO::PullupMode::Disable);
const auto ir_receiver_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortD, 0, IO::PullupMode::Disable);

auto enc_a = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortB, 7, IO::PullupMode::Enable);
auto enc_b = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortD, 1, IO::PullupMode::Enable);
auto enc_button_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortD, 2, IO::PullupMode::Enable);

auto mute_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortD, 3, IO::PullupMode::Enable);
auto amp_fault_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortC, 1, IO::PullupMode::Enable);
auto amp_clip_pin = IO::PinChangeInterruptPin(IO::PinChangeInterruptPort::PortC, 0, IO::PullupMode::Enable);

auto sda_pin = IO::IO(IO::Port::PortC, 5, IO::Direction::Input);
auto scl_pin = IO::IO(IO::Port::PortC, 4, IO::Direction::Input);
const I2C::Master &i2c_bus = I2C::SoftwareMaster(scl_pin, sda_pin);

constexpr uint8_t encoder_stepsize = 2;
constexpr uint16_t standby_delay_seconds = 120;
constexpr uint16_t lightsout_delay_seconds = 10;
