#pragma once

#include <cstdint>

namespace ArchViz
{
    using ResourceHandle = uint32_t;

    struct ModelHandle
    {
        ResourceHandle index;
    }; // struct ModelHandle

    struct MeshHandle
    {
        ResourceHandle index;
    }; // struct MeshHandle

    struct MaterialHandle
    {
        ResourceHandle index;
    }; // struct MaterialHandle

    struct AudioHandle
    {
        ResourceHandle index;
    };

    struct BufferHandle
    {
        ResourceHandle index;
    }; // struct BufferHandle

    struct TextureHandle
    {
        ResourceHandle index;
    }; // struct TextureHandle

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    struct GpuBufferHandle
    {
        ResourceHandle index;
    }; // struct BufferHandle

    struct GpuTextureHandle
    {
        ResourceHandle index;
    }; // struct TextureHandle

    struct ShaderStateHandle
    {
        ResourceHandle index;
    }; // struct ShaderStateHandle

    struct SamplerHandle
    {
        ResourceHandle index;
    }; // struct SamplerHandle

    struct DescriptorSetLayoutHandle
    {
        ResourceHandle index;
    }; // struct DescriptorSetLayoutHandle

    struct DescriptorSetHandle
    {
        ResourceHandle index;
    }; // struct DescriptorSetHandle

    struct PipelineHandle
    {
        ResourceHandle index;
    }; // struct PipelineHandle

    struct RenderPassHandle
    {
        ResourceHandle index;
    }; // struct RenderPassHandle

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    static const uint32_t k_invalid_index = 0xffffffff;

    // Invalid handles
    // CPU side
    static ModelHandle    k_invalid_model {k_invalid_index};
    static MeshHandle     k_invalid_mesh {k_invalid_index};
    static MaterialHandle k_invalid_material {k_invalid_index};
    static AudioHandle    k_invalid_audio {k_invalid_index};
    static BufferHandle   k_invalid_buffer {k_invalid_index};
    static TextureHandle  k_invalid_texture {k_invalid_index};

    // GPU side
    static GpuBufferHandle           k_invalid_gpu_buffer {k_invalid_index};
    static GpuTextureHandle          k_invalid_gpu_texture {k_invalid_index};
    static ShaderStateHandle         k_invalid_shader {k_invalid_index};
    static SamplerHandle             k_invalid_sampler {k_invalid_index};
    static DescriptorSetLayoutHandle k_invalid_layout {k_invalid_index};
    static DescriptorSetHandle       k_invalid_set {k_invalid_index};
    static PipelineHandle            k_invalid_pipeline {k_invalid_index};
    static RenderPassHandle          k_invalid_pass {k_invalid_index};

    template<typename T>
    inline bool is_valid_handle(T handle)
    {
        return T.index != k_invalid_index;
    }
} // namespace ArchViz
