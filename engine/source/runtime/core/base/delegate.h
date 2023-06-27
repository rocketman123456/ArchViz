// The MIT License(MIT)
// 
// Copyright(c) 2015 Stefan Reinalter
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#pragma once
#include <cassert>
#include <functional>

namespace ArchViz
{
    template<typename T>
    class Delegate
    {};

    template<typename R, typename... Args>
    class Delegate<R(Args...)>
    {
    private:
        typedef R (*ProxyFunction)(void*, Args...);

        template<R (*Function)(Args...)>
        static inline R functionProxy(void*, Args... args)
        {
            return Function(std::forward<Args>(args)...);
        }

        template<class C, R (C::*Function)(Args...)>
        static inline R methodProxy(void* instance, Args... args)
        {
            return (static_cast<C*>(instance)->*Function)(std::forward<Args>(args)...);
        }

        template<class C, R (C::*Function)(Args...) const>
        static inline R constMethodProxy(void* instance, Args... args)
        {
            return (static_cast<const C*>(instance)->*Function)(std::forward<Args>(args)...);
        }

    public:
        Delegate() : m_instance(nullptr), m_proxy(nullptr) {}
        Delegate(const Delegate<R(Args...)>& obj) : m_instance(obj.m_instance), m_proxy(obj.m_proxy) {}

        template<R (*Function)(Args...)>
        void bind(void)
        {
            m_instance = nullptr;
            m_proxy    = &functionProxy<Function>;
        }

        template<class C, R (C::*Function)(Args...)>
        void bind(C* instance)
        {
            m_instance = instance;
            m_proxy    = &methodProxy<C, Function>;
        }

        template<class C, R (C::*Function)(Args...) const>
        void bind(const C* instance)
        {
            m_instance = const_cast<C*>(instance);
            m_proxy    = &constMethodProxy<C, Function>;
        }

        R invoke(Args... args) const
        {
            assert((m_proxy != nullptr) && "Cannot invoke unbound Delegate. Call Bind() first.");
            return m_proxy(m_instance, std::forward<Args>(args)...);
        }

        R operator()(Args... args) { return invoke(std::forward<Args>(args)...); }

        bool operator==(Delegate& ps)
        {
            if (this->m_instance == ps.m_instance && this->m_proxy == ps.m_proxy)
            {
                return true;
            }
            return false;
        }

        bool operator==(const Delegate& ps) const
        {
            if (this->m_instance == ps.m_instance && this->m_proxy == ps.m_proxy)
            {
                return true;
            }
            return false;
        }

    private:
        void*         m_instance = nullptr;
        ProxyFunction m_proxy    = nullptr;
    };
} // namespace Piccolo
