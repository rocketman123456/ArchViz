#include "runtime/core/container/rb_tree.h"

#include <iostream>

namespace ArchViz
{
    void rb_tree::init(Node* nil)
    {
        m_nil = nil;
        m_nil->set_color(NodeColor::BLACK);
        m_nil->m_left  = m_nil;
        m_nil->m_right = m_nil;
        m_nil->m_value = 0;
        m_root         = m_nil;
    }

    rb_tree::Node* rb_tree::search(const std::size_t v)
    {
        Node* x = m_root;
        while (x != m_nil)
        {
            if (v == x->m_value)
                break;
            else if (v < x->m_value)
                x = x->m_left;
            else
                x = x->m_right;
        }
        return x;
    }

    rb_tree::Node* rb_tree::search_best(const std::size_t v)
    {
        Node* y = nullptr;
        Node* x = m_root;
        while (x != m_nil)
        {
            y = x;
            if (v == x->m_value)
                break;
            else if (v < x->m_value)
                x = x->m_left;
            else
                x = x->m_right;
        }
        while (y != nullptr && v > y->m_value)
            y = y->get_parent();
        return y;
    }

    rb_tree::Node* rb_tree::search_at_least(const std::size_t v)
    {
        Node* x = m_root;
        while (x != m_nil)
        {
            if (v <= x->m_value)
                return x;
            else
                x = x->m_right;
        }
        return nullptr;
    }

    void rb_tree::insert(Node* z)
    {
        Node* y = nullptr;
        Node* x = m_root;
        while (x != m_nil)
        {
            y = x;
            if (z->m_value < x->m_value)
                x = x->m_left;
            else
                x = x->m_right;
        }

        z->set_parent(y);
        if (y == nullptr)
            m_root = z;
        else if (z->m_value < y->m_value)
            y->m_left = z;
        else
            y->m_right = z;

        z->m_left  = m_nil;
        z->m_right = m_nil;
        z->set_color(NodeColor::RED);

        insert_fixup(z);
    }

    void rb_tree::insert_fixup(Node* z)
    {
        while (z != m_root && z->get_parent()->get_color() == NodeColor::RED)
        {
            if (z->get_parent() == z->get_parent()->get_parent()->m_left)
            {
                Node* y = z->get_parent()->get_parent()->m_right;
                if (y->get_color() == NodeColor::RED)
                {
                    z->get_parent()->set_color(NodeColor::BLACK);
                    y->set_color(NodeColor::BLACK);
                    z->get_parent()->get_parent()->set_color(NodeColor::RED);
                    z = z->get_parent()->get_parent();
                }
                else
                {
                    if (z == z->get_parent()->m_right)
                    {
                        z = z->get_parent();
                        left_rotate(z);
                    }
                    z->get_parent()->set_color(NodeColor::BLACK);
                    z->get_parent()->get_parent()->set_color(NodeColor::RED);
                    right_rotate(z->get_parent()->get_parent());
                }
            }
            else
            {
                Node* y = z->get_parent()->get_parent()->m_left;
                if (y->get_color() == NodeColor::RED)
                {
                    z->get_parent()->set_color(NodeColor::BLACK);
                    y->set_color(NodeColor::BLACK);
                    z->get_parent()->get_parent()->set_color(NodeColor::RED);
                    z = z->get_parent()->get_parent();
                }
                else
                {
                    if (z == z->get_parent()->m_left)
                    {
                        z = z->get_parent();
                        right_rotate(z);
                    }
                    z->get_parent()->set_color(NodeColor::BLACK);
                    z->get_parent()->get_parent()->set_color(NodeColor::RED);
                    left_rotate(z->get_parent()->get_parent());
                }
            }
        }
        m_root->set_color(NodeColor::BLACK);
        m_root->m_prev_size = 0;
    }

    void rb_tree::remove(Node* z)
    {
        Node*     x              = nullptr;
        Node*     y              = z;
        NodeColor yOriginalColor = y->get_color();
        if (z->m_left == m_nil)
        {
            x = z->m_right;
            transplant(z, z->m_right);
        }
        else if (z->m_right == m_nil)
        {
            x = z->m_left;
            transplant(z, z->m_left);
        }
        else
        {
            y              = successor(z);
            yOriginalColor = y->get_color();
            x              = y->m_right;
            if (y->get_parent() == z)
            {
                x->set_parent(y);
            }
            else
            {
                transplant(y, y->m_right);
                y->m_right = z->m_right;
                if (y->m_right != m_nil)
                    y->m_right->set_parent(y);
            }
            transplant(z, y);
            y->m_left = z->m_left;
            if (y->m_left != m_nil)
                y->m_left->set_parent(y);
            y->set_color(z->get_color());
        }

        if (yOriginalColor == NodeColor::BLACK)
            remove_fixup(x);
    }

    void rb_tree::remove_fixup(Node* z)
    {
        while (z != m_root && z->get_color() == NodeColor::BLACK)
        {
            Node* w = nullptr;
            if (z->get_parent()->m_left == z)
            {
                w = z->get_parent()->m_right;
                if (w->get_color() == NodeColor::RED)
                {
                    w->set_color(NodeColor::BLACK);
                    z->get_parent()->set_color(NodeColor::RED);
                    left_rotate(z->get_parent());
                    w = z->get_parent()->m_right;
                }
                if ((w->m_right == m_nil || w->m_right->get_color() == NodeColor::BLACK) && (w->m_left == m_nil || w->m_left->get_color() == NodeColor::BLACK))
                {
                    w->set_color(NodeColor::RED);
                    z = z->get_parent();
                }
                else
                {
                    if (w->m_right == m_nil || w->m_right->get_color() == NodeColor::BLACK)
                    {
                        w->m_left->set_color(NodeColor::BLACK);
                        w->set_color(NodeColor::RED);
                        right_rotate(w);
                        w = z->get_parent()->m_right;
                    }
                    w->set_color(z->get_parent()->get_color());
                    z->get_parent()->set_color(NodeColor::BLACK);
                    w->m_right->set_color(NodeColor::BLACK);
                    left_rotate(z->get_parent());
                    z = m_root;
                }
            }
            else
            {
                w = z->get_parent()->m_left;
                if (w->get_color() == NodeColor::RED)
                {
                    w->set_color(NodeColor::BLACK);
                    z->get_parent()->set_color(NodeColor::RED);
                    right_rotate(z->get_parent());
                    w = z->get_parent()->m_left;
                }
                if ((w->m_right == m_nil || w->m_right->get_color() == NodeColor::BLACK) && (w->m_left == m_nil || w->m_left->get_color() == NodeColor::BLACK))
                {
                    w->set_color(NodeColor::RED);
                    z = z->get_parent();
                }
                else
                {
                    if (w->m_left == m_nil || w->m_left->get_color() == NodeColor::BLACK)
                    {
                        w->m_right->set_color(NodeColor::BLACK);
                        w->set_color(NodeColor::RED);
                        left_rotate(w);
                        w = z->get_parent()->m_left;
                    }
                    w->set_color(z->get_parent()->get_color());
                    z->get_parent()->set_color(NodeColor::BLACK);
                    w->m_left->set_color(NodeColor::BLACK);
                    right_rotate(z->get_parent());
                    z = m_root;
                }
            }
        }
        z->set_color(NodeColor::BLACK);
    }

    rb_tree::Node* rb_tree::successor(Node* x)
    {
        x = x->m_right;
        while (x->m_left != m_nil)
        {
            x = x->m_left;
        }
        return x;
    }

    void rb_tree::print() const
    {
        if (m_root->m_right != m_nil)
        {
            print(m_root->m_right, true, "");
        }
        std::cout << m_root->m_value << '\n';
        if (m_root->m_left != m_nil)
        {
            print(m_root->m_left, false, "");
        }
    }

    void rb_tree::print(Node* x, bool isRight, std::string indent) const
    {
        if (x->m_right != m_nil)
        {
            print(x->m_right, true, indent + (isRight ? "        " : " |      "));
        }
        std::cout << indent;
        if (isRight)
        {
            std::cout << " /";
        }
        else
        {
            std::cout << " \\";
        }
        std::cout << "----- ";
        std::cout << x->m_value << '\n';
        if (x->m_left != m_nil)
        {
            print(x->m_left, false, indent + (isRight ? " |      " : "        "));
        }
    }

    void rb_tree::transplant(Node* u, Node* v)
    {
        Node* uParent = u->get_parent();
        if (uParent == nullptr)
            m_root = v;
        else if (u == uParent->m_left)
            uParent->m_left = v;
        else
            uParent->m_right = v;
        v->set_parent(uParent);
    }

    void rb_tree::left_rotate(Node* x)
    {
        Node* xParent = x->get_parent();
        Node* y       = x->m_right;
        x->m_right    = y->m_left;
        if (y->m_left != m_nil)
            y->m_left->set_parent(x);
        y->set_parent(xParent);
        if (xParent == nullptr)
            m_root = y;
        else if (xParent->m_left == x)
            xParent->m_left = y;
        else
            xParent->m_right = y;
        y->m_left = x;
        x->set_parent(y);
    }

    void rb_tree::right_rotate(Node* x)
    {
        Node* xParent = x->get_parent();
        Node* y       = x->m_left;
        x->m_left     = y->m_right;
        if (y->m_right != m_nil)
            y->m_right->set_parent(x);
        y->set_parent(xParent);
        if (xParent == nullptr)
            m_root = y;
        else if (xParent->m_left == x)
            xParent->m_left = y;
        else
            xParent->m_right = y;
        y->m_right = x;
        x->set_parent(y);
    }
}