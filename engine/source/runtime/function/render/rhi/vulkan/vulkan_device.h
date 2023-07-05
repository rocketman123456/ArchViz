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
        void clear();

        void wait();

    private:
        void pickPhysicalDevice();
        void createLogicalDevice();

    public:
        bool             m_enable_validation_layers;
        VkInstance       m_instance;
        VkSurfaceKHR     m_surface;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice         m_device          = VK_NULL_HANDLE;

        bool m_bindless_support = false;

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
        VkQueue m_transfer_queue;

        // TODO
    };
} // namespace ArchViz
