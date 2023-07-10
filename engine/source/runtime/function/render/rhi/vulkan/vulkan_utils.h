#pragma once

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>

namespace ArchViz
{
    struct QueueFamilyIndices;
    struct SwapChainSupportDetails;

    class VulkanUtils
    {
    public:
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        static std::vector<const char*> getRequiredExtensions(bool enableValidationLayers);

        static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

        static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

        static bool isSwapChainAdequate(VkPhysicalDevice physical_device, VkSurfaceKHR surface, bool extensions_supported);

        static bool isDeviceSuitable(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

        static bool checkDeviceExtensionSupport(VkPhysicalDevice physical_device, const std::vector<const char*>& deviceExtensions);

        static bool checkBindlessSupport(VkPhysicalDevice physical_device);

        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);

        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);

        static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

        static uint32_t findMemoryType(VkPhysicalDevice physical_device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

        static VkFormat findSupportedFormat(VkPhysicalDevice physical_device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        static VkFormat findDepthFormat(VkPhysicalDevice physical_device);

        static bool hasStencilComponent(VkFormat format);
    };
} // namespace ArchViz
