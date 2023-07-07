#pragma once

#include "runtime/function/render/rhi/gpu_enum.h"
#include "runtime/function/render/rhi/gpu_handle.h"

#include <volk.h>

#include <vk_mem_alloc.h>

#include <cstdint>
#include <limits>

namespace ArchViz
{
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

        DepthStencilCreation& setDepth(bool write, VkCompareOp comparison_test);

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

        BlendState& setColor(VkBlendFactor source_color, VkBlendFactor destination_color, VkBlendOp color_operation);
        BlendState& setAlpha(VkBlendFactor source_color, VkBlendFactor destination_color, VkBlendOp color_operation);
        BlendState& setColorWriteMask(ColorWriteEnabled::Mask value);

    }; // struct BlendState

    struct BlendStateCreation
    {
        BlendState blend_states[k_max_image_outputs];
        uint32_t   active_states = 0;

        BlendStateCreation& reset();
        BlendState&         addBlendState();

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
        BufferCreation& setData(void* data);
        BufferCreation& setName(const char* name);

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

        TextureCreation& setSize(uint16_t width, uint16_t height, uint16_t depth);
        TextureCreation& setFlags(uint8_t mipmaps, uint8_t flags);
        TextureCreation& setFormatType(VkFormat format, TextureType::Enum type);
        TextureCreation& setName(const char* name);
        TextureCreation& setData(void* data);

    }; // struct TextureCreation

    struct SamplerCreation
    {
        VkFilter            min_filter = VK_FILTER_NEAREST;
        VkFilter            mag_filter = VK_FILTER_NEAREST;
        VkSamplerMipmapMode mip_filter = VK_SAMPLER_MIPMAP_MODE_NEAREST;

        VkSamplerAddressMode address_mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode address_mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode address_mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        const char* name = nullptr;

        SamplerCreation& setMinMagMip(VkFilter min, VkFilter mag, VkSamplerMipmapMode mip);
        SamplerCreation& setAddressModeU(VkSamplerAddressMode u);
        SamplerCreation& setAddressModeUV(VkSamplerAddressMode u, VkSamplerAddressMode v);
        SamplerCreation& setAddressModeUVW(VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w);
        SamplerCreation& setName(const char* name);

    }; // struct SamplerCreation

    struct ShaderStage
    {
        const char*           code      = nullptr;
        uint32_t              code_size = 0;
        VkShaderStageFlagBits type      = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

    }; // struct ShaderStage

    struct ShaderStateCreation
    {
        ShaderStage stages[k_max_shader_stages];

        const char* name = nullptr;

        uint32_t stages_count = 0;
        uint32_t spv_input    = 0;

        // Building helpers
        ShaderStateCreation& reset();
        ShaderStateCreation& setName(const char* name);
        ShaderStateCreation& addStage(const char* code, uint32_t code_size, VkShaderStageFlagBits type);
        ShaderStateCreation& setSpvInput(bool value);

    }; // struct ShaderStateCreation

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
        DescriptorSetLayoutCreation& addBinding(const Binding& binding);
        DescriptorSetLayoutCreation& addBindingAtIndex(const Binding& binding, int index);
        DescriptorSetLayoutCreation& setName(const char* name);
        DescriptorSetLayoutCreation& setSetIndex(uint32_t index);

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
        DescriptorSetCreation& setLayout(DescriptorSetLayoutHandle layout);
        DescriptorSetCreation& texture(TextureHandle texture, uint16_t binding);
        DescriptorSetCreation& buffer(BufferHandle buffer, uint16_t binding);
        DescriptorSetCreation& textureSampler(TextureHandle texture, SamplerHandle sampler, uint16_t binding); // TODO: separate samplers from textures
        DescriptorSetCreation& setName(const char* name);

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
        VertexInputCreation& addVertexStream(const VertexStream& stream);
        VertexInputCreation& addVertexAttribute(const VertexAttribute& attribute);
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
        RenderPassOutput& setOperations(RenderPassOperation::Enum color, RenderPassOperation::Enum depth, RenderPassOperation::Enum stencil);

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
        RenderPassCreation& addRenderTexture(TextureHandle texture);
        RenderPassCreation& setScaling(float scale_x, float scale_y, uint8_t resize);
        RenderPassCreation& setDepthStencilTexture(TextureHandle texture);
        RenderPassCreation& setName(const char* name);
        RenderPassCreation& setType(RenderPassType::Enum type);
        RenderPassCreation& setOperations(RenderPassOperation::Enum color, RenderPassOperation::Enum depth, RenderPassOperation::Enum stencil);

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

        PipelineCreation& addDescriptorSetLayout(DescriptorSetLayoutHandle handle);
        RenderPassOutput& renderPassOutput();

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
        ExecutionBarrier& addImageBarrier(const ImageBarrier& image_barrier);
        ExecutionBarrier& addMemoryBarrier(const MemoryBarrier& memory_barrier);

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

        VkBuffer vk_buffer;
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
