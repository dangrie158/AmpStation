#pragma once
#include <inttypes.h>
#include <avr/io.h>

namespace IO
{
    enum class PullupMode
    {
        Disable = false,
        Enable = true
    };

    enum class Direction
    {
        Input = 0,
        Output = 1,
        InputWithPullup = 2
    };

    class Port
    {
    public:
        static const Port PortB;
        static const Port PortC;
        static const Port PortD;

        Port(volatile uint8_t *port, volatile uint8_t *ddr, volatile uint8_t *pin)
            : m_port(port), m_ddr(ddr), m_pin(pin) {}

        inline const void write(const uint8_t value) const
        {
            *this->m_port = value;
        }

        inline const void set(const uint8_t mask) const
        {
            *this->m_port |= mask;
        }

        inline const void clear(const uint8_t mask) const
        {
            *this->m_port &= ~mask;
        }

        inline const void set_output(const uint8_t mask) const
        {
            *this->m_ddr |= mask;
        }

        inline const void set_input(const uint8_t mask) const
        {
            *this->m_ddr &= ~mask;
            *this->m_port &= ~mask;
        }

        inline const void set_input_pullup(const uint8_t mask) const
        {
            *this->m_ddr &= ~mask;
            *this->m_port |= mask;
        }

        uint8_t read() const
        {
            return *this->m_pin;
        }

    private:
        volatile uint8_t *m_port;
        volatile uint8_t *m_ddr;
        volatile uint8_t *m_pin;
    };

    class Output
    {
    public:
        Output(const Port &port, uint8_t pin)
            : m_port(port),
              m_pin(pin)
        {
            m_port.set_output(1 << m_pin);
        }

        inline void set() const
        {
            m_port.set(1 << m_pin);
        }

        inline void clear() const
        {
            m_port.clear(1 << m_pin);
        }

        inline void write(uint8_t value)
        {
            value ? set() : clear();
        }

    private:
        const Port &m_port;
        const uint8_t m_pin;
    };

    class Input
    {
    public:
        Input(const Port &port, uint8_t pin, PullupMode pullup)
            : m_port(port), m_pin(pin)
        {
            if (pullup == PullupMode::Enable)
            {
                m_port.set_input_pullup(1 << m_pin);
            }
            else
            {
                m_port.set_input(1 << m_pin);
            }
        }

        inline bool read() const
        {
            return m_port.read() & (1 << m_pin);
        }

    private:
        const Port &m_port;
        const uint8_t m_pin;
    };

    class IO
    {
    public:
        IO(const Port &port, uint8_t pin, Direction mode)
            : m_port(port), m_pin(pin)
        {
            set_mode(mode);
        }

        void set_mode(Direction mode) const
        {
            switch (mode)
            {
            case Direction::Input:
                m_port.set_input(1 << m_pin);
                break;
            case Direction::Output:
                m_port.set_output(1 << m_pin);
                break;
            case Direction::InputWithPullup:
                m_port.set_input_pullup(1 << m_pin);
                break;
            }
        }

        inline void set() const
        {
            set_mode(Direction::Output);
            m_port.set(1 << m_pin);
        }

        inline void clear() const
        {
            set_mode(Direction::Output);
            m_port.clear(1 << m_pin);
        }

        inline uint8_t read() const
        {
            set_mode(Direction::Input);
            return m_port.read() & (1 << m_pin);
        }

    private:
        const Port &m_port;
        const uint8_t m_pin;
    };
}
