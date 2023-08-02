#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include <string>

namespace ArchViz
{
    REFLECTION_TYPE(AudioRes)
    CLASS(AudioRes, Fields)
    {
        REFLECTION_BODY(AudioRes);

    public:
        std::string audio_file_uri;
    };
} // namespace ArchViz
