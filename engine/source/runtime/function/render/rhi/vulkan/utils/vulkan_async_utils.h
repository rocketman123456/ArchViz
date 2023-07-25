#pragma once
#include "runtime/function/render/rhi/gpu_resources.h"

#include <volk.h>

#include <string>

namespace ArchViz
{
    struct FileLoadRequest
    {
        std::string   path;
        BufferHandle  buffer  = k_invalid_buffer;
        TextureHandle texture = k_invalid_texture;
    };

    struct GpuUploadRequest
    {
        void*         data       = nullptr;
        BufferHandle  cpu_buffer = k_invalid_buffer;
        BufferHandle  gpu_buffer = k_invalid_buffer;
        TextureHandle texture    = k_invalid_texture;
    };

    class VulkanAsyncUtils
    {
    public:
    };
} // namespace ArchViz
