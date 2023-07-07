#include "runtime/core/math/geometry/axis_aligned.h"

namespace ArchViz
{
    AxisAlignedBox::AxisAlignedBox(const Vector3& center, const Vector3& half_extent) { update(center, half_extent); }

    void AxisAlignedBox::merge(const Vector3& new_point)
    {
        m_min_corner.makeFloor(new_point);
        m_max_corner.makeCeil(new_point);

        m_center      = 0.5f * (m_min_corner + m_max_corner);
        m_half_extent = m_center - m_min_corner;
    }

    void AxisAlignedBox::merge(const AxisAlignedBox& new_box)
    {
        merge(new_box.m_min_corner);
        merge(new_box.m_max_corner);
    }

    void AxisAlignedBox::update(const Vector3& center, const Vector3& half_extent)
    {
        m_center      = center;
        m_half_extent = half_extent;
        m_min_corner  = center - half_extent;
        m_max_corner  = center + half_extent;
    }

    bool AxisAlignedBox::intersect(const AxisAlignedBox& rhs) const
    {
        if (m_max_corner.x < rhs.m_min_corner.x || m_max_corner.y < rhs.m_min_corner.y || m_max_corner.z < rhs.m_min_corner.z)
        {
            return false;
        }
        if (rhs.m_max_corner.x < m_min_corner.x || rhs.m_max_corner.y < m_min_corner.y || rhs.m_max_corner.z < m_min_corner.z)
        {
            return false;
        }
        return true;
    }

} // namespace ArchViz