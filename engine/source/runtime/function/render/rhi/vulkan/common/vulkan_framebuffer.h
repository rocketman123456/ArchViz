#pragma once
#include "runtime/core/base/macro.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"

#include <volk.h>

#include <algorithm>
#include <memory>
#include <vector>

namespace ArchViz
{
    class VulkanDevice;

    /**
     * @brief Encapsulates a single frame buffer attachment
     */
    struct FramebufferAttachment
    {
        VkImage                 image;
        VkDeviceMemory          memory;
        VkImageView             view;
        VkFormat                format;
        VkImageSubresourceRange subresource_range;
        VkAttachmentDescription description;

        /**
         * @brief Returns true if the attachment has a depth component
         */
        bool hasDepth()
        {
            std::vector<VkFormat> formats = {
                VK_FORMAT_D16_UNORM,
                VK_FORMAT_X8_D24_UNORM_PACK32,
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D16_UNORM_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT,
                VK_FORMAT_D32_SFLOAT_S8_UINT,
            };
            return std::find(formats.begin(), formats.end(), format) != std::end(formats);
        }

        /**
         * @brief Returns true if the attachment has a stencil component
         */
        bool hasStencil()
        {
            std::vector<VkFormat> formats = {
                VK_FORMAT_S8_UINT,
                VK_FORMAT_D16_UNORM_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT,
                VK_FORMAT_D32_SFLOAT_S8_UINT,
            };
            return std::find(formats.begin(), formats.end(), format) != std::end(formats);
        }

        /**
         * @brief Returns true if the attachment is a depth and/or stencil attachment
         */
        bool isDepthStencil() { return (hasDepth() || hasStencil()); }
    };

    /**
     * @brief Describes the attributes of an attachment to be created
     */
    struct AttachmentCreateInfo
    {
        uint32_t              width, height;
        uint32_t              layer_count;
        VkFormat              format;
        VkImageUsageFlags     usage;
        VkSampleCountFlagBits image_sample_count = VK_SAMPLE_COUNT_1_BIT;
    };

    /**
     * @brief Encapsulates a complete Vulkan framebuffer with an arbitrary number and combination of attachments
     */
    class Framebuffer
    {
    private:
        std::shared_ptr<VulkanDevice> m_vulkan_device;

    public:
        uint32_t                           m_width;
        uint32_t                           m_height;
        VkFramebuffer                      m_framebuffer;
        VkRenderPass                       m_render_pass;
        VkSampler                          m_sampler;
        std::vector<FramebufferAttachment> m_attachments;

        /**
         * Default constructor
         *
         * @param vulkanDevice Pointer to a valid VulkanDevice
         */
        Framebuffer(std::shared_ptr<VulkanDevice> vulkan_device)
        {
            ASSERT(vulkan_device);
            this->m_vulkan_device = vulkan_device;
        }

        /**
         * Destroy and free Vulkan resources used for the framebuffer and all of its attachments
         */
        ~Framebuffer()
        {
            ASSERT(m_vulkan_device);
            // for (auto attachment : m_attachments)
            // {
            //     vkDestroyImage(m_vulkan_device->m_logical_device, attachment.image, nullptr);
            //     vkDestroyImageView(m_vulkan_device->m_logical_device, attachment.view, nullptr);
            //     vkFreeMemory(m_vulkan_device->m_logical_device, attachment.memory, nullptr);
            // }
            // vkDestroySampler(m_vulkan_device->m_logical_device, m_sampler, nullptr);
            // vkDestroyRenderPass(m_vulkan_device->m_logical_device, m_render_pass, nullptr);
            // vkDestroyFramebuffer(m_vulkan_device->m_logical_device, m_framebuffer, nullptr);
        }
    };
} // namespace ArchViz
