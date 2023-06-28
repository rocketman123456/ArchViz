#include "runtime/core/math/solver/lcp.h"

namespace ArchViz
{
    VectorN lcp_gauss_seidel(const MatrixMxN& A, const VectorN& b)
    {
        const size_t N = b.n();

        VectorN x(N);
        x.zero();

        for (int iter = 0; iter < N; iter++)
        {
            for (int i = 0; i < N; i++)
            {
                float dx = (b[i] - A.m_rows[i].dotProduct(x)) / A.m_rows[i][i];
                if (dx * 0.0f == dx * 0.0f)
                {
                    x[i] = x[i] + dx;
                }
            }
        }
        return x;
    }
} // namespace ArchViz
