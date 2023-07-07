#include "runtime/function/render/rhi/vulkan/vulkan_instance.h"
#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"
#include "runtime/function/render/rhi/vulkan/vulkan_utils.h"

#include "runtime/core/base/macro.h"

static VkResult
create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

namespace ArchViz
{
    VulkanInstance::VulkanInstance(bool validation) : m_validation(validation) {}

    void VulkanInstance::createInstance()
    {
#if defined(__GNUC__)
        // https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
#if defined(__linux__)
        char const* vk_layer_path = ARCHVIZ_XSTR(PICCOLO_VK_LAYER_PATH);
        setenv("VK_LAYER_PATH", vk_layer_path, 1);
#elif defined(__MACH__)
        // https://developer.apple.com/library/archive/documentation/Porting/Conceptual/PortingUnix/compiling/compiling.html
        char const* vk_layer_path    = ARCHVIZ_XSTR(ARCHVIZ_VK_LAYER_PATH);
        char const* vk_icd_filenames = ARCHVIZ_XSTR(ARCHVIZ_VK_ICD_FILENAMES);
        setenv("VK_LAYER_PATH", vk_layer_path, 1);
        setenv("VK_ICD_FILENAMES", vk_icd_filenames, 1);
#else
#error Unknown Platform
#endif
#endif

        if (volkInitialize() != VK_SUCCESS)
        {
            LOG_FATAL("failed to initialize volk!");
        }

        if (m_validation && !VulkanUtils::checkValidationLayerSupport(VulkanConstants::validation_layers))
        {
            LOG_FATAL("validation layers requested, but not available!");
        }

        VkApplicationInfo app_info {};
        app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName   = "ArchViz";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName        = "ArchViz Engine";
        app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion         = m_vulkan_api_version;

        VkInstanceCreateInfo create_info {};
        create_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;

        auto extensions                     = VulkanUtils::getRequiredExtensions(m_validation);
        create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();

#ifdef __MACH__
        create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
#endif

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
        if (m_validation)
        {
            create_info.enabledLayerCount   = static_cast<uint32_t>(VulkanConstants::validation_layers.size());
            create_info.ppEnabledLayerNames = VulkanConstants::validation_layers.data();

            VulkanUtils::populateDebugMessengerCreateInfo(debugCreateInfo);
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

    void VulkanInstance::setupDebugMessenger()
    {
        if (!m_validation)
            return;

        VkDebugUtilsMessengerCreateInfoEXT create_info;
        VulkanUtils::populateDebugMessengerCreateInfo(create_info);

        if (create_debug_utils_messenger_ext(m_instance, &create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
        {
            LOG_FATAL("failed to set up debug messenger!");
        }
    }

    void VulkanInstance::initSurface()
    {
        if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create window surface!");
        }
    }

    void VulkanInstance::connect(GLFWwindow* window)
    {
        ASSERT(window);
        m_window = window;
    }

    void VulkanInstance::initialize()
    {
        createInstance();
        setupDebugMessenger();
        initSurface();
    }

    void VulkanInstance::clear()
    {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

        if (m_validation)
        {
            destroy_debug_utils_messenger_ext(m_instance, m_debug_messenger, nullptr);
        }

        vkDestroyInstance(m_instance, nullptr);
    }
} // namespace ArchViz
