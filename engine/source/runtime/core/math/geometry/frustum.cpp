#include "runtime/core/math/geometry/frustum.h"

namespace ArchViz
{
    void Frustum::update(const FMatrix4& matrix)
    {
        planes[LEFT][0] = matrix(0, 3) + matrix(0, 0);
        planes[LEFT][1] = matrix(1, 3) + matrix(1, 0);
        planes[LEFT][2] = matrix(2, 3) + matrix(2, 0);
        planes[LEFT][3] = matrix(3, 3) + matrix(3, 0);

        planes[RIGHT][0] = matrix(0, 3) - matrix(0, 0);
        planes[RIGHT][1] = matrix(1, 3) - matrix(1, 0);
        planes[RIGHT][2] = matrix(2, 3) - matrix(2, 0);
        planes[RIGHT][3] = matrix(3, 3) - matrix(3, 0);

        planes[TOP][0] = matrix(0, 3) - matrix(0, 1);
        planes[TOP][1] = matrix(1, 3) - matrix(1, 1);
        planes[TOP][2] = matrix(2, 3) - matrix(2, 1);
        planes[TOP][3] = matrix(3, 3) - matrix(3, 1);

        planes[BOTTOM][0] = matrix(0, 3) + matrix(0, 1);
        planes[BOTTOM][1] = matrix(1, 3) + matrix(1, 1);
        planes[BOTTOM][2] = matrix(2, 3) + matrix(2, 1);
        planes[BOTTOM][3] = matrix(3, 3) + matrix(3, 1);

        planes[BACK][0] = matrix(0, 3) + matrix(0, 2);
        planes[BACK][1] = matrix(1, 3) + matrix(1, 2);
        planes[BACK][2] = matrix(2, 3) + matrix(2, 2);
        planes[BACK][3] = matrix(3, 3) + matrix(3, 2);

        planes[FRONT][0] = matrix(0, 3) - matrix(0, 2);
        planes[FRONT][1] = matrix(1, 3) - matrix(1, 2);
        planes[FRONT][2] = matrix(2, 3) - matrix(2, 2);
        planes[FRONT][3] = matrix(3, 3) - matrix(3, 2);

        for (auto i = 0; i < planes.size(); i++)
        {
            float length = sqrtf(planes[i][0] * planes[i][0] + planes[i][1] * planes[i][1] + planes[i][2] * planes[i][2]);
            planes[i] /= length;
        }
    }

    bool Frustum::checkSphere(const FVector3& pos, float radius)
    {
        for (auto i = 0; i < planes.size(); i++)
        {
            if ((planes[i][0] * pos[0]) + (planes[i][1] * pos[1]) + (planes[i][2] * pos[2]) + planes[i][3] <= -radius)
            {
                return false;
            }
        }
        return true;
    }
} // namespace ArchViz
