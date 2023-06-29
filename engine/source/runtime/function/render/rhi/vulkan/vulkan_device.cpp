#include "runtime/function/render/rhi/vulkan/vulkan_device.h"

#include "runtime/core/base/macro.h"

namespace ArchViz
{
    /**
     * Default constructor
     *
     * @param physicalDevice Physical device that is to be used
     */
    VulkanDevice::VulkanDevice(VkPhysicalDevice physical_device)
    {
        ASSERT(physicalDevice);
        this->m_physical_device = physical_device;

        // Store Properties features, limits and properties of the physical device for later use
        // Device properties also contain limits and sparse properties
        vkGetPhysicalDeviceProperties(m_physical_device, &m_properties);
        // Features should be checked by the examples before using them
        vkGetPhysicalDeviceFeatures(m_physical_device, &m_features);
        // Memory properties are used regularly for creating all kinds of buffers
        vkGetPhysicalDeviceMemoryProperties(m_physical_device, &m_memory_properties);
        // Queue family properties, used for setting up requested queues upon device creation
        uint32_t queue_family_count;
        vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queue_family_count, nullptr);
        ASSERT(queue_family_count > 0);
        m_queue_family_properties.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queue_family_count, m_queue_family_properties.data());

        // Get list of supported extensions
        uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extension_count, nullptr);
        if (extension_count > 0)
        {
            std::vector<VkExtensionProperties> extensions(extension_count);
            if (vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extension_count, &extensions.front()) == VK_SUCCESS)
            {
                for (auto ext : extensions)
                {
                    m_supported_extensions.push_back(ext.extensionName);
                }
            }
        }
    }

    /**
     * Default destructor
     *
     * @note Frees the logical device
     */
    VulkanDevice::~VulkanDevice()
    {
        if (m_command_pool)
        {
            vkDestroyCommandPool(m_logical_device, m_command_pool, nullptr);
        }
        if (m_logical_device)
        {
            vkDestroyDevice(m_logical_device, nullptr);
        }
    }
} // namespace ArchViz
