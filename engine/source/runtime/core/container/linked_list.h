#pragma once

#include <cstddef>

namespace ArchViz
{
    class linked_list
    {
    public:
        struct Node
        {
            Node*       m_next;
            std::size_t m_value;
        };
        Node* m_head;

    public:
        void search_first(const std::size_t v, Node*& curr, Node*& prev);
        void search_best(const std::size_t v, Node*& curr, Node*& prev);
    };
} // namespace Piccolo
