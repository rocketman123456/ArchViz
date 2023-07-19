#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include "runtime/function/render/rhi/gpu_enum.h"
#include "runtime/function/render/rhi/gpu_resources.h"

#include <volk.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ArchViz
{
    // class VulkanBuffer;
    // class VulkanTexture;

    using FrameGraphHandle = uint32_t;

    struct FrameGraphResourceHandle
    {
        FrameGraphHandle index;
    };

    struct FrameGraphNodeHandle
    {
        FrameGraphHandle index;
    };

    struct FrameGraphRenderPassHandle
    {
        FrameGraphHandle index;
    };

    enum class FrameGraphResourceType : int32_t
    {
        Invalid    = -1,
        Buffer     = 0,
        Texture    = 1,
        Attachment = 2,
        Reference  = 3
    };

    struct FrameGraphBuffer
    {
        size_t                size;
        VkBufferUsageFlags    flags;
        VkMemoryPropertyFlags properties;

        BufferHandle buffer;
    };

    struct FrameGraphTexture
    {
        uint32_t width;
        uint32_t height;
        uint32_t depth;

        VkFormat          format;
        VkImageUsageFlags flags;

        RenderPassOperation::Enum load_op;

        TextureHandle texture;
    };

    struct FrameGraphResourceInfo
    {
        bool external = false;

        union
        {
            FrameGraphBuffer  buffer;
            FrameGraphTexture texture;
        };
    };

    struct FrameGraphResource
    {
        FrameGraphResourceType type;
        FrameGraphResourceInfo resource_info;

        FrameGraphNodeHandle     producer;
        FrameGraphResourceHandle output_andle;

        int32_t ref_count {0};

        std::string name {};
    };

    REFLECTION_TYPE(FrameGraphResourceInputCreation)
    CLASS(FrameGraphResourceInputCreation, WhiteListFields)
    {
        REFLECTION_BODY(FrameGraphResourceInputCreation)
    public:
        FrameGraphResourceType type;
        FrameGraphResourceInfo resourceInfo;

        META(Enable)
        std::string name {};
    };

    REFLECTION_TYPE(FrameGraphResourceOutputCreation)
    CLASS(FrameGraphResourceOutputCreation, WhiteListFields)
    {
        REFLECTION_BODY(FrameGraphResourceOutputCreation)
    public:
        FrameGraphResourceType type;
        FrameGraphResourceInfo resource_info;

        META(Enable)
        std::string name {};
    };

    REFLECTION_TYPE(FrameGraphNodeCreation)
    CLASS(FrameGraphNodeCreation, Fields)
    {
        REFLECTION_BODY(FrameGraphNodeCreation)

    public:
        std::vector<FrameGraphResourceInputCreation>  inputs;
        std::vector<FrameGraphResourceOutputCreation> outputs;

        bool enabled {false};

        std::string name {};
    };

    // Interact with vulkan render pass
    struct FrameGraphRenderPass
    {
        void preRender() {}
        void render() {}
    };

    struct FrameGraphNode
    {
        int32_t ref_count {0};

        // TODO: render pass handle
        uint32_t render_pass;
        // TODO: framebuffer handle
        uint32_t framebuffer;

        std::vector<FrameGraphResourceHandle> inputs;
        std::vector<FrameGraphResourceHandle> outputs;

        std::vector<FrameGraphNodeHandle> edges_forward;
        std::vector<FrameGraphNodeHandle> edges_backward;

        bool enabled {true};

        std::string name;
    };

} // namespace ArchViz
