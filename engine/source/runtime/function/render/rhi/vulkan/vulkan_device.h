#pragma once

#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"

#include <volk.h>

#include <cstdint>
#include <string>
#include <vector>

namespace ArchViz
{
    class VulkanDevice
    {
    public:
        explicit VulkanDevice(bool enable_validation) : m_enable_validation_layers(enable_validation) {}

        void connect(VkInstance instance, VkSurfaceKHR surface);
        void initialize();
        void cleanup();

    private:
        void pickPhysicalDevice();
        void createLogicalDevice();

    public:
        bool             m_enable_validation_layers;
        VkInstance       m_instance;
        VkSurfaceKHR     m_surface;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice         m_device  = VK_NULL_HANDLE;

        VkPhysicalDeviceProperties           m_properties;
        VkPhysicalDeviceFeatures             m_features;
        VkPhysicalDeviceFeatures             m_enabled_features;
        VkPhysicalDeviceMemoryProperties     m_memory_properties;
        std::vector<VkQueueFamilyProperties> m_queue_family_properties;
        std::vector<std::string>             m_supported_extensions;

        VkCommandPool m_command_pool = VK_NULL_HANDLE;

        QueueFamilyIndices m_indices;

        VkQueue m_graphics_queue;
        VkQueue m_compute_queue;
        VkQueue m_present_queue;

        const std::vector<const char*> m_validation_layers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> m_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        // TODO
    };
} // namespace ArchViz
