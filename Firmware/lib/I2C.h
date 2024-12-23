#pragma once
#include <util/delay.h>
#include <inttypes.h>
#include "IO.h"

constexpr auto SCL_CLOCK = 50000L;
static_assert(((F_CPU / SCL_CLOCK) - 16) / 2 >= 10);

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

    class SoftwareMaster : public Master
    {
    public:
        SoftwareMaster(const IO::IO &scl, const IO::IO &sda)
            : m_scl(scl), m_sda(sda){};

        virtual void init() const override;
        virtual Result start(uint8_t address, Direction) const override;
        virtual void start_wait(uint8_t address, Direction) const override;
        virtual void stop() const override;
        virtual Result write(uint8_t data) const override;
        virtual uint8_t read(uint8_t do_ack) const override;

    protected:
    private:
        inline void scl_set() const
        {
            m_scl.set_mode(IO::Direction::InputWithPullup);
        }
        inline void scl_clear() const
        {
            m_scl.clear();
            m_scl.set_mode(IO::Direction::Output);
        }
        inline void sda_set() const
        {
            m_sda.set_mode(IO::Direction::InputWithPullup);
        }
        inline void sda_clear() const
        {
            m_sda.clear();
            m_sda.set_mode(IO::Direction::Output);
        }
        inline void bit_clock() const
        {
            // 10kHz Clock
            _delay_us(50);
        }

        inline uint8_t read_bit() const
        {
            sda_set();
            scl_set();
            bit_clock();
            uint8_t data = m_sda.read();

            scl_clear();
            return data;
        }

        inline void write_bit(uint8_t data) const
        {
            if (data)
            {
                sda_set();
            }
            else
            {
                sda_clear();
            }

            scl_set();
            bit_clock();

            scl_clear();
            bit_clock();
        }

        const IO::IO &m_scl;
        const IO::IO &m_sda;
    };
}
