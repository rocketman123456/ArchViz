#pragma once

#include <volk.h>

#include <cstdint>
#include <string>
#include <vector>

namespace ArchViz
{
    class VulkanDevice
    {
    public:
        explicit VulkanDevice(VkPhysicalDevice physical_device);
        ~VulkanDevice();

    public:
        /** @brief Physical device representation */
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        /** @brief Logical device representation (application's view of the device) */
        VkDevice m_logical_device = VK_NULL_HANDLE;
        /** @brief Properties of the physical device including limits that the application can check against */
        VkPhysicalDeviceProperties m_properties;
        /** @brief Features of the physical device that an application can use to check if a feature is supported */
        VkPhysicalDeviceFeatures m_features;
        /** @brief Features that have been enabled for use on the physical device */
        VkPhysicalDeviceFeatures m_enabled_features;
        /** @brief Memory types and heaps of the physical device */
        VkPhysicalDeviceMemoryProperties m_memory_properties;
        /** @brief Queue family properties of the physical device */
        std::vector<VkQueueFamilyProperties> m_queue_family_properties;
        /** @brief List of extensions supported by the device */
        std::vector<std::string> m_supported_extensions;
        /** @brief Default command pool for the graphics queue family index */
        VkCommandPool m_command_pool = VK_NULL_HANDLE;
        /** @brief Contains queue family indices */
        struct QueueFamilyIndices
        {
            uint32_t m_graphics;
            uint32_t m_compute;
            uint32_t m_transfer;
        } m_queue_family_indices;

        // TODO
    };
} // namespace ArchViz
