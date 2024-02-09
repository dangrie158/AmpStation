#pragma once
#include <inttypes.h>

enum class BufferResult
{
    Ok,
    Overflow,
};

template <typename T, uint8_t size>
class Buffer
{
public:
    T read() volatile
    {
        if (!available())
        {
            return 0;
        }
        T value = m_buffer[read_head];
        read_head = (read_head + 1) % size;
        return value;
    }

    BufferResult write(T value) volatile
    {
        if (full())
        {
            return BufferResult::Overflow;
        }
        m_buffer[write_head] = value;
        write_head = (write_head + 1) % size;
        return BufferResult::Ok;
    }

    bool full() volatile
    {
        return (write_head + 1) % size == read_head;
    }

    bool available() volatile
    {
        return write_head != read_head;
    }

private:
    volatile T m_buffer[size];
    volatile uint8_t write_head = 0;
    volatile uint8_t read_head = 0;
};
