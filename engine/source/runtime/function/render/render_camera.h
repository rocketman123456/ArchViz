#pragma once

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

        FVector3 m_position {0, 0, 0};
        FVector3 m_front {1, 0, 0};
        FVector3 m_up {0, 0, 1};
        FVector3 m_right {0, 1, 0};
        FVector3 m_world_up {0, 0, 1};

        float m_speed {0.1};
        float m_sensitivity {0.5};

        float m_yaw;
        float m_pitch;

        FQuaternion m_rotation;

        FMatrix4 m_view;
        FMatrix4 m_projction;

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
