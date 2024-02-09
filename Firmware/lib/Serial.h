#pragma once
#include <inttypes.h>
#include <avr/io.h>
#include "Buffer.h"
namespace Serial
{
    void UartReceiveInterrupt();
    void UartTransmitInterrupt();

    enum UARTStatus : uint8_t
    {
        BufferOverflow = 1 << 0,
        BufferUndeflow = 1 << 1,
        ParityError = 1 << UPE0,
        DataOverrun = 1 << DOR0,
        FrameError = 1 << FE0,
        DataRegisterEmpty = 1 << UDRE0,
        TransmitComplete = 1 << TXC0,
        ReceiveComplete = 1 << RXC0,
    };

    class UART
    {
    public:
        static UART the;

        void init(uint32_t baudrate) const;

        uint8_t getc(void);
        void putc(const uint8_t data);
        void puts(const char *s);
        void puts_p(const char *s);

        friend void UartReceiveInterrupt();
        friend void UartTransmitInterrupt();

    protected:
    private:
        UART(){};
        UART(UART &other) = delete;
        volatile uint8_t status = {};
        volatile Buffer<char, 32> rx_buffer = Buffer<char, 32>();
        volatile Buffer<char, 32> tx_buffer = Buffer<char, 32>();
        void operator=(const UART &) = delete;
    };
}
