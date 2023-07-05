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

        static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

        static bool isSwapChainAdequate(VkPhysicalDevice device, VkSurfaceKHR surface, bool extensions_supported);

        static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

        static bool checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);

        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);

        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);

        static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
    };

    // void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    // std::vector<const char*> get_required_extensions(bool enableValidationLayers);

    // bool check_validation_layer_support(const std::vector<const char*>& validationLayers);

    // SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR surface);

    // bool is_swap_chain_adequate(VkPhysicalDevice device, VkSurfaceKHR surface, bool extensions_supported);

    // bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface);

    // bool check_device_extension_support(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);

    // QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);

    // VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);

    // VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);

    // VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
} // namespace ArchViz
