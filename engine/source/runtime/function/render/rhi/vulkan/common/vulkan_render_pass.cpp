#include "runtime/function/render/rhi/vulkan/common/vulkan_render_pass.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_utils.h"

#include "runtime/core/base/macro.h"

#include <array>

namespace ArchViz
{
    void VulkanRenderPass::initialize()
    {
        // TODO : use enum to decide what attachment should use
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

        // TODO : use enum to decide what attachment should use
        VkAttachmentDescription depth_attachment {};
        depth_attachment.format         = VulkanUtils::findDepthFormat(m_device->m_physical_device);
        depth_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_attachment_ref {};
        depth_attachment_ref.attachment = 1;
        depth_attachment_ref.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        std::array<VkSubpassDescription, 2> subpasses {};

        subpasses[0].pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpasses[0].colorAttachmentCount    = 1;
        subpasses[0].pColorAttachments       = &color_attachment_ref;
        subpasses[0].pDepthStencilAttachment = &depth_attachment_ref;
        subpasses[0].inputAttachmentCount    = 0;
        subpasses[0].pInputAttachments       = nullptr;
        subpasses[0].preserveAttachmentCount = 0;
        subpasses[0].pPreserveAttachments    = nullptr;
        subpasses[0].pResolveAttachments     = nullptr;

        subpasses[1].pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpasses[1].colorAttachmentCount    = 1;
        subpasses[1].pColorAttachments       = &color_attachment_ref;
        subpasses[1].inputAttachmentCount    = 0;
        subpasses[1].pInputAttachments       = nullptr;
        subpasses[1].preserveAttachmentCount = 0;
        subpasses[1].pPreserveAttachments    = nullptr;
        subpasses[1].pResolveAttachments     = nullptr;

        std::array<VkSubpassDependency, 3> dependencies;

        dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass      = 0;
        dependencies[0].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].srcAccessMask   = 0;
        dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = 0;

        dependencies[1].srcSubpass      = VK_SUBPASS_EXTERNAL;
        dependencies[1].dstSubpass      = 0;
        dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependencies[1].srcAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        dependencies[1].dependencyFlags = 0;

        dependencies[2].srcSubpass      = 0;
        dependencies[2].dstSubpass      = 1;
        dependencies[2].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[2].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[2].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[2].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        dependencies[2].dependencyFlags = 0;

        std::array<VkAttachmentDescription, 2> attachments = {color_attachment, depth_attachment};

        VkRenderPassCreateInfo render_pass_info {};
        render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        render_pass_info.pAttachments    = attachments.data();
        render_pass_info.subpassCount    = static_cast<uint32_t>(subpasses.size());
        render_pass_info.pSubpasses      = subpasses.data();
        render_pass_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
        render_pass_info.pDependencies   = dependencies.data();

        if (vkCreateRenderPass(m_device->m_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create render pass!");
        }
    }

    void VulkanRenderPass::clear() { vkDestroyRenderPass(m_device->m_device, m_render_pass, nullptr); }
} // namespace ArchViz