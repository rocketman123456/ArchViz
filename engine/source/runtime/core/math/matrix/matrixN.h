#pragma once
#include "runtime/core/base/macro.h"
#include "runtime/core/math/vector/vectorN.h"

namespace ArchViz
{
    REFLECTION_TYPE(MatrixMxN)
    CLASS(MatrixMxN, Fields)
    {
        REFLECTION_BODY(MatrixMxN);

    public:
        std::vector<VectorN> m_rows {};

        MatrixMxN()  = default;
        ~MatrixMxN() = default;

        MatrixMxN(std::size_t M, std::size_t N)
        {
            m_rows.resize(M);
            for (auto& row : m_rows)
            {
                row.m_data.resize(N);
            }
        }

        MatrixMxN(const MatrixMxN& rhs) { *this = rhs; }

        std::size_t m() const { return m_rows.size(); }

        std::size_t n() const
        {
            if (m_rows.size() == 0)
                return 0;
            else
                return m_rows[0].m_data.size();
        }

        float* operator[](size_t row_index)
        {
            assert(row_index < m());
            return m_rows[row_index].ptr();
        }

        const float* operator[](size_t row_index) const
        {
            assert(row_index < m());
            return m_rows[row_index].ptr();
        }

        const MatrixMxN& operator=(const MatrixMxN& rhs)
        {
            m_rows.clear();
            m_rows.resize(rhs.m());
            for (int m = 0; m < rhs.m(); m++)
            {
                m_rows[m] = rhs.m_rows[m];
            }
            return *this;
        }

        const MatrixMxN& operator*=(float rhs)
        {
            for (int i = 0; i < m(); i++)
            {
                m_rows[i] *= rhs;
            }
            return *this;
        }

        friend MatrixMxN operator*(float scalar, const MatrixMxN& rhs)
        {
            MatrixMxN prod(rhs.m(), rhs.n());
            for (size_t row_index = 0; row_index < rhs.m(); row_index++)
            {
                for (size_t col_index = 0; col_index < rhs.n(); col_index++)
                {
                    prod[row_index][col_index] = scalar * rhs.m_rows[row_index][col_index];
                }
            }
            return prod;
        }

        VectorN operator*(const VectorN& rhs) const
        {
            // Check that the incoming vector is of the correct dimension
            if (rhs.m_data.size() != n())
            {
                LOG_WARN("matrix dimension mismatch with vector");
                return rhs;
            }

            VectorN tmp(m());
            for (int i = 0; i < m(); i++)
            {
                tmp[i] = rhs.dotProduct(m_rows[i]);
            }
            return tmp;
        }

        MatrixMxN operator*(const MatrixMxN& rhs) const
        {
            // Check that the incoming matrix of the correct dimension
            if (rhs.m() != n() && rhs.n() != m())
            {
                LOG_WARN("matrix dimension mismatch with matrix");
                return rhs;
            }

            MatrixMxN tranposedRHS = rhs.transpose();
            MatrixMxN tmp(m(), rhs.n());
            for (int i = 0; i < m(); i++)
            {
                for (int j = 0; j < rhs.n(); j++)
                {
                    tmp.m_rows[i][j] = m_rows[i].dotProduct(tranposedRHS.m_rows[j]);
                }
            }
            return tmp;
        }

        MatrixMxN operator*(const float rhs) const
        {
            MatrixMxN tmp = *this;
            for (int i = 0; i < m(); i++)
            {
                for (int j = 0; j < n(); j++)
                {
                    tmp.m_rows[i][j] *= rhs;
                }
            }
            return tmp;
        }

        void zero()
        {
            for (auto& row : m_rows)
            {
                row.zero();
            }
        }

        MatrixMxN transpose() const
        {
            MatrixMxN tmp(n(), m());
            for (int i = 0; i < m(); i++)
            {
                for (int j = 0; j < n(); j++)
                {
                    tmp.m_rows[j][i] = m_rows[i][j];
                }
            }
            return tmp;
        }
    };
} // namespace ArchViz
