#pragma once

#define NOMINMAX
#include <soloud.h>
#include <soloud_wav.h>

namespace ArchViz
{
    struct AudioData
    {
        SoLoud::Wav wave;
    };
} // namespace ArchViz
