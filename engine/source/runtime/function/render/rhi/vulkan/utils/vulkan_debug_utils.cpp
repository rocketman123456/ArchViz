#include "runtime/function/render/rhi/vulkan/utils/vulkan_debug_utils.h"

namespace ArchViz
{
    PFN_vkCmdBeginDebugUtilsLabelEXT  vkCmdBeginDebugUtilsLabelEXT {nullptr};
    PFN_vkCmdEndDebugUtilsLabelEXT    vkCmdEndDebugUtilsLabelEXT {nullptr};
    PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT {nullptr};

    void VulkanDebugUtils::setup(VkInstance instance)
    {
        vkCmdBeginDebugUtilsLabelEXT  = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT"));
        vkCmdEndDebugUtilsLabelEXT    = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT"));
        vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT"));
    }

    void VulkanDebugUtils::cmdBeginLabel(VkCommandBuffer cmdbuffer, const std::string& caption, const FVector4& color)
    {
        if (!vkCmdBeginDebugUtilsLabelEXT)
        {
            return;
        }
        VkDebugUtilsLabelEXT labelInfo {};
        labelInfo.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        labelInfo.pLabelName = caption.c_str();
        memcpy(labelInfo.color, &color[0], sizeof(float) * 4);
        vkCmdBeginDebugUtilsLabelEXT(cmdbuffer, &labelInfo);
    }

    void VulkanDebugUtils::cmdEndLabel(VkCommandBuffer cmdbuffer)
    {
        if (!vkCmdEndDebugUtilsLabelEXT)
        {
            return;
        }
        vkCmdEndDebugUtilsLabelEXT(cmdbuffer);
    }
} // namespace ArchViz
