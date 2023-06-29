#pragma once

#include <assert.h>
#include <stdio.h>

namespace ArchViz
{
    class FPSCounter
    {
    public:
        explicit FPSCounter(float avg_interval = 0.5f) : m_avg_interval(avg_interval) { assert(m_avg_interval > 0.0f); }

        bool tick(float delta_seconds, bool frame_rendered = true)
        {
            if (frame_rendered)
                m_num_frames++;

            m_accumulated_time += delta_seconds;

            if (m_accumulated_time > m_avg_interval)
            {
                m_current_fps = static_cast<float>(m_num_frames / m_accumulated_time);
                if (m_print_fps)
                    printf("FPS: %.1f\n", m_current_fps);
                m_num_frames       = 0;
                m_accumulated_time = 0;
                return true;
            }

            return false;
        }

        inline float getFPS() const { return m_current_fps; }

        bool m_print_fps = true;

    private:
        const float  m_avg_interval     = 0.5f;
        unsigned int m_num_frames       = 0;
        double       m_accumulated_time = 0;
        float        m_current_fps      = 0.0f;
    };
} // namespace ArchViz
