#pragma once
#include "runtime/core/meta/reflection/reflection.h"
#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"

#include <vk_mem_alloc.h>
#include <volk.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ArchViz
{
    class VulkanInstance;

    REFLECTION_TYPE(VulkanDeviceCreateInfo)
    CLASS(VulkanDeviceCreateInfo, Fields)
    {
        REFLECTION_BODY(VulkanDeviceCreateInfo)
    public:
        bool validation;
        bool standlone_table;

        std::vector<std::string> validation_layers;
        std::vector<std::string> device_extensions;
    };

    class VulkanDevice //: public std::enable_shared_from_this<VulkanDevice>
    {
    public:
        VulkanDevice()  = default;
        ~VulkanDevice() = default;

        void initialize();
        void clear();

        void wait();

    private:
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();
        void createAssetAllocator();

    public:
        bool m_validation {true};
        bool m_standlone_table {false};

        std::vector<std::string> m_validation_layers;
        std::vector<const char*> m_validation_layers_cstring;

        std::vector<std::string> m_device_extensions;
        std::vector<const char*> m_device_extensions_cstring;

        bool m_bindless_support {false};

        std::shared_ptr<VulkanInstance> m_instance {nullptr};

        VkPhysicalDevice m_physical_device {VK_NULL_HANDLE};
        VkDevice         m_device {VK_NULL_HANDLE};

        VkPhysicalDeviceProperties           m_properties {};
        VkPhysicalDeviceFeatures             m_features {};
        VkPhysicalDeviceFeatures             m_enabled_features {};
        VkPhysicalDeviceMemoryProperties     m_memory_properties {};
        std::vector<VkQueueFamilyProperties> m_queue_family_properties {};
        std::vector<std::string>             m_supported_extensions {};

        VkCommandPool m_command_pool {VK_NULL_HANDLE};

        QueueFamilyIndices m_indices {};

        VkQueue m_graphics_queue {VK_NULL_HANDLE};
        VkQueue m_compute_queue {VK_NULL_HANDLE};
        VkQueue m_present_queue {VK_NULL_HANDLE};
        VkQueue m_transfer_queue {VK_NULL_HANDLE};

        // asset allocator use VMA library
        VmaAllocator m_vma_allocator {nullptr};
    };
} // namespace ArchViz
