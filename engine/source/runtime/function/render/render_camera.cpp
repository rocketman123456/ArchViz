#include "runtime/function/render/render_camera.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/math/math.h"

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
        m_left_edge   = left;
        m_right_edge  = right;
        m_bottom_edge = bottom;
        m_top_edge    = top;
        m_znear       = znear;
        m_zfar        = zfar;
    }

    void RenderCamera::setViewPort(float width, float height)
    {
        m_width  = width;
        m_height = height;
    }

    void RenderCamera::move(CameraMovement direction, float dt)
    {
        float velocity = m_move_speed * dt;
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
        m_rotation = m_rotation + FVector3({dy * m_rotate_speed, -dx * m_rotate_speed, 0.0f});

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    void RenderCamera::updateCameraVectors()
    {
        // calculate the new Front vector
        m_front[0] = -cos(Math::degreesToRadians(m_rotation[0]) * (m_filp_y ? -1.0f : 1.0f)) * sin(Math::degreesToRadians(m_rotation[1]));
        m_front[1] = sin(Math::degreesToRadians(m_rotation[0]));
        m_front[2] = cos(Math::degreesToRadians(m_rotation[0])) * cos(Math::degreesToRadians(m_rotation[1]));
        m_front.normalize();

        //// also re-calculate the Right and Up vector
        m_right = m_world_up.cross(m_front).normalized();
        m_up    = m_front.cross(m_right).normalized();
    }

    void RenderCamera::update()
    {
        std::lock_guard<std::mutex> lock_guard(m_view_matrix_mutex);

        updateView();
        updatePerspective();
    }

    void RenderCamera::updatePerspective() { m_projction = Math::perspective(m_fov, m_ratio, m_znear, m_zfar); }

    void RenderCamera::updateOrthogonal() { m_projction = Math::orthogonal(m_left_edge, m_right_edge, m_bottom_edge, m_top_edge, m_znear, m_zfar); }

    void RenderCamera::updateView()
    {
        // m_view = Math::lookAt(m_position, m_position + m_front, m_up);
        FMatrix4 rot_mat   = FMatrix4::Identity();
        FMatrix4 trans_mat = FMatrix4::Identity();

        float angle_x = Math::degreesToRadians(m_rotation[0]);
        float angle_y = Math::degreesToRadians(m_rotation[1]);
        float angle_z = Math::degreesToRadians(m_rotation[2]);

        rot_mat.block<3, 3>(0, 0) = rot_mat.block<3, 3>(0, 0) * Eigen::AngleAxisf(angle_x, FVector3({1.0f, 0.0f, 0.0f})).toRotationMatrix();
        rot_mat.block<3, 3>(0, 0) = rot_mat.block<3, 3>(0, 0) * Eigen::AngleAxisf(angle_y, FVector3({0.0f, 1.0f, 0.0f})).toRotationMatrix();
        rot_mat.block<3, 3>(0, 0) = rot_mat.block<3, 3>(0, 0) * Eigen::AngleAxisf(angle_z, FVector3({0.0f, 0.0f, 1.0f})).toRotationMatrix();

        FVector3 translation = m_position;
        if (m_filp_y)
        {
            translation[2] *= -1.0f;
        }
        trans_mat.block<3, 1>(0, 3) = translation;

        if (m_type == RenderCameraType::FirstPerson)
        {
            m_view = rot_mat * trans_mat;
        }
        else
        {
            m_view = trans_mat * rot_mat;
        }
    }

} // namespace ArchViz
