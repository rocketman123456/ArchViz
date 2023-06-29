#pragma once
#include "runtime/core/math/matrix/matrixN.h"
#include "runtime/core/math/vector/vectorN.h"

namespace ArchViz
{
    // b = A * x, solve x
    VectorN lcp_gauss_seidel(const MatrixMxN& A, const VectorN& b);
} // namespace ArchViz