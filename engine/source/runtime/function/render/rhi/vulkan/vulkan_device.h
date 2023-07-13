#pragma once

#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"

#include <volk.h>

#include <vk_mem_alloc.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ArchViz
{
    class VulkanInstance;

    class VulkanDevice
    {
    public:
        explicit VulkanDevice(bool enable_validation) : m_enable_validation_layers(enable_validation) {}
        ~VulkanDevice() = default;

        void connect(std::shared_ptr<VulkanInstance> instance);
        void initialize();
        void clear();

        void wait();

    private:
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();
        void createAssetAllocator();

    public:
        bool m_enable_validation_layers;
        bool m_bindless_support;

        std::shared_ptr<VulkanInstance> m_instance;

        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice         m_device          = VK_NULL_HANDLE;

        VkPhysicalDeviceProperties           m_properties;
        VkPhysicalDeviceFeatures             m_features;
        VkPhysicalDeviceFeatures             m_enabled_features;
        VkPhysicalDeviceMemoryProperties     m_memory_properties;
        std::vector<VkQueueFamilyProperties> m_queue_family_properties;
        std::vector<std::string>             m_supported_extensions;

        VkCommandPool   m_command_pool   = VK_NULL_HANDLE;

        QueueFamilyIndices m_indices;

        VkQueue m_graphics_queue = VK_NULL_HANDLE;
        VkQueue m_compute_queue  = VK_NULL_HANDLE;
        VkQueue m_present_queue  = VK_NULL_HANDLE;
        VkQueue m_transfer_queue = VK_NULL_HANDLE;

        // asset allocator use VMA library
        VmaAllocator m_assets_allocator = nullptr;

        // TODO
    };
} // namespace ArchViz
