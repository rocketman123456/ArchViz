#pragma once

#include <cstdint>

namespace ArchViz
{
    static const uint32_t k_invalid_index = 0xffffffff;

    using ResourceHandle = uint32_t;

    struct BufferHandle
    {
        ResourceHandle index;
    }; // struct BufferHandle

    struct TextureHandle
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

    // Invalid handles
    static BufferHandle              k_invalid_buffer {k_invalid_index};
    static TextureHandle             k_invalid_texture {k_invalid_index};
    static ShaderStateHandle         k_invalid_shader {k_invalid_index};
    static SamplerHandle             k_invalid_sampler {k_invalid_index};
    static DescriptorSetLayoutHandle k_invalid_layout {k_invalid_index};
    static DescriptorSetHandle       k_invalid_set {k_invalid_index};
    static PipelineHandle            k_invalid_pipeline {k_invalid_index};
    static RenderPassHandle          k_invalid_pass {k_invalid_index};

    static const uint8_t k_max_image_outputs          = 8;  // Maximum number of images/render_targets/fbo attachments usable.
    static const uint8_t k_max_descriptor_set_layouts = 8;  // Maximum number of layouts in the pipeline.
    static const uint8_t k_max_shader_stages          = 5;  // Maximum simultaneous shader stages. Applicable to all different type of pipelines.
    static const uint8_t k_max_descriptors_per_set    = 16; // Maximum list elements for both descriptor set layout and descriptor sets.
    static const uint8_t k_max_vertex_streams         = 16;
    static const uint8_t k_max_vertex_attributes      = 16;

    static const uint32_t k_submit_header_sentinel = 0xfefeb7ba;
    static const uint32_t k_max_resource_deletions = 64;
}