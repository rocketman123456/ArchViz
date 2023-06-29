#include "runtime/core/math/matrix/matrix2.h"

namespace ArchViz
{
    const Matrix2x2 Matrix2x2::ZERO({{0, 0}, {0, 0}});
    const Matrix2x2 Matrix2x2::IDENTITY({{1, 0}, {0, 1}});

    Matrix2x2 operator*(float scalar, const Matrix2x2& rhs)
    {
        Matrix2x2 prod;
        for (size_t row_index = 0; row_index < 2; row_index++)
        {
            for (size_t col_index = 0; col_index < 2; col_index++)
            {
                prod[row_index][col_index] = scalar * rhs.m_rows[row_index][col_index];
            }
        }
        return prod;
    }

    Vector2 operator*(const Vector2& point, const Matrix2x2& rhs)
    {
        Vector2 prod;
        for (size_t row_index = 0; row_index < 2; row_index++)
        {
            prod[row_index] = point.x * rhs.m_rows[0][row_index] + point.y * rhs.m_rows[1][row_index];
        }
        return prod;
    }
} // namespace ArchViz