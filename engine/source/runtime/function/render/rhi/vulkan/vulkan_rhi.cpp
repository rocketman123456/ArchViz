#include "runtime/function/render/rhi/vulkan/vulkan_rhi.h"
#include "runtime/function/render/rhi/vulkan/vulkan_utils.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/vulkan_swap_chain.h"

#include "runtime/function/window/window_system.h"

#include "runtime/core/base/macro.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>
#include <set>

VkResult CreateDebugUtilsMessengerEXT(VkInstance                                instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks*              pAllocator,
                                      VkDebugUtilsMessengerEXT*                 pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

namespace ArchViz
{
    void VulkanRHI::createInstance()
    {
        if (volkInitialize() != VK_SUCCESS)
        {
            LOG_FATAL("failed to initialize volk!");
        }

        if (m_enable_validation_layers && !checkValidationLayerSupport(m_validation_layers))
        {
            LOG_FATAL("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo {};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "ArchViz";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "ArchViz Engine";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_0;

        VkInstanceCreateInfo create_info {};
        create_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &appInfo;

        auto extensions                     = getRequiredExtensions(m_enable_validation_layers);
        create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
        if (m_enable_validation_layers)
        {
            create_info.enabledLayerCount   = static_cast<uint32_t>(m_validation_layers.size());
            create_info.ppEnabledLayerNames = m_validation_layers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            create_info.enabledLayerCount = 0;
            create_info.pNext             = nullptr;
        }

        if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create instance!");
        }

        volkLoadInstance(m_instance);
    }

    void VulkanRHI::setupDebugMessenger()
    {
        if (!m_enable_validation_layers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT create_info;
        populateDebugMessengerCreateInfo(create_info);

        if (CreateDebugUtilsMessengerEXT(m_instance, &create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
        {
            LOG_FATAL("failed to set up debug messenger!");
        }
    }

    void VulkanRHI::createSurface()
    {
        if (glfwCreateWindowSurface(m_instance, m_initialize_info.window_system->getWindow(), nullptr, &m_surface) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create window surface!");
        }
    }

    void VulkanRHI::pickPhysicalDevice()
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
            bool extension = checkDeviceExtensionSupport(device, m_device_extensions);
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

        m_vulkan_device = std::make_shared<VulkanDevice>(m_physical_device);
    }

    void VulkanRHI::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(m_physical_device, m_surface);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {indices.m_graphics_family.value(), indices.m_compute_family.value(), indices.m_present_family.value()};

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

        create_info.enabledExtensionCount   = static_cast<uint32_t>(m_device_extensions.size());
        create_info.ppEnabledExtensionNames = m_device_extensions.data();

        if (m_enable_validation_layers)
        {
            create_info.enabledLayerCount   = static_cast<uint32_t>(m_validation_layers.size());
            create_info.ppEnabledLayerNames = m_validation_layers.data();
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

        vkGetDeviceQueue(m_device, indices.m_graphics_family.value(), 0, &m_graphics_queue);
        vkGetDeviceQueue(m_device, indices.m_compute_family.value(), 0, &m_compute_queue);
        vkGetDeviceQueue(m_device, indices.m_present_family.value(), 0, &m_present_queue);
    }

    void VulkanRHI::createSwapChain()
    {
        SwapChainSupportDetails swap_chain_support = querySwapChainSupport(m_physical_device, m_surface);

        VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(swap_chain_support.formats);
        VkPresentModeKHR   present_mode   = chooseSwapPresentMode(swap_chain_support.presentModes);
        VkExtent2D         extent         = chooseSwapExtent(swap_chain_support.capabilities, m_initialize_info.window_system->getWindow());

        uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
        if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
        {
            image_count = swap_chain_support.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR create_info {};
        create_info.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = m_surface;

        create_info.minImageCount    = image_count;
        create_info.imageFormat      = surface_format.format;
        create_info.imageColorSpace  = surface_format.colorSpace;
        create_info.imageExtent      = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices              = findQueueFamilies(m_physical_device, m_surface);
        uint32_t           queueFamilyIndices[] = {indices.m_graphics_family.value(), indices.m_present_family.value()};

        if (indices.m_graphics_family != indices.m_present_family)
        {
            create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices   = queueFamilyIndices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        create_info.preTransform   = swap_chain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode    = present_mode;
        create_info.clipped        = VK_TRUE;

        create_info.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, nullptr);
        m_swap_chain_images.resize(image_count);
        vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, m_swap_chain_images.data());

        m_swap_chain_image_format = surface_format.format;
        m_swap_chain_extent       = extent;
    }

    void VulkanRHI::createImageViews()
    {
        m_swap_chain_image_views.resize(m_swap_chain_images.size());

        for (size_t i = 0; i < m_swap_chain_images.size(); i++)
        {
            VkImageViewCreateInfo create_info {};
            create_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image                           = m_swap_chain_images[i];
            create_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format                          = m_swap_chain_image_format;
            create_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel   = 0;
            create_info.subresourceRange.levelCount     = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount     = 1;

            if (vkCreateImageView(m_device, &create_info, nullptr, &m_swap_chain_image_views[i]) != VK_SUCCESS)
            {
                LOG_FATAL("failed to create image views!");
            }
        }
    }

    void VulkanRHI::createGraphicsPipeline() {}

    void VulkanRHI::initialize(RHIInitInfo initialize_info)
    {
        m_initialize_info = initialize_info;

        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createGraphicsPipeline();
    }

    void VulkanRHI::prepareContext() {}

    void VulkanRHI::clear()
    {
        for (auto image_view : m_swap_chain_image_views)
        {
            vkDestroyImageView(m_device, image_view, nullptr);
        }

        vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr);
        vkDestroyDevice(m_device, nullptr);

        if (m_enable_validation_layers)
        {
            DestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        }

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }
} // namespace ArchViz
