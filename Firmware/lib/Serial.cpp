#include "Serial.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <avr/pgmspace.h>

Serial::UART Serial::UART::the = {};

ISR(USART_RX_vect)
{
    Serial::UartReceiveInterrupt();
}

ISR(USART_UDRE_vect)
{
    Serial::UartTransmitInterrupt();
}

void Serial::UartReceiveInterrupt()
{
    Serial::UART::the.status = UCSR0A;

    auto result = Serial::UART::the.rx_buffer.write(UDR0);
    if (result == BufferResult::Overflow)
    {
        Serial::UART::the.status |= Serial::UARTStatus::BufferOverflow;
    }
}

void Serial::UartTransmitInterrupt()
{
    if (Serial::UART::the.tx_buffer.available())
    {
        // snd next byte
        UDR0 = Serial::UART::the.tx_buffer.read();
    }
    else
    {
        // no more data to send
        UCSR0B &= ~(1 << UDRIE0);
    }
}

void Serial::UART::init(uint32_t baudrate, Direction direction) const
{
    // Set baud rate
    uint16_t ubrr = (F_CPU / 8 / baudrate - 1) / 2;
    UBRR0L = ubrr;
    UBRR0H = ubrr >> 8;

    // Enable receiver and transmitter with interrupts
    UCSR0B = direction;

    // Set frame format: 8data, 1stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Enable global interrupts
    sei();
}

uint8_t Serial::UART::getc(void)
{
    while (!rx_buffer.available())
        ;
    return rx_buffer.read();
}

void Serial::UART::putc(const uint8_t data)
{
    while (tx_buffer.full())
        ;
    tx_buffer.write(data);
    UCSR0B |= (1 << UDRIE0);
}

void Serial::UART::puts(const char *s)
{
    while (*s)
    {
        putc(*s++);
    }
}

void Serial::UART::puts_p(const char *s)
{
    while (pgm_read_byte(s))
    {
        putc(pgm_read_byte(s++));
    }
}
