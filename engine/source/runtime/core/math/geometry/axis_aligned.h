#pragma once
#include "runtime/core/math/vector/vector3.h"
#include "runtime/core/meta/reflection/reflection.h"
#include <limits>

namespace ArchViz
{
    REFLECTION_TYPE(AxisAlignedBox)
    CLASS(AxisAlignedBox, Fields)
    {
        REFLECTION_BODY(AxisAlignedBox)
    public:
        AxisAlignedBox() {}
        AxisAlignedBox(const Vector3& center, const Vector3& half_extent);

        void merge(const Vector3& new_point);
        void merge(const AxisAlignedBox& new_box);
        void update(const Vector3& center, const Vector3& half_extent);

        bool intersect(const AxisAlignedBox& rhs) const;

        const Vector3& getCenter() const { return m_center; }
        const Vector3& getHalfExtent() const { return m_half_extent; }
        const Vector3& getMinCorner() const { return m_min_corner; }
        const Vector3& getMaxCorner() const { return m_max_corner; }

        float widthX() const { return m_max_corner.x - m_min_corner.x; }
        float widthY() const { return m_max_corner.y - m_min_corner.y; }
        float widthZ() const { return m_max_corner.z - m_min_corner.z; }

    public:
        Vector3 m_center {Vector3::ZERO};
        Vector3 m_half_extent {Vector3::ZERO};

        Vector3 m_min_corner {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
        Vector3 m_max_corner {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};
    };
} // namespace ArchViz