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
    using FrameGraphHandle = uint32_t;

    struct FrameGraphResourceHandle
    {
        FrameGraphHandle index {k_invalid_index};
    };

    struct FrameGraphNodeHandle
    {
        FrameGraphHandle index {k_invalid_index};
    };

    struct FrameGraphRenderPassHandle
    {
        FrameGraphHandle index {k_invalid_index};
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
        FrameGraphResourceHandle output_handle;

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

        META(Enable)
        std::string type_name {};
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

        META(Enable)
        std::string type_name {};
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

    REFLECTION_TYPE(FrameGraphCreation)
    CLASS(FrameGraphCreation, Fields)
    {
        REFLECTION_BODY(FrameGraphCreation)

    public:
        std::vector<FrameGraphNodeCreation> nodes;

        std::string name {};
    };

    // Interact with vulkan render pass
    struct FrameGraphRenderPass
    {
        void preRender() {}
        void render() {}
    };

} // namespace ArchViz
