#include "runtime/function/render/rhi/vulkan/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/vulkan_utils.h"

#include "runtime/core/base/macro.h"

#include <set>

namespace ArchViz
{
    void VulkanDevice::connect(VkInstance instance, VkSurfaceKHR surface)
    {
        ASSERT(instance);
        m_instance = instance;
        ASSERT(surface);
        m_surface = surface;
    }

    void VulkanDevice::wait()
    {
        ASSERT(m_device);
        vkDeviceWaitIdle(m_device);
    }

    void VulkanDevice::initialize()
    {
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void VulkanDevice::clear()
    {
        if (m_command_pool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_device, m_command_pool, nullptr);
        }
        if (m_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_device, nullptr);
        }
    }

    void VulkanDevice::pickPhysicalDevice()
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

        if (device_count == 0)
        {
            LOG_FATAL("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

        for (const auto& device : devices)
        {
            bool suitable  = isDeviceSuitable(device, m_surface);
            bool extension = checkDeviceExtensionSupport(device, VulkanConstants::device_extensions);
            bool adequate  = isSwapChainAdequate(device, m_surface, extension);
            if (suitable && extension && adequate)
            {
                m_physical_device = device;
                break;
            }
        }

        if (m_physical_device == VK_NULL_HANDLE)
        {
            LOG_FATAL("failed to find a suitable GPU!");
        }

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
                    // LOG_DEBUG("supported vulkan extensions: {}", ext.extensionName);
                    m_supported_extensions.push_back(ext.extensionName);
                }
            }
        }
    }

    void VulkanDevice::createLogicalDevice()
    {
        m_indices = findQueueFamilies(m_physical_device, m_surface);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t>                   unique_queue_families = {
            m_indices.m_graphics_family.value(), m_indices.m_compute_family.value(), m_indices.m_present_family.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : unique_queue_families)
        {
            VkDeviceQueueCreateInfo queue_create_info {};
            queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queueFamily;
            queue_create_info.queueCount       = 1;
            queue_create_info.pQueuePriorities = &queuePriority;
            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features {};

        VkDeviceCreateInfo create_info {};
        create_info.sType                 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pQueueCreateInfos     = queue_create_infos.data();
        create_info.queueCreateInfoCount  = static_cast<uint32_t>(queue_create_infos.size());
        create_info.pEnabledFeatures      = &device_features;
        create_info.enabledExtensionCount = 0;

        create_info.enabledExtensionCount   = static_cast<uint32_t>(VulkanConstants::device_extensions.size());
        create_info.ppEnabledExtensionNames = VulkanConstants::device_extensions.data();

        if (m_enable_validation_layers)
        {
            create_info.enabledLayerCount   = static_cast<uint32_t>(VulkanConstants::validation_layers.size());
            create_info.ppEnabledLayerNames = VulkanConstants::validation_layers.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create logical device!");
        }

        volkLoadDevice(m_device);

        vkGetDeviceQueue(m_device, m_indices.m_graphics_family.value(), 0, &m_graphics_queue);
        vkGetDeviceQueue(m_device, m_indices.m_compute_family.value(), 0, &m_compute_queue);
        vkGetDeviceQueue(m_device, m_indices.m_present_family.value(), 0, &m_present_queue);
    }
} // namespace ArchViz
