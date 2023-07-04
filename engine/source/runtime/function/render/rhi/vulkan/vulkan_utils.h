#pragma once

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>

namespace ArchViz
{
    struct QueueFamilyIndices;
    struct SwapChainSupportDetails;

    void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    std::vector<const char*> get_required_extensions(bool enableValidationLayers);

    bool check_validation_layer_support(const std::vector<const char*>& validationLayers);

    SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface);

    bool is_swap_chain_adequate(VkPhysicalDevice device, VkSurfaceKHR surface, bool extensions_supported);

    bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface);

    bool check_device_extension_support(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);

    QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);

    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);

    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);

    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
} // namespace ArchViz
