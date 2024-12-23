#include "I2C.h"
#include <avr/io.h>
#include <util/twi.h>

const I2C::HardwareMaster I2C::HardwareMaster::the = {};

void I2C::HardwareMaster::init() const
{
    TWSR = 0;                              // no prescaler
    TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2; // set bit rate
}

I2C::Result I2C::HardwareMaster::start(uint8_t address, Direction direction) const
{
    if (direction == Direction::Read)
    {
        address |= 1;
    }

    // send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // wait until transmission completed
    while (!(TWCR & (1 << TWINT)))
        ;

    // check status register
    if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START))
        return Result::Error;

    // send device address
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wail until transmission completed and ACK/NACK has been received
    while (!(TWCR & (1 << TWINT)))
        ;

    // check status register
    if ((TW_STATUS != TW_MT_SLA_ACK) && (TW_STATUS != TW_MR_SLA_ACK))
        return Result::Error;

    return Result::Success;
}

void I2C::HardwareMaster::start_wait(uint8_t address, Direction direction) const
{
    while (1)
    {
        if (start(address, direction) == Result::Success)
        {
            break;
        }
    }
}

void I2C::HardwareMaster::stop() const
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while (TWCR & (1 << TWSTO))
        ;
}

I2C::Result I2C::HardwareMaster::write(uint8_t data) const
{
    // send data to the previously addressed device
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait until transmission completed
    while (!(TWCR & (1 << TWINT)))
        ;

    // check status register Mask prescaler bits
    if (TW_STATUS != TW_MT_DATA_ACK)
        return Result::Error;
    return Result::Success;
}

uint8_t I2C::HardwareMaster::read(uint8_t do_ack) const
{
    uint8_t command = (1 << TWINT) | (1 << TWEN);
    if (do_ack)
    {
        command |= (1 << TWEA);
    }

    TWCR = command;
    while (!(TWCR & (1 << TWINT)))
        ;

    return TWDR;
}

void I2C::SoftwareMaster::init() const
{
    m_scl.clear();
    m_sda.clear();
    scl_set();
    sda_set();
    bit_clock();
}

I2C::Result I2C::SoftwareMaster::start(uint8_t address, Direction direction) const
{
    m_sda.set_mode(IO::Direction::Input);
    m_scl.set_mode(IO::Direction::Input);

    sda_set();
    scl_set();
    bit_clock();

    sda_clear();
    bit_clock();
    scl_clear();
    bit_clock();

    if (direction == Direction::Read)
    {
        address |= 1;
    }
    return write(address);
}

void I2C::SoftwareMaster::start_wait(uint8_t address, Direction direction) const
{
    start(address, direction);
}

void I2C::SoftwareMaster::stop() const
{
    sda_clear();
    bit_clock();
    scl_set();
    bit_clock();
    sda_set();
    scl_clear();
    bit_clock();
}

I2C::Result I2C::SoftwareMaster::write(uint8_t data) const
{
    for (uint8_t i = 0; i < 8; i++)
    {
        write_bit(data & 0x80);
        data <<= 1;
    }

    return read_bit() ? Result::Error : Result::Success;
}

uint8_t I2C::SoftwareMaster::read(uint8_t do_ack) const
{
    uint8_t result = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        result <<= 1;
        result |= read_bit();
    }

    if (do_ack)
    {
        write_bit(0);
    }
    else
    {
        write_bit(1);
    }
    bit_clock();

    return result;
}
