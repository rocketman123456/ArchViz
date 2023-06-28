#pragma once

#include <volk.h>

#include <vector>

namespace ArchViz
{
    struct QueueFamilyIndices;
    struct SwapChainSupportDetails;

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    std::vector<const char*> getRequiredExtensions(bool enableValidationLayers);

    bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

    bool isSwapChainAdequate(VkPhysicalDevice device, VkSurfaceKHR surface, bool extensions_supported);

    bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

} // namespace ArchViz
