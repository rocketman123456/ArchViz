#include "runtime/function/render/render_camera.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/math/graphics_utils.h"

#include <cmath>

namespace ArchViz
{
    void RenderCamera::setPerspective(float fov, float ratio, float znear, float zfar)
    {
        m_fov   = fov;
        m_ratio = ratio;
        m_znear = znear;
        m_zfar  = zfar;
    }

    void RenderCamera::setOrthogonal(float left, float right, float bottom, float top, float znear, float zfar)
    {
        m_left   = left;
        m_right_ = right;
        m_bottom = bottom;
        m_top    = top;
        m_znear  = znear;
        m_zfar   = zfar;
    }

    void RenderCamera::move(CameraMovement direction, float dt)
    {
        float velocity = m_speed * dt;
        if (direction == CameraMovement::Forward)
            m_position += m_front * velocity;
        if (direction == CameraMovement::Backward)
            m_position -= m_front * velocity;
        if (direction == CameraMovement::Left)
            m_position -= m_right * velocity;
        if (direction == CameraMovement::Right)
            m_position += m_right * velocity;
    }

    void RenderCamera::rotate(float dx, float dy)
    {
        float xoffset = dx * m_sensitivity;
        float yoffset = dy * m_sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        bool constrain_pitch = true;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrain_pitch)
        {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    void RenderCamera::updateCameraVectors()
    {
        // calculate the new Front vector
        FVector3 front;
        front[0] = cos(Degree(m_yaw).valueRadians()) * cos(Degree(m_pitch).valueRadians());
        front[1] = sin(Degree(m_pitch).valueRadians());
        front[2] = sin(Degree(m_yaw).valueRadians()) * cos(Degree(m_pitch).valueRadians());

        m_front = front.normalized();
        // also re-calculate the Right and Up vector
        m_right = m_front.cross(m_world_up).normalized();
        m_up    = m_right.cross(m_front).normalized();
    }

    void RenderCamera::update()
    {
        std::lock_guard<std::mutex> lock_guard(m_view_matrix_mutex);

        switch (m_type)
        {
            case RenderCameraType::Perspective:
                updatePerspective();
                break;
            case RenderCameraType::Orthogonal:
                updateOrthogonal();
                break;
            default:
                break;
        }
        updateView();
    }

    void RenderCamera::updatePerspective() { m_projction = GraphicsUtils::perspective(m_fov, m_ratio, m_znear, m_zfar); }

    void RenderCamera::updateOrthogonal() { m_projction = GraphicsUtils::orthogonal(m_left, m_right_, m_bottom, m_top, m_znear, m_zfar); }

    void RenderCamera::updateView() { m_view = GraphicsUtils::lookAt(m_position, m_position + m_front, m_up); }

} // namespace ArchViz
