#pragma once
#include "runtime/resource/resource_manager/loader/loader.h"

#include "runtime/resource/res_type/components/audio_res.h"
#include "runtime/resource/res_type/data/audio_data.h"

#include <memory>
#include <string>
#include <utility>

namespace ArchViz
{
    class AudioLoader : public Loader<AudioData, AudioRes>
    {
    public:
        virtual ~AudioLoader() = default;

        std::pair<std::shared_ptr<AudioData>, size_t> createResource(const AudioRes& create_info) override;
        std::pair<std::shared_ptr<AudioData>, size_t> createResource(const std::string& uri) override;
    };
}
