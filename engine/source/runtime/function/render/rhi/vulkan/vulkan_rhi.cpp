#include "runtime/function/render/rhi/vulkan/vulkan_rhi.h"
#include "runtime/function/render/rhi/vulkan/vulkan_utils.h"

#include "runtime/function/window/window_system.h"

#include "runtime/core/base/macro.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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
        appInfo.pApplicationName   = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "No Engine";
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

    void VulkanRHI::initialize(RHIInitInfo initialize_info)
    {
        m_initialize_info = initialize_info;

        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void VulkanRHI::prepareContext() {}

    void VulkanRHI::clear()
    {
        vkDestroyDevice(m_device, nullptr);

        if (m_enable_validation_layers)
        {
            DestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        }

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }
} // namespace ArchViz
