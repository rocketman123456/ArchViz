#pragma once
#include "runtime/resource/resource_manager/resource_handle.h"

#include <memory>
#include <string>
#include <vector>

namespace ArchViz
{
    class TextureData
    {
    public:
        std::vector<uint8_t> m_data;

        int32_t m_width;
        int32_t m_height;
        int32_t m_channel;

        std::string m_uri;
    };

    class MaterialData
    {
    public:
        ResHandle m_base_colour {k_invalid_res_handle};
        ResHandle m_metallic_roughness {k_invalid_res_handle};
        ResHandle m_normal {k_invalid_res_handle};
        ResHandle m_occlusion {k_invalid_res_handle};
        ResHandle m_emissive {k_invalid_res_handle};
        // std::string m_uri;
    };
} // namespace ArchViz
