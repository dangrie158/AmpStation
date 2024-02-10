#pragma once

#include <stdlib.h>

template <typename T>
class List
{
    struct Node
    {
        Node *next = nullptr;
        T data;
    };

public:
    void push(T value)
    {
        struct Node *new_node = (Node *)malloc(sizeof(struct Node));
        new_node->data = value;
        new_node->next = m_head;
        m_head = new_node;
    }

    void for_each(void (*callback)(T))
    {
        for (Node *current = m_head; current; current = current->next)
        {
            callback(current->data);
        }
    }

private:
    Node *m_head = nullptr;
};
