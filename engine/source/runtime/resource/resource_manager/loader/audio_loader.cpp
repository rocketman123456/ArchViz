#include "runtime/resource/resource_manager/loader/audio_loader.h"

#include "runtime/function/global/global_context.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"
#include "runtime/resource/resource_manager/resource_manager.h"

#include <filesystem>
#include <vector>

namespace ArchViz
{
    std::pair<std::shared_ptr<AudioData>, size_t> AudioLoader::createResource(const AudioRes& create_info)
    {
        std::shared_ptr<AudioData> audio = std::make_shared<AudioData>();

        std::vector<std::byte> content;
        g_runtime_global_context.m_asset_manager->readBinaryFile(create_info.audio_file_uri, content);

        audio->wave.loadMem((unsigned char*)content.data(), content.size());
        
        return {audio, static_cast<size_t>(audio->wave.mSampleCount)};
    }

    std::pair<std::shared_ptr<AudioData>, size_t> AudioLoader::createResource(const std::string& uri)
    {
        std::shared_ptr<AudioData> audio = std::make_shared<AudioData>();

        std::vector<std::byte> content;
        g_runtime_global_context.m_asset_manager->readBinaryFile(uri, content);

        audio->wave.loadMem((unsigned char*)content.data(), content.size());

        return {audio, static_cast<size_t>(audio->wave.mSampleCount)};
    }
} // namespace ArchViz
