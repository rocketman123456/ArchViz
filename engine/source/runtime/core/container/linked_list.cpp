#include "runtime/core/container/linked_list.h"

namespace Piccolo
{
    void linked_list::search_first(const std::size_t v, Node*& curr, Node*& prev)
    {
        Node* prevX = nullptr;
        Node* x     = m_head;
        while (x != nullptr)
        {
            if (v <= x->m_value)
                break;
            prevX = x;
            x     = x->m_next;
        }
        prev = prevX;
        curr = x;
    }

    void linked_list::search_best(const std::size_t v, Node*& curr, Node*& prev)
    {
        Node* prevBest = nullptr;
        Node* best     = nullptr;
        Node* prevX    = nullptr;
        Node* x        = m_head;
        while (x != nullptr)
        {
            if (v == x->m_value)
            {
                prevBest = prevX;
                best     = x;
                break;
            }
            else if (x->m_value > v)
            {
                if (best == nullptr || x->m_value < best->m_value)
                {
                    prevBest = prevX;
                    best     = x;
                }
            }
            prevX = x;
            x     = x->m_next;
        }
        prev = prevBest;
        curr = best;
    }
}