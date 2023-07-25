#include "runtime/function/render/rhi/vulkan/common/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_instance.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_utils.h"

#include "runtime/core/base/macro.h"

#include <set>

namespace ArchViz
{
    void VulkanDevice::wait()
    {
        ASSERT(m_device);
        vkDeviceWaitIdle(m_device);
    }

    void VulkanDevice::initialize()
    {
        pickPhysicalDevice();
        createLogicalDevice();
        createCommandPool();
        createAssetAllocator();
    }

    void VulkanDevice::clear()
    {
        vmaDestroyAllocator(m_vma_allocator);

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
        vkEnumeratePhysicalDevices(m_instance->m_instance, &device_count, nullptr);

        if (device_count == 0)
        {
            LOG_FATAL("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_instance->m_instance, &device_count, devices.data());

        for (const auto& device : devices)
        {
            VkPhysicalDeviceFeatures supported_features;
            vkGetPhysicalDeviceFeatures(device, &supported_features);

            bool suitable  = VulkanUtils::isDeviceSuitable(device, m_instance->m_surface);
            bool extension = VulkanUtils::checkDeviceExtensionSupport(device, m_device_extensions_cstring);
            bool adequate  = VulkanUtils::isSwapChainAdequate(device, m_instance->m_surface, extension);
            bool bindless  = VulkanUtils::checkBindlessSupport(device);
            if (suitable && extension && adequate && bindless && supported_features.samplerAnisotropy)
            {
                // m_bindless_support = bindless;
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
            // LOG_DEBUG("[extension] supported vulkan extensions list:");
            std::vector<VkExtensionProperties> extensions(extension_count);
            if (vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extension_count, &extensions.front()) == VK_SUCCESS)
            {
                for (const auto& ext : extensions)
                {
                    // LOG_DEBUG("    supported vulkan extensions: {}", ext.extensionName);
                    m_supported_extensions.push_back(ext.extensionName);
                }
            }
        }
    }

    void VulkanDevice::createLogicalDevice()
    {
        m_indices = VulkanUtils::findQueueFamilies(m_physical_device, m_instance->m_surface);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t>                   unique_queue_families = {m_indices.m_graphics_family.value(), m_indices.m_compute_family.value(), m_indices.m_present_family.value()};

        float queue_priority = 1.0f;
        for (uint32_t queue_family : unique_queue_families)
        {
            VkDeviceQueueCreateInfo queue_create_info {};
            queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount       = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features {};
        device_features.samplerAnisotropy = VK_TRUE;

        VkPhysicalDeviceFeatures2 physical_features2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
        vkGetPhysicalDeviceFeatures2(m_physical_device, &physical_features2);

        VkDeviceCreateInfo create_info {};
        create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pQueueCreateInfos       = queue_create_infos.data();
        create_info.queueCreateInfoCount    = static_cast<uint32_t>(queue_create_infos.size());
        create_info.enabledExtensionCount   = static_cast<uint32_t>(m_device_extensions_cstring.size());
        create_info.ppEnabledExtensionNames = m_device_extensions_cstring.data();
        create_info.pEnabledFeatures        = nullptr;
        // create_info.pEnabledFeatures        = &device_features;
        create_info.pNext = &physical_features2;

        if (m_validation)
        {
            create_info.enabledLayerCount   = static_cast<uint32_t>(m_validation_layers_cstring.size());
            create_info.ppEnabledLayerNames = m_validation_layers_cstring.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        // Query bindless extension, called Descriptor Indexing (https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VK_EXT_descriptor_indexing.html)
        VkPhysicalDeviceDescriptorIndexingFeatures indexing_features {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES, nullptr};
        VkPhysicalDeviceFeatures2                  device_features_2 {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &indexing_features};

        vkGetPhysicalDeviceFeatures2(m_physical_device, &device_features_2);

        m_bindless_support = indexing_features.descriptorBindingPartiallyBound && indexing_features.runtimeDescriptorArray;

        if (m_bindless_support)
        {
            indexing_features.descriptorBindingPartiallyBound = VK_TRUE;
            indexing_features.runtimeDescriptorArray          = VK_TRUE;

            physical_features2.pNext = &indexing_features;
        }

        if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create logical device!");
        }

        volkLoadDevice(m_device);

        vkGetDeviceQueue(m_device, m_indices.m_graphics_family.value(), 0, &m_graphics_queue);
        vkGetDeviceQueue(m_device, m_indices.m_compute_family.value(), 0, &m_compute_queue);
        vkGetDeviceQueue(m_device, m_indices.m_present_family.value(), 0, &m_present_queue);
        vkGetDeviceQueue(m_device, m_indices.m_transfer_family.value(), 0, &m_transfer_queue);
    }

    void VulkanDevice::createCommandPool()
    {
        VkCommandPoolCreateInfo pool_info {};
        pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = m_indices.m_graphics_family.value();

        if (vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create command pool!");
        }
    }

    void VulkanDevice::createAssetAllocator()
    {
        VmaVulkanFunctions vulkan_functions    = {};
        vulkan_functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vulkan_functions.vkGetDeviceProcAddr   = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocator_create_info = {};
        allocator_create_info.vulkanApiVersion       = m_instance->m_vulkan_api_version;
        allocator_create_info.instance               = m_instance->m_instance;
        allocator_create_info.physicalDevice         = m_physical_device;
        allocator_create_info.device                 = m_device;
        allocator_create_info.pVulkanFunctions       = &vulkan_functions;

        vmaCreateAllocator(&allocator_create_info, &m_vma_allocator);
    }
} // namespace ArchViz
