/*
 * Copyright (C) 2013-2015 Sergey Kosarevsky (sk@linderdaum.com)
 * Copyright (C) 2013-2015 Viktor Latypov (vl@linderdaum.com)
 * Based on Linderdaum Engine http://www.linderdaum.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must display the names 'Sergey Kosarevsky' and
 *    'Viktor Latypov' in the credits of the application, if such credits exist.
 *    The authors of this work must be notified via email (sk@linderdaum.com) in
 *    this case of redistribution.
 *
 * 3. Neither the name of copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "runtime/core/math/math_type.h"

#include <numeric>

namespace ArchViz
{
    /// Virtual trackball for user interaction with rotations
    class VirtualTrackball
    {
    public:
        VirtualTrackball() = default;

        /**
            Get rotation matrix for new mouse point
        **/
        FMatrix4 dragTo(FVector2 screenPoint, float speed, bool keyPressed)
        {
            if (keyPressed && !m_is_dragging_active)
            {
                startDragging(screenPoint);

                m_is_dragging_active = keyPressed;

                return FMatrix4::Identity();
            }

            m_is_dragging_active = keyPressed;

            if (!keyPressed)
                return FMatrix4::Identity();

            m_point_cur = projectOnSphere(screenPoint);

            const FVector3 direction = m_point_cur - m_point_prev;
            const float    shift     = direction.norm();

            FMatrix4 rot_matrix = FMatrix4::Identity();

            if (shift > std::numeric_limits<float>::epsilon())
            {
                const FVector3 axis          = m_point_prev.cross(m_point_cur);
                rot_matrix.block<3, 3>(0, 0) = Eigen::AngleAxisf(shift * speed, axis).matrix();
            }

            m_rotation_delta = rot_matrix;
            return rot_matrix;
        }

        const FMatrix4& getRotationDelta() const { return m_rotation_delta; };

        /**
            Get current rotation matrix
        **/
        FMatrix4 getRotationMatrix() const { return m_rotation * m_rotation_delta; }

    private:
        void startDragging(const FVector2& screen_point)
        {
            m_rotation       = m_rotation * m_rotation_delta;
            m_rotation_delta = FMatrix4::Identity();
            m_point_cur      = projectOnSphere(screen_point);
            m_point_prev     = m_point_cur;
        }

        FVector3 projectOnSphere(const FVector2& screen_point)
        {
            // convert to -1.0...1.0 range
            FVector3 proj(+(2.0f * screen_point[0] - 1.0f), -(2.0f * screen_point[1] - 1.0f), 0.0f);

            const float Length = std::min(proj.norm(), 1.0f);

            proj[2] = sqrtf(1.001f - Length * Length);

            return proj.normalized();
        }

    private:
        FVector3 m_point_cur          = FVector3::Zero();
        FVector3 m_point_prev         = FVector3::Zero();
        FMatrix4 m_rotation           = FMatrix4::Identity();
        FMatrix4 m_rotation_delta     = FMatrix4::Identity();
        bool     m_is_dragging_active = false;
    };
} // namespace ArchViz
