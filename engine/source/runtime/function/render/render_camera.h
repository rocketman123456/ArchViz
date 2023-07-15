#pragma once
#include "runtime/core/math/geometry/trackball.h"
#include "runtime/core/math/math_headers.h"

#include <mutex>

namespace ArchViz
{
    enum class RenderCameraType : uint32_t
    {
        Perspective,
        Orthogonal,
    };

    enum class CameraMovement : uint32_t
    {
        Forward,
        Backward,
        Left,
        Right
    };

    class RenderCamera
    {
    public:
        void setPerspective(float fov, float ratio, float znear, float zfar);
        void setOrthogonal(float left, float right, float bottom, float top, float znear, float zfar);
        void setViewPort(float width, float height);

        void update();

        void move(CameraMovement direction, float dt);
        void rotate(float dx, float dy);

    protected:
        void updatePerspective();
        void updateOrthogonal();

        void updateView();

        void updateCameraVectors();

    public:
        RenderCameraType m_type {RenderCameraType::Perspective};

        FVector3 m_position {FVector3::Zero()};
        FVector3 m_front {FVector3::Zero()};
        FVector3 m_up {FVector3::Zero()};
        FVector3 m_right {FVector3::Zero()};
        FVector3 m_world_up {FVector3::Zero()};

        float m_move_speed {1.};
        float m_mouse_speed {0.1};

        float m_zoom;
        float m_width;
        float m_height;

        float m_yaw;
        float m_pitch;

        FQuaternion m_rotation {FQuaternion::Identity()};

        FMatrix4 m_view {FMatrix4::Identity()};
        FMatrix4 m_projction {FMatrix4::Identity()};
        FMatrix4 m_view_projection {FMatrix4::Identity()};

    protected:
        float m_fov;
        float m_ratio;

        float m_znear;
        float m_zfar;

        float m_left;
        float m_right_;
        float m_bottom;
        float m_top;

    protected:
        std::mutex m_view_matrix_mutex;
    };

} // namespace ArchViz
