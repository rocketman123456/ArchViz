#pragma once

#include <cstddef>
#include <string>

namespace ArchViz
{
    class rb_tree
    {
    public:
        enum class NodeColor : std::size_t
        {
            BLACK,
            RED
        };
        class Node
        {
        public:
            std::size_t m_prev_size;

        private:
            Node* m_parent;

        public:
            Node*       m_right;
            Node*       m_left;
            std::size_t m_value;

            inline Node* get_parent_raw() { return m_parent; }

            inline Node* get_parent() { return reinterpret_cast<Node*>((std::size_t)m_parent >> 2 << 2); }

            inline void set_parent(Node* p) { m_parent = reinterpret_cast<Node*>((std::size_t)p | ((std::size_t)m_parent & 2) | 1); }

            inline NodeColor get_color()
            {
                if (m_parent == nullptr)
                    return NodeColor::BLACK;
                return (std::size_t)m_parent & 2 ? NodeColor::RED : NodeColor::BLACK;
            }

            inline void set_color(NodeColor color)
            {
                if (color == NodeColor::RED)
                    m_parent = reinterpret_cast<Node*>((std::size_t)m_parent | 2);
                else
                    m_parent = reinterpret_cast<Node*>((std::size_t)m_parent & ~((std::size_t)2));
            }
        };

    private:
        Node* m_nil;
        Node* m_root;

    public:
        void  init(Node* nil);
        Node* search(const std::size_t v);
        Node* search_best(const std::size_t v);
        Node* search_at_least(const std::size_t v);
        void  insert(Node* z);
        void  remove(Node* z);
        Node* successor(Node* x);
        void  print() const;

    private:
        void insert_fixup(Node* z);
        void remove_fixup(Node* z);
        void transplant(Node* u, Node* v);
        void left_rotate(Node* x);
        void right_rotate(Node* x);
        void print(Node* x, bool isRight, std::string indent) const;
    };
}