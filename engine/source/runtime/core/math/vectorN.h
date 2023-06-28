#pragma once
#include "runtime/core/math/math.h"
#include "runtime/core/meta/reflection/reflection.h"

#include <cassert>
#include <cstdlib>
#include <vector>

namespace ArchViz
{
    REFLECTION_TYPE(VectorN)
    CLASS(VectorN, Fields)
    {
        REFLECTION_BODY(VectorN);

    public:
        std::vector<float> m_data;

    public:
        VectorN() : m_data {} {}

        VectorN(std::size_t N) { m_data.resize(N); }

        VectorN(const VectorN& rhs)
        {
            m_data.resize(rhs.m_data.size());
            std::memcpy(m_data.data(), rhs.m_data.data(), sizeof(float) * m_data.size());
        }

        VectorN& operator=(const VectorN& rhs)
        {
            m_data.clear();
            m_data.resize(rhs.m_data.size());
            std::memcpy(m_data.data(), rhs.m_data.data(), sizeof(float) * m_data.size());
            return *this;
        }

        ~VectorN() = default;

        std::size_t n() const { return m_data.size(); }

        float operator[](const std::size_t idx) const { return m_data[idx]; }

        float& operator[](const std::size_t idx) { return m_data[idx]; }

        /// Pointer accessor for direct copying
        float* ptr() { return m_data.data(); }
        /// Pointer accessor for direct copying
        const float* ptr() const { return m_data.data(); }

        const VectorN& operator*=(float rhs)
        {
            for (int i = 0; i < m_data.size(); i++)
            {
                m_data[i] *= rhs;
            }
            return *this;
        }

        VectorN operator*(float rhs) const
        {
            VectorN tmp = *this;
            tmp *= rhs;
            return tmp;
        }

        VectorN operator+(const VectorN& rhs) const
        {
            VectorN tmp = *this;
            for (int i = 0; i < tmp.m_data.size(); i++)
            {
                tmp.m_data[i] += rhs.m_data[i];
            }
            return tmp;
        }

        VectorN operator-(const VectorN& rhs) const
        {
            VectorN tmp = *this;
            for (int i = 0; i < tmp.m_data.size(); i++)
            {
                tmp.m_data[i] -= rhs.m_data[i];
            }
            return tmp;
        }

        const VectorN& operator+=(const VectorN& rhs)
        {
            for (int i = 0; i < m_data.size(); i++)
            {
                m_data[i] += rhs.m_data[i];
            }
            return *this;
        }

        const VectorN& operator-=(const VectorN& rhs)
        {
            for (int i = 0; i < m_data.size(); i++)
            {
                m_data[i] -= rhs.m_data[i];
            }
            return *this;
        }

        float dotProduct(const VectorN& rhs) const
        {
            float sum = 0;
            for (int i = 0; i < m_data.size(); i++)
            {
                sum += m_data[i] * rhs.m_data[i];
            }
            return sum;
        }

        void zero()
        {
            for (int i = 0; i < m_data.size(); i++)
            {
                m_data[i] = 0.0f;
            }
        }

        bool isValid() const
        {
            for (int i = 0; i < m_data.size(); ++i)
            {
                if (m_data[i] * 0.0f != m_data[i] * 0.0f)
                {
                    return false;
                }
            }
            return true;
        }

        bool isNaN() const
        {
            for (auto x : m_data)
            {
                if (Math::isNan(x))
                    return true;
            }
            return false;
        }
    };
} // namespace ArchViz
