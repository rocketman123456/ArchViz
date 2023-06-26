#pragma once
#include "runtime/core/math/matrixN.h"
#include "runtime/core/math/vectorN.h"

namespace Piccolo
{
    // b = A * x, solve x
    VectorN lcp_gauss_seidel(const MatrixMxN& A, const VectorN& b);
}