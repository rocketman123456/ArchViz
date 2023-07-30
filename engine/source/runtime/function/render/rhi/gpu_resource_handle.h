#pragma once

#include "runtime/resource/resource_manager/resource_handle.h"

namespace ArchViz
{
    struct GpuBufferHandle
    {
        ResourceHandle index;
    }; // struct GpuTextureHandle

    struct GpuTextureHandle
    {
        ResourceHandle index;
    }; // struct GpuTextureHandle

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

    // Invalid handles
    // GPU side
    static GpuBufferHandle           k_invalid_gpu_buffer {k_invalid_index};
    static GpuTextureHandle          k_invalid_gpu_texture {k_invalid_index};
    static ShaderStateHandle         k_invalid_shader {k_invalid_index};
    static SamplerHandle             k_invalid_sampler {k_invalid_index};
    static DescriptorSetLayoutHandle k_invalid_layout {k_invalid_index};
    static DescriptorSetHandle       k_invalid_set {k_invalid_index};
    static PipelineHandle            k_invalid_pipeline {k_invalid_index};
    static RenderPassHandle          k_invalid_pass {k_invalid_index};
} // namespace ArchViz
