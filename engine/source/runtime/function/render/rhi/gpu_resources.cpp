#include "runtime/function/render/rhi/gpu_resources.h"

namespace ArchViz
{
    // DepthStencilCreation ////////////////////////////////////

    DepthStencilCreation& DepthStencilCreation::setDepth(bool write, VkCompareOp comparison_test)
    {
        depth_write_enable = write;
        depth_comparison   = comparison_test;
        // Setting depth like this means we want to use the depth test.
        depth_enable = 1;

        return *this;
    }

    // BlendState  /////////////////////////////////////////////
    BlendState& BlendState::setColor(VkBlendFactor source, VkBlendFactor destination, VkBlendOp operation)
    {
        source_color      = source;
        destination_color = destination;
        color_operation   = operation;
        blend_enabled     = 1;

        return *this;
    }

    BlendState& BlendState::setAlpha(VkBlendFactor source, VkBlendFactor destination, VkBlendOp operation)
    {
        source_alpha      = source;
        destination_alpha = destination;
        alpha_operation   = operation;
        separate_blend    = 1;

        return *this;
    }

    BlendState& BlendState::setColorWriteMask(ColorWriteEnabled::Mask value)
    {
        color_write_mask = value;

        return *this;
    }

    // BlendStateCreation //////////////////////////////////////
    BlendStateCreation& BlendStateCreation::reset()
    {
        active_states = 0;

        return *this;
    }

    BlendState& BlendStateCreation::addBlendState() { return blend_states[active_states++]; }

    // BufferCreation //////////////////////////////////////////
    BufferCreation& BufferCreation::reset()
    {
        size         = 0;
        initial_data = nullptr;

        return *this;
    }

    BufferCreation& BufferCreation::set(VkBufferUsageFlags flags, ResourceUsageType::Enum usage_, uint32_t size_)
    {
        type_flags = flags;
        usage      = usage_;
        size       = size_;

        return *this;
    }

    BufferCreation& BufferCreation::setData(void* data_)
    {
        initial_data = data_;

        return *this;
    }

    BufferCreation& BufferCreation::setName(const std::string& name_)
    {
        name = name_;

        return *this;
    }

    // TextureCreation /////////////////////////////////////////
    TextureCreation& TextureCreation::setSize(uint16_t width_, uint16_t height_, uint16_t depth_)
    {
        width  = width_;
        height = height_;
        depth  = depth_;

        return *this;
    }

    TextureCreation& TextureCreation::setFlags(uint8_t mipmaps_, uint8_t flags_)
    {
        mipmaps = mipmaps_;
        flags   = flags_;

        return *this;
    }

    TextureCreation& TextureCreation::setFormatType(VkFormat format_, TextureType::Enum type_)
    {
        format = format_;
        type   = type_;

        return *this;
    }

    TextureCreation& TextureCreation::setName(const std::string& name_)
    {
        name = name_;

        return *this;
    }

    TextureCreation& TextureCreation::setData(void* data_)
    {
        initial_data = data_;

        return *this;
    }

    // SamplerCreation /////////////////////////////////////////
    SamplerCreation& SamplerCreation::setMinMagMip(VkFilter min, VkFilter mag, VkSamplerMipmapMode mip)
    {
        min_filter = min;
        mag_filter = mag;
        mip_filter = mip;

        return *this;
    }

    SamplerCreation& SamplerCreation::setAddressModeU(VkSamplerAddressMode u)
    {
        address_mode_u = u;

        return *this;
    }

    SamplerCreation& SamplerCreation::setAddressModeUV(VkSamplerAddressMode u, VkSamplerAddressMode v)
    {
        address_mode_u = u;
        address_mode_v = v;

        return *this;
    }

    SamplerCreation& SamplerCreation::setAddressModeUVW(VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w)
    {
        address_mode_u = u;
        address_mode_v = v;
        address_mode_w = w;

        return *this;
    }

    SamplerCreation& SamplerCreation::setName(const std::string& name_)
    {
        name = name_;

        return *this;
    }

    // ShaderStateCreation /////////////////////////////////////
    ShaderStateCreation& ShaderStateCreation::reset()
    {
        stages_count = 0;

        return *this;
    }

    ShaderStateCreation& ShaderStateCreation::setName(const std::string& name_)
    {
        name = name_;

        return *this;
    }

    ShaderStateCreation& ShaderStateCreation::addStage(const std::string& code, uint32_t code_size, VkShaderStageFlagBits type)
    {
        stages[stages_count].code      = code;
        stages[stages_count].code_size = code_size;
        stages[stages_count].type      = type;
        ++stages_count;

        return *this;
    }

    ShaderStateCreation& ShaderStateCreation::setSpvInput(bool value)
    {
        spv_input = value;
        return *this;
    }

    // DescriptorSetLayoutCreation ////////////////////////////////////////////
    DescriptorSetLayoutCreation& DescriptorSetLayoutCreation::reset()
    {
        num_bindings = 0;
        set_index    = 0;
        return *this;
    }

    DescriptorSetLayoutCreation& DescriptorSetLayoutCreation::addBinding(const Binding& binding)
    {
        bindings[num_bindings++] = binding;
        return *this;
    }

    DescriptorSetLayoutCreation& DescriptorSetLayoutCreation::addBindingAtIndex(const Binding& binding, int index)
    {
        bindings[index] = binding;
        num_bindings    = (index + 1) > num_bindings ? (index + 1) : num_bindings;
        return *this;
    }

    DescriptorSetLayoutCreation& DescriptorSetLayoutCreation::setName(const std::string& name_)
    {
        name = name_;
        return *this;
    }

    DescriptorSetLayoutCreation& DescriptorSetLayoutCreation::setSetIndex(uint32_t index)
    {
        set_index = index;
        return *this;
    }

    // DescriptorSetCreation //////////////////////////////////////////////////
    DescriptorSetCreation& DescriptorSetCreation::reset()
    {
        num_resources = 0;
        return *this;
    }

    DescriptorSetCreation& DescriptorSetCreation::setLayout(DescriptorSetLayoutHandle layout_)
    {
        layout = layout_;
        return *this;
    }

    DescriptorSetCreation& DescriptorSetCreation::texture(GpuTextureHandle texture, uint16_t binding)
    {
        // Set a default sampler
        samplers[num_resources]    = k_invalid_sampler;
        bindings[num_resources]    = binding;
        resources[num_resources++] = texture.index;
        return *this;
    }

    DescriptorSetCreation& DescriptorSetCreation::buffer(GpuBufferHandle buffer, uint16_t binding)
    {
        samplers[num_resources]    = k_invalid_sampler;
        bindings[num_resources]    = binding;
        resources[num_resources++] = buffer.index;
        return *this;
    }

    DescriptorSetCreation& DescriptorSetCreation::textureSampler(GpuTextureHandle texture, SamplerHandle sampler, uint16_t binding)
    {
        bindings[num_resources]   = binding;
        resources[num_resources]  = texture.index;
        samplers[num_resources++] = sampler;
        return *this;
    }

    DescriptorSetCreation& DescriptorSetCreation::setName(const std::string& name_)
    {
        name = name_;
        return *this;
    }

    // VertexInputCreation /////////////////////////////////////
    VertexInputCreation& VertexInputCreation::reset()
    {
        num_vertex_streams = num_vertex_attributes = 0;
        return *this;
    }

    VertexInputCreation& VertexInputCreation::addVertexStream(const VertexStream& stream)
    {
        vertex_streams[num_vertex_streams++] = stream;
        return *this;
    }

    VertexInputCreation& VertexInputCreation::addVertexAttribute(const VertexAttribute& attribute)
    {
        vertex_attributes[num_vertex_attributes++] = attribute;
        return *this;
    }

    // RenderPassOutput ////////////////////////////////////////
    RenderPassOutput& RenderPassOutput::reset()
    {
        num_color_formats = 0;
        for (uint32_t i = 0; i < k_max_image_outputs; ++i)
        {
            color_formats[i] = VK_FORMAT_UNDEFINED;
        }
        depth_stencil_format = VK_FORMAT_UNDEFINED;
        color_operation = depth_operation = stencil_operation = RenderPassOperation::DontCare;
        return *this;
    }

    RenderPassOutput& RenderPassOutput::color(VkFormat format)
    {
        color_formats[num_color_formats++] = format;
        return *this;
    }

    RenderPassOutput& RenderPassOutput::depth(VkFormat format)
    {
        depth_stencil_format = format;
        return *this;
    }

    RenderPassOutput& RenderPassOutput::setOperations(RenderPassOperation::Enum color_, RenderPassOperation::Enum depth_, RenderPassOperation::Enum stencil_)
    {
        color_operation   = color_;
        depth_operation   = depth_;
        stencil_operation = stencil_;

        return *this;
    }

    // PipelineCreation ////////////////////////////////////////
    PipelineCreation& PipelineCreation::addDescriptorSetLayout(DescriptorSetLayoutHandle handle)
    {
        descriptor_set_layout[num_active_layouts++] = handle;
        return *this;
    }

    RenderPassOutput& PipelineCreation::renderPassOutput() { return render_pass; }

    // RenderPassCreation //////////////////////////////////////
    RenderPassCreation& RenderPassCreation::reset()
    {
        num_render_targets    = 0;
        depth_stencil_texture = k_invalid_gpu_texture;
        resize                = 0;
        scale_x               = 1.f;
        scale_y               = 1.f;
        color_operation = depth_operation = stencil_operation = RenderPassOperation::DontCare;

        return *this;
    }

    RenderPassCreation& RenderPassCreation::addRenderTexture(GpuTextureHandle texture)
    {
        output_textures[num_render_targets++] = texture;

        return *this;
    }

    RenderPassCreation& RenderPassCreation::setScaling(float scale_x_, float scale_y_, uint8_t resize_)
    {
        scale_x = scale_x_;
        scale_y = scale_y_;
        resize  = resize_;

        return *this;
    }

    RenderPassCreation& RenderPassCreation::setDepthStencilTexture(GpuTextureHandle texture)
    {
        depth_stencil_texture = texture;

        return *this;
    }

    RenderPassCreation& RenderPassCreation::setName(const std::string& name_)
    {
        name = name_;

        return *this;
    }

    RenderPassCreation& RenderPassCreation::setType(RenderPassType::Enum type_)
    {
        type = type_;

        return *this;
    }

    RenderPassCreation& RenderPassCreation::setOperations(RenderPassOperation::Enum color_, RenderPassOperation::Enum depth_, RenderPassOperation::Enum stencil_)
    {
        color_operation   = color_;
        depth_operation   = depth_;
        stencil_operation = stencil_;

        return *this;
    }

    // ExecutionBarrier ////////////////////////////////////////
    ExecutionBarrier& ExecutionBarrier::reset()
    {
        num_image_barriers = num_memory_barriers = 0;
        source_pipeline_stage                    = PipelineStage::DrawIndirect;
        destination_pipeline_stage               = PipelineStage::DrawIndirect;
        return *this;
    }

    ExecutionBarrier& ExecutionBarrier::set(PipelineStage::Enum source, PipelineStage::Enum destination)
    {
        source_pipeline_stage      = source;
        destination_pipeline_stage = destination;

        return *this;
    }

    ExecutionBarrier& ExecutionBarrier::addImageBarrier(const ImageBarrier& image_barrier)
    {
        image_barriers[num_image_barriers++] = image_barrier;

        return *this;
    }

    ExecutionBarrier& ExecutionBarrier::addMemoryBarrier(const MemoryBarrier& memory_barrier)
    {
        memory_barriers[num_memory_barriers++] = memory_barrier;

        return *this;
    }
} // namespace ArchViz