#include "runtime/function/render/rhi/vulkan/vulkan_render_pass.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"

#include "runtime/core/base/macro.h"

namespace ArchViz
{
    void VulkanRenderPass::setDevice(std::shared_ptr<VulkanDevice> device)
    {
        ASSERT(device);
        m_device = device;
    }

    void VulkanRenderPass::initialize()
    {
        VkAttachmentDescription color_attachment {};
        color_attachment.format         = m_color_format;
        color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &color_attachment_ref;

        VkRenderPassCreateInfo render_pass_info {};
        render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments    = &color_attachment;
        render_pass_info.subpassCount    = 1;
        render_pass_info.pSubpasses      = &subpass;

        if (vkCreateRenderPass(m_device->m_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create render pass!");
        }
    }

    void VulkanRenderPass::clear() { vkDestroyRenderPass(m_device->m_device, m_render_pass, nullptr); }
} // namespace ArchViz