#pragma once
#include "runtime/core/math/math.h"
#include "runtime/core/math/vector/vector2.h"

#include <vector>

namespace ArchViz
{
    REFLECTION_TYPE(Matrix2x2)
    CLASS(Matrix2x2, Fields)
    {
        REFLECTION_BODY(Matrix2x2);

    public:
        std::vector<Vector2> m_rows {Vector2::ZERO, Vector2::ZERO};

        Matrix2x2() { operator=(IDENTITY); }

        explicit Matrix2x2(float arr[2][2])
        {
            memcpy(m_rows[0].ptr(), arr[0], 2 * sizeof(float));
            memcpy(m_rows[1].ptr(), arr[1], 2 * sizeof(float));
        }

        Matrix2x2(float(&float_array)[4])
        {
            m_rows[0][0] = float_array[0];
            m_rows[0][1] = float_array[1];
            m_rows[1][0] = float_array[2];
            m_rows[1][1] = float_array[3];
        }

        Matrix2x2(float entry00, float entry01, float entry10, float entry11)
        {
            m_rows[0][0] = entry00;
            m_rows[0][1] = entry01;
            m_rows[1][0] = entry10;
            m_rows[1][1] = entry11;
        }

        Matrix2x2(const Vector2& row0, const Vector2& row1)
        {
            m_rows[0] = row0;
            m_rows[1] = row1;
        }

        Matrix2x2(const Matrix2x2& rhs)
        {
            m_rows[0] = rhs.m_rows[0];
            m_rows[1] = rhs.m_rows[1];
        }

        float* operator[](size_t row_index) { return m_rows[row_index].ptr(); }

        const float* operator[](size_t row_index) const { return m_rows[row_index].ptr(); }

        Matrix2x2& operator=(const Matrix2x2& rhs)
        {
            m_rows[0] = rhs.m_rows[0];
            m_rows[1] = rhs.m_rows[1];
            return *this;
        }

        Matrix2x2 operator+(const Matrix2x2& rhs) const
        {
            Matrix2x2 diff;
            for (size_t row_index = 0; row_index < 2; row_index++)
            {
                diff.m_rows[row_index] = m_rows[row_index] + rhs.m_rows[row_index];
            }
            return diff;
        }

        Matrix2x2 operator-(const Matrix2x2& rhs) const
        {
            Matrix2x2 diff;
            for (size_t row_index = 0; row_index < 2; row_index++)
            {
                diff.m_rows[row_index] = m_rows[row_index] - rhs.m_rows[row_index];
            }
            return diff;
        }

        const Matrix2x2& operator*=(const float rhs)
        {
            m_rows[0] *= rhs;
            m_rows[1] *= rhs;
            return *this;
        }

        const Matrix2x2& operator+=(const Matrix2x2& rhs)
        {
            m_rows[0] += rhs.m_rows[0];
            m_rows[1] += rhs.m_rows[1];
            return *this;
        }

        // matrix * scalar
        Matrix2x2 operator*(float scalar) const
        {
            Matrix2x2 prod;
            for (size_t row_index = 0; row_index < 2; row_index++)
            {
                for (size_t col_index = 0; col_index < 2; col_index++)
                {
                    prod[row_index][col_index] = scalar * m_rows[row_index][col_index];
                }
            }
            return prod;
        }

        Vector2 operator*(const Vector2& rhs) const
        {
            Vector2 prod;
            for (size_t row_index = 0; row_index < 2; row_index++)
            {
                prod[row_index] = m_rows[row_index][0] * rhs.x + m_rows[row_index][1] * rhs.y;
            }
            return prod;
        }

        Matrix2x2 operator*(const Matrix2x2& rhs) const
        {
            Matrix2x2 prod;
            for (size_t row_index = 0; row_index < 2; row_index++)
            {
                for (size_t col_index = 0; col_index < 2; col_index++)
                {
                    prod.m_rows[row_index][col_index] = m_rows[row_index][0] * rhs.m_rows[0][col_index] + m_rows[row_index][1] * rhs.m_rows[1][col_index];
                }
            }
            return prod;
        }

        Matrix2x2 transpose() const
        {
            Matrix2x2 transpose_v;
            for (size_t row_index = 0; row_index < 2; row_index++)
            {
                for (size_t col_index = 0; col_index < 2; col_index++)
                    transpose_v[row_index][col_index] = m_rows[col_index][row_index];
            }
            return transpose_v;
        }

        static const Matrix2x2 ZERO;
        static const Matrix2x2 IDENTITY;
    };

    Matrix2x2 operator*(float scalar, const Matrix2x2& rhs);

    Vector2 operator*(const Vector2& point, const Matrix2x2& rhs);
} // namespace ArchViz