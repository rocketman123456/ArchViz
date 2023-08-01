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
        ResourceHandle m_base_colour {k_invalid_res_handle};
        ResourceHandle m_metallic_roughness {k_invalid_res_handle};
        ResourceHandle m_normal {k_invalid_res_handle};
        ResourceHandle m_occlusion {k_invalid_res_handle};
        ResourceHandle m_emissive {k_invalid_res_handle};
        // std::string m_uri;
    };
} // namespace ArchViz
