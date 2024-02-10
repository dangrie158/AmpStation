#pragma once

#include <inttypes.h>
#include "IO.h"
#include "List.h"
#include <avr/interrupt.h>

namespace IO
{
    void PCI0Interrupt();
    void PCI1Interrupt();
    void PCI2Interrupt();

    class PinChangeListener
    {
    public:
        virtual void call() = 0;
    };

    class PinChangeInterruptPort
    {
    public:
        static PinChangeInterruptPort PortB;
        static PinChangeInterruptPort PortC;
        static PinChangeInterruptPort PortD;

        void attach(Input pin) const
        {
            *m_mask_register |= (1 << pin.pin());
            PCICR |= (1 << m_interrupt_control_bit);
            sei();
        }

        void disable(Input pin) const
        {
            *m_mask_register &= ~(1 << pin.pin());
            if (*m_mask_register == 0)
            {
                PCICR &= ~(1 << m_interrupt_control_bit);
            }
        };

        void attach_callback(PinChangeListener *listener)
        {
            pin_listeners.push(listener);
        }

        const Port &port() const
        {
            return m_port;
        }

        friend void PCI0Interrupt();
        friend void PCI1Interrupt();
        friend void PCI2Interrupt();

    private:
        PinChangeInterruptPort(volatile uint8_t *mask_register, uint8_t interrupt_control_bit, const Port &port)
            : m_mask_register(mask_register), m_interrupt_control_bit(interrupt_control_bit), m_port(port) {}

        volatile uint8_t *m_mask_register;
        uint8_t m_interrupt_control_bit;
        const Port &m_port;
        List<PinChangeListener *> pin_listeners = {};
        void call_listeners()
        {
            pin_listeners.for_each([](PinChangeListener *listener)
                                   { listener->call(); });
        }
    };

    class PinChangeInterruptPin : public Input
    {
    public:
        PinChangeInterruptPin(PinChangeInterruptPort &pc_port, uint8_t pin, PullupMode pullup)
            : Input(pc_port.port(), pin, pullup), m_pc_port(pc_port) {}

        PinChangeInterruptPort &pc_port() const
        {
            return m_pc_port;
        }

    private:
        PinChangeInterruptPort &m_pc_port;
    };
};
