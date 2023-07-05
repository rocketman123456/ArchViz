#pragma once

#include "runtime/function/render/rhi/gpu_enum.h"

#include <volk.h>

#include <cstdint>
#include <limits>

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

    struct Rect2D
    {
        float x      = 0.0f;
        float y      = 0.0f;
        float width  = 0.0f;
        float height = 0.0f;
    }; // struct Rect2D

    //
    //
    struct Rect2DInt
    {
        int16_t  x      = 0;
        int16_t  y      = 0;
        uint16_t width  = 0;
        uint16_t height = 0;
    }; // struct Rect2D

    //
    //
    struct Viewport
    {
        Rect2DInt rect;
        float     min_depth = 0.0f;
        float     max_depth = 0.0f;
    }; // struct Viewport

    //
    //
    struct ViewportState
    {
        uint32_t num_viewports = 0;
        uint32_t num_scissors  = 0;

        Viewport*  viewport = nullptr;
        Rect2DInt* scissors = nullptr;
    }; // struct ViewportState

    struct StencilOperationState
    {
        VkStencilOp fail         = VK_STENCIL_OP_KEEP;
        VkStencilOp pass         = VK_STENCIL_OP_KEEP;
        VkStencilOp depth_fail   = VK_STENCIL_OP_KEEP;
        VkCompareOp compare      = VK_COMPARE_OP_ALWAYS;
        uint32_t    compare_mask = 0xff;
        uint32_t    write_mask   = 0xff;
        uint32_t    reference    = 0xff;
    }; // struct StencilOperationState

    struct DepthStencilCreation
    {
        StencilOperationState front;
        StencilOperationState back;
        VkCompareOp           depth_comparison = VK_COMPARE_OP_ALWAYS;

        uint8_t depth_enable       = 1;
        uint8_t depth_write_enable = 1;
        uint8_t stencil_enable     = 1;
        uint8_t pad                = 5;

        // Default constructor
        DepthStencilCreation() : depth_enable(0), depth_write_enable(0), stencil_enable(0) {}

        DepthStencilCreation& set_depth(bool write, VkCompareOp comparison_test);

    }; // struct DepthStencilCreation

    struct BlendState
    {

        VkBlendFactor source_color      = VK_BLEND_FACTOR_ONE;
        VkBlendFactor destination_color = VK_BLEND_FACTOR_ONE;
        VkBlendOp     color_operation   = VK_BLEND_OP_ADD;

        VkBlendFactor source_alpha      = VK_BLEND_FACTOR_ONE;
        VkBlendFactor destination_alpha = VK_BLEND_FACTOR_ONE;
        VkBlendOp     alpha_operation   = VK_BLEND_OP_ADD;

        ColorWriteEnabled::Mask color_write_mask = ColorWriteEnabled::All_mask;

        uint8_t blend_enabled : 1;
        uint8_t separate_blend : 1;
        uint8_t pad : 6;

        BlendState() : blend_enabled(0), separate_blend(0) {}

        BlendState& set_color(VkBlendFactor source_color, VkBlendFactor destination_color, VkBlendOp color_operation);
        BlendState& set_alpha(VkBlendFactor source_color, VkBlendFactor destination_color, VkBlendOp color_operation);
        BlendState& set_color_write_mask(ColorWriteEnabled::Mask value);

    }; // struct BlendState

    struct BlendStateCreation
    {

        BlendState blend_states[k_max_image_outputs];
        uint32_t   active_states = 0;

        BlendStateCreation& reset();
        BlendState&         add_blend_state();

    }; // BlendStateCreation

    //
    //
    struct RasterizationCreation
    {

        VkCullModeFlagBits cull_mode = VK_CULL_MODE_NONE;
        VkFrontFace        front     = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        FillMode::Enum     fill      = FillMode::Solid;
    }; // struct RasterizationCreation

    //
    //
    struct BufferCreation
    {

        VkBufferUsageFlags      type_flags   = 0;
        ResourceUsageType::Enum usage        = ResourceUsageType::Immutable;
        uint32_t                size         = 0;
        void*                   initial_data = nullptr;

        const char* name = nullptr;

        BufferCreation& reset();
        BufferCreation& set(VkBufferUsageFlags flags, ResourceUsageType::Enum usage, uint32_t size);
        BufferCreation& set_data(void* data);
        BufferCreation& set_name(const char* name);

    }; // struct BufferCreation

    //
    //
    struct TextureCreation
    {

        void*    initial_data = nullptr;
        uint16_t width        = 1;
        uint16_t height       = 1;
        uint16_t depth        = 1;
        uint8_t  mipmaps      = 1;
        uint8_t  flags        = 0; // TextureFlags bitmasks

        VkFormat          format = VK_FORMAT_UNDEFINED;
        TextureType::Enum type   = TextureType::Texture2D;

        const char* name = nullptr;

        TextureCreation& set_size(uint16_t width, uint16_t height, uint16_t depth);
        TextureCreation& set_flags(uint8_t mipmaps, uint8_t flags);
        TextureCreation& set_format_type(VkFormat format, TextureType::Enum type);
        TextureCreation& set_name(const char* name);
        TextureCreation& set_data(void* data);

    }; // struct TextureCreation

    //
    //
    struct SamplerCreation
    {

        VkFilter            min_filter = VK_FILTER_NEAREST;
        VkFilter            mag_filter = VK_FILTER_NEAREST;
        VkSamplerMipmapMode mip_filter = VK_SAMPLER_MIPMAP_MODE_NEAREST;

        VkSamplerAddressMode address_mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode address_mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode address_mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        const char* name = nullptr;

        SamplerCreation& set_min_mag_mip(VkFilter min, VkFilter mag, VkSamplerMipmapMode mip);
        SamplerCreation& set_address_mode_u(VkSamplerAddressMode u);
        SamplerCreation& set_address_mode_uv(VkSamplerAddressMode u, VkSamplerAddressMode v);
        SamplerCreation& set_address_mode_uvw(VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w);
        SamplerCreation& set_name(const char* name);

    }; // struct SamplerCreation

    //
    //
    struct ShaderStage
    {

        const char*           code      = nullptr;
        uint32_t              code_size = 0;
        VkShaderStageFlagBits type      = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

    }; // struct ShaderStage

    //
    //
    struct ShaderStateCreation
    {

        ShaderStage stages[k_max_shader_stages];

        const char* name = nullptr;

        uint32_t stages_count = 0;
        uint32_t spv_input    = 0;

        // Building helpers
        ShaderStateCreation& reset();
        ShaderStateCreation& set_name(const char* name);
        ShaderStateCreation& add_stage(const char* code, uint32_t code_size, VkShaderStageFlagBits type);
        ShaderStateCreation& set_spv_input(bool value);

    }; // struct ShaderStateCreation

    //
    //
    struct DescriptorSetLayoutCreation
    {
        //
        // A single descriptor binding. It can be relative to one or more resources of the same type.
        //
        struct Binding
        {
            VkDescriptorType type  = VK_DESCRIPTOR_TYPE_MAX_ENUM;
            uint16_t         start = 0;
            uint16_t         count = 0;
            const char*      name  = nullptr; // Comes from external memory.
        };                                    // struct Binding

        Binding  bindings[k_max_descriptors_per_set];
        uint32_t num_bindings = 0;
        uint32_t set_index    = 0;

        const char* name = nullptr;

        // Building helpers
        DescriptorSetLayoutCreation& reset();
        DescriptorSetLayoutCreation& add_binding(const Binding& binding);
        DescriptorSetLayoutCreation& add_binding_at_index(const Binding& binding, int index);
        DescriptorSetLayoutCreation& set_name(const char* name);
        DescriptorSetLayoutCreation& set_set_index(uint32_t index);

    }; // struct DescriptorSetLayoutCreation

    //
    //
    struct DescriptorSetCreation
    {

        ResourceHandle resources[k_max_descriptors_per_set];
        SamplerHandle  samplers[k_max_descriptors_per_set];
        uint16_t       bindings[k_max_descriptors_per_set];

        DescriptorSetLayoutHandle layout;
        uint32_t                  num_resources = 0;

        const char* name = nullptr;

        // Building helpers
        DescriptorSetCreation& reset();
        DescriptorSetCreation& set_layout(DescriptorSetLayoutHandle layout);
        DescriptorSetCreation& texture(TextureHandle texture, uint16_t binding);
        DescriptorSetCreation& buffer(BufferHandle buffer, uint16_t binding);
        DescriptorSetCreation& texture_sampler(TextureHandle texture, SamplerHandle sampler, uint16_t binding); // TODO: separate samplers from textures
        DescriptorSetCreation& set_name(const char* name);

    }; // struct DescriptorSetCreation

    //
    //
    struct DescriptorSetUpdate
    {
        DescriptorSetHandle descriptor_set;

        uint32_t frame_issued = 0;
    }; // DescriptorSetUpdate

    //
    //
    struct VertexAttribute
    {

        uint16_t                    location = 0;
        uint16_t                    binding  = 0;
        uint32_t                    offset   = 0;
        VertexComponentFormat::Enum format   = VertexComponentFormat::Count;

    }; // struct VertexAttribute

    //
    //
    struct VertexStream
    {

        uint16_t              binding    = 0;
        uint16_t              stride     = 0;
        VertexInputRate::Enum input_rate = VertexInputRate::Count;

    }; // struct VertexStream

    //
    //
    struct VertexInputCreation
    {

        uint32_t num_vertex_streams    = 0;
        uint32_t num_vertex_attributes = 0;

        VertexStream    vertex_streams[k_max_vertex_streams];
        VertexAttribute vertex_attributes[k_max_vertex_attributes];

        VertexInputCreation& reset();
        VertexInputCreation& add_vertex_stream(const VertexStream& stream);
        VertexInputCreation& add_vertex_attribute(const VertexAttribute& attribute);
    }; // struct VertexInputCreation

    //
    //
    struct RenderPassOutput
    {

        VkFormat color_formats[k_max_image_outputs];
        VkFormat depth_stencil_format;
        uint32_t num_color_formats;

        RenderPassOperation::Enum color_operation   = RenderPassOperation::DontCare;
        RenderPassOperation::Enum depth_operation   = RenderPassOperation::DontCare;
        RenderPassOperation::Enum stencil_operation = RenderPassOperation::DontCare;

        RenderPassOutput& reset();
        RenderPassOutput& color(VkFormat format);
        RenderPassOutput& depth(VkFormat format);
        RenderPassOutput& set_operations(RenderPassOperation::Enum color, RenderPassOperation::Enum depth, RenderPassOperation::Enum stencil);

    }; // struct RenderPassOutput

    //
    //
    struct RenderPassCreation
    {

        uint16_t             num_render_targets = 0;
        RenderPassType::Enum type               = RenderPassType::Geometry;

        TextureHandle output_textures[k_max_image_outputs];
        TextureHandle depth_stencil_texture;

        float   scale_x = 1.f;
        float   scale_y = 1.f;
        uint8_t resize  = 1;

        RenderPassOperation::Enum color_operation   = RenderPassOperation::DontCare;
        RenderPassOperation::Enum depth_operation   = RenderPassOperation::DontCare;
        RenderPassOperation::Enum stencil_operation = RenderPassOperation::DontCare;

        const char* name = nullptr;

        RenderPassCreation& reset();
        RenderPassCreation& add_render_texture(TextureHandle texture);
        RenderPassCreation& set_scaling(float scale_x, float scale_y, uint8_t resize);
        RenderPassCreation& set_depth_stencil_texture(TextureHandle texture);
        RenderPassCreation& set_name(const char* name);
        RenderPassCreation& set_type(RenderPassType::Enum type);
        RenderPassCreation& set_operations(RenderPassOperation::Enum color, RenderPassOperation::Enum depth, RenderPassOperation::Enum stencil);

    }; // struct RenderPassCreation

    //
    //
    struct PipelineCreation
    {

        RasterizationCreation rasterization;
        DepthStencilCreation  depth_stencil;
        BlendStateCreation    blend_state;
        VertexInputCreation   vertex_input;
        ShaderStateCreation   shaders;

        RenderPassOutput          render_pass;
        DescriptorSetLayoutHandle descriptor_set_layout[k_max_descriptor_set_layouts];
        const ViewportState*      viewport = nullptr;

        uint32_t num_active_layouts = 0;

        const char* name = nullptr;

        PipelineCreation& add_descriptor_set_layout(DescriptorSetLayoutHandle handle);
        RenderPassOutput& render_pass_output();

    }; // struct PipelineCreation

    // API-agnostic structs /////////////////////////////////////////////////////////

    //
    // Helper methods for texture formats
    //
    namespace TextureFormat
    {

        inline bool is_depth_stencil(VkFormat value) { return value == VK_FORMAT_D16_UNORM_S8_UINT || value == VK_FORMAT_D24_UNORM_S8_UINT || value == VK_FORMAT_D32_SFLOAT_S8_UINT; }
        inline bool is_depth_only(VkFormat value) { return value >= VK_FORMAT_D16_UNORM && value < VK_FORMAT_D32_SFLOAT; }
        inline bool is_stencil_only(VkFormat value) { return value == VK_FORMAT_S8_UINT; }

        inline bool has_depth(VkFormat value) { return (value >= VK_FORMAT_D16_UNORM && value < VK_FORMAT_S8_UINT) || (value >= VK_FORMAT_D16_UNORM_S8_UINT && value <= VK_FORMAT_D32_SFLOAT_S8_UINT); }
        inline bool has_stencil(VkFormat value) { return value >= VK_FORMAT_S8_UINT && value <= VK_FORMAT_D32_SFLOAT_S8_UINT; }
        inline bool has_depth_or_stencil(VkFormat value) { return value >= VK_FORMAT_D16_UNORM && value <= VK_FORMAT_D32_SFLOAT_S8_UINT; }

    } // namespace TextureFormat

    struct ResourceData
    {

        void* data = nullptr;

    }; // struct ResourceData

    //
    //
    struct ResourceBinding
    {
        uint16_t type  = 0; // ResourceType
        uint16_t start = 0;
        uint16_t count = 0;
        uint16_t set   = 0;

        const char* name = nullptr;
    }; // struct ResourceBinding

    // API-agnostic descriptions ////////////////////////////////////////////////////

    //
    //
    struct ShaderStateDescription
    {

        void*       native_handle = nullptr;
        const char* name          = nullptr;

    }; // struct ShaderStateDescription

    //
    //
    struct BufferDescription
    {

        void*       native_handle = nullptr;
        const char* name          = nullptr;

        VkBufferUsageFlags      type_flags = 0;
        ResourceUsageType::Enum usage      = ResourceUsageType::Immutable;
        uint32_t                size       = 0;
        BufferHandle            parent_handle;

    }; // struct BufferDescription

    //
    //
    struct TextureDescription
    {

        void*       native_handle = nullptr;
        const char* name          = nullptr;

        uint16_t width          = 1;
        uint16_t height         = 1;
        uint16_t depth          = 1;
        uint8_t  mipmaps        = 1;
        uint8_t  render_target  = 0;
        uint8_t  compute_access = 0;

        VkFormat          format = VK_FORMAT_UNDEFINED;
        TextureType::Enum type   = TextureType::Texture2D;

    }; // struct Texture

    //
    //
    struct SamplerDescription
    {

        const char* name = nullptr;

        VkFilter            min_filter = VK_FILTER_NEAREST;
        VkFilter            mag_filter = VK_FILTER_NEAREST;
        VkSamplerMipmapMode mip_filter = VK_SAMPLER_MIPMAP_MODE_NEAREST;

        VkSamplerAddressMode address_mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode address_mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode address_mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    }; // struct SamplerDescription

    //
    //
    struct DescriptorSetLayoutDescription
    {

        ResourceBinding bindings[k_max_descriptors_per_set];
        uint32_t        num_active_bindings = 0;

    }; // struct DescriptorSetLayoutDescription

    //
    //
    struct DesciptorSetDescription
    {

        ResourceData resources[k_max_descriptors_per_set];
        uint32_t     num_active_resources = 0;

    }; // struct DesciptorSetDescription

    //
    //
    struct PipelineDescription
    {

        ShaderStateHandle shader;

    }; // struct PipelineDescription

    // API-agnostic resource modifications //////////////////////////////////////////

    struct MapBufferParameters
    {
        BufferHandle buffer;
        uint32_t     offset = 0;
        uint32_t     size   = 0;

    }; // struct MapBufferParameters

    // Synchronization //////////////////////////////////////////////////////////////

    //
    //
    struct ImageBarrier
    {

        TextureHandle texture;

    }; // struct ImageBarrier

    //
    //
    struct MemoryBarrier
    {

        BufferHandle buffer;

    }; // struct MemoryBarrier

    //
    //
    struct ExecutionBarrier
    {

        PipelineStage::Enum source_pipeline_stage;
        PipelineStage::Enum destination_pipeline_stage;

        uint32_t new_barrier_experimental = std::numeric_limits<uint32_t>::max();
        uint32_t load_operation           = 0;

        uint32_t num_image_barriers;
        uint32_t num_memory_barriers;

        ImageBarrier  image_barriers[8];
        MemoryBarrier memory_barriers[8];

        ExecutionBarrier& reset();
        ExecutionBarrier& set(PipelineStage::Enum source, PipelineStage::Enum destination);
        ExecutionBarrier& add_image_barrier(const ImageBarrier& image_barrier);
        ExecutionBarrier& add_memory_barrier(const MemoryBarrier& memory_barrier);

    }; // struct Barrier

    //
    //
    struct ResourceUpdate
    {

        ResourceDeletionType::Enum type;
        ResourceHandle             handle;
        uint32_t                   current_frame;
    }; // struct ResourceUpdate

    // Resources /////////////////////////////////////////////////////////////

    static const uint32_t k_max_swapchain_images = 3;

    struct DeviceStateVulkan;

    //
    //
    struct Buffer
    {

        VkBuffer       vk_buffer;
        // VmaAllocation  vma_allocation;
        VkDeviceMemory vk_device_memory;
        VkDeviceSize   vk_device_size;

        VkBufferUsageFlags      type_flags    = 0;
        ResourceUsageType::Enum usage         = ResourceUsageType::Immutable;
        uint32_t                size          = 0;
        uint32_t                global_offset = 0; // Offset into global constant, if dynamic

        BufferHandle handle;
        BufferHandle parent_buffer;

        const char* name = nullptr;

    }; // struct BufferVulkan

    //
    //
    struct Sampler
    {

        VkSampler vk_sampler;

        VkFilter            min_filter = VK_FILTER_NEAREST;
        VkFilter            mag_filter = VK_FILTER_NEAREST;
        VkSamplerMipmapMode mip_filter = VK_SAMPLER_MIPMAP_MODE_NEAREST;

        VkSamplerAddressMode address_mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode address_mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode address_mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        const char* name = nullptr;

    }; // struct SamplerVulkan

    //
    //
    struct Texture
    {

        VkImage       vk_image;
        VkImageView   vk_image_view;
        VkFormat      vk_format;
        VkImageLayout vk_image_layout;
        // VmaAllocation vma_allocation;

        uint16_t width   = 1;
        uint16_t height  = 1;
        uint16_t depth   = 1;
        uint8_t  mipmaps = 1;
        uint8_t  flags   = 0;

        TextureHandle     handle;
        TextureType::Enum type = TextureType::Texture2D;

        Sampler* sampler = nullptr;

        const char* name = nullptr;
    }; // struct TextureVulkan

    //
    //
    struct ShaderState
    {
        VkPipelineShaderStageCreateInfo shader_stage_info[k_max_shader_stages];

        const char* name = nullptr;

        uint32_t active_shaders    = 0;
        bool     graphics_pipeline = false;

        // spirv::ParseResult* parse_result;
    }; // struct ShaderStateVulkan

    //
    //
    struct DescriptorBinding
    {
        VkDescriptorType type;
        uint16_t         start = 0;
        uint16_t         count = 0;
        uint16_t         set   = 0;

        const char* name = nullptr;
    }; // struct ResourceBindingVulkan

    //
    //
    struct DesciptorSetLayout
    {
        VkDescriptorSetLayout vk_descriptor_set_layout;

        VkDescriptorSetLayoutBinding* vk_binding   = nullptr;
        DescriptorBinding*            bindings     = nullptr;
        uint16_t                      num_bindings = 0;
        uint16_t                      set_index    = 0;

        DescriptorSetLayoutHandle handle;
    }; // struct DesciptorSetLayoutVulkan

    //
    //
    struct DesciptorSet
    {
        VkDescriptorSet vk_descriptor_set;

        ResourceHandle* resources = nullptr;
        SamplerHandle*  samplers  = nullptr;
        uint16_t*       bindings  = nullptr;

        const DesciptorSetLayout* layout        = nullptr;
        uint32_t                  num_resources = 0;
    }; // struct DesciptorSetVulkan

    //
    //
    struct Pipeline
    {
        VkPipeline       vk_pipeline;
        VkPipelineLayout vk_pipeline_layout;

        VkPipelineBindPoint vk_bind_point;

        ShaderStateHandle shader_state;

        const DesciptorSetLayout* descriptor_set[k_max_descriptor_set_layouts];
        DescriptorSetLayoutHandle descriptor_set_layout_handle[k_max_descriptor_set_layouts];
        uint32_t                  num_active_layouts = 0;

        DepthStencilCreation  depth_stencil;
        BlendStateCreation    blend_state;
        RasterizationCreation rasterization;

        PipelineHandle handle;
        bool           graphics_pipeline = true;
    }; // struct PipelineVulkan

    //
    //
    struct RenderPass
    {
        VkRenderPass  vk_render_pass;
        VkFramebuffer vk_frame_buffer;

        RenderPassOutput output;

        TextureHandle output_textures[k_max_image_outputs];
        TextureHandle output_depth;

        RenderPassType::Enum type;

        float    scale_x    = 1.f;
        float    scale_y    = 1.f;
        uint16_t width      = 0;
        uint16_t height     = 0;
        uint16_t dispatch_x = 0;
        uint16_t dispatch_y = 0;
        uint16_t dispatch_z = 0;

        uint8_t resize             = 0;
        uint8_t num_render_targets = 0;

        const char* name = nullptr;
    }; // struct RenderPass
} // namespace ArchViz
