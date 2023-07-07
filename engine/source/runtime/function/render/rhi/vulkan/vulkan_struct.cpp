#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"

namespace ArchViz
{
    const std::vector<const char*> VulkanConstants::validation_layers = {
#ifdef VULKAN_DEBUG_REPORT
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // VULKAN_DEBUG_REPORT
        "VK_LAYER_KHRONOS_validation",
    };

    const std::vector<const char*> VulkanConstants::device_extensions = {

#ifdef __MACH__
        "VK_KHR_portability_subset",
#endif
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
} // namespace ArchViz