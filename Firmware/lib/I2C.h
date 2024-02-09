#pragma once
#include <inttypes.h>

constexpr auto SCL_CLOCK = 50000L;
static_assert(((F_CPU / SCL_CLOCK) - 16) / 2 >= 2);

namespace I2C
{

    enum class Direction
    {
        Write = 0,
        Read = 1
    };

    enum class Result
    {
        Success = 0,
        Error = 1
    };

    class Master
    {

    public:
        virtual void init() const = 0;
        virtual Result start(uint8_t address, Direction) const = 0;
        virtual void start_wait(uint8_t address, Direction) const = 0;
        virtual void stop() const = 0;
        virtual Result write(uint8_t data) const = 0;
        virtual uint8_t read(uint8_t do_ack) const = 0;
    };

    class HardwareMaster : public Master
    {
    public:
        static const HardwareMaster the;

        virtual void init() const override;
        virtual Result start(uint8_t address, Direction) const override;
        virtual void start_wait(uint8_t address, Direction) const override;
        virtual void stop() const override;
        virtual Result write(uint8_t data) const override;
        virtual uint8_t read(uint8_t do_ack) const override;

    protected:
    private:
        HardwareMaster(){};
        HardwareMaster(HardwareMaster &other) = delete;
        void operator=(const HardwareMaster &) = delete;
    };
}
