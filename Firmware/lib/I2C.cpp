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
