#include "runtime/function/render/rhi/vulkan/common/vulkan_instance.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_utils.h"
#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"

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
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT      flags,
                                                       VkDebugReportObjectTypeEXT objectType,
                                                       uint64_t                   object,
                                                       size_t                     location,
                                                       int32_t                    messageCode,
                                                       const char*                pLayerPrefix,
                                                       const char*                pMessage,
                                                       void*                      pUserData)
    {
        (void)object;
        (void)location;
        (void)messageCode;
        (void)pUserData;
        (void)pLayerPrefix; // Unused arguments

        if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
        {
            LOG_DEBUG("[vulkan] Debug report from ObjectType: {} : Message: {}", objectType, pMessage);
        }
        else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
        {
            LOG_INFO("[vulkan] Debug report from ObjectType: {} : Message: {}", objectType, pMessage);
        }
        else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            LOG_WARN("[vulkan] Debug report from ObjectType: {} : Message: {}", objectType, pMessage);
        }
        else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        {
            LOG_WARN("[vulkan] Debug report from ObjectType: {} : Message: {}", objectType, pMessage);
        }
        else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            LOG_ERROR("[vulkan] Debug report from ObjectType: {} : Message: {}", objectType, pMessage);
        }

        // fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
        return VK_FALSE;
    }

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

        if (m_validation && !VulkanUtils::checkValidationLayerSupport(m_validation_layers_cstring))
        {
            LOG_FATAL("validation layers requested, but not available!");
        }

        VkApplicationInfo app_info {};
        app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName   = m_name.c_str();
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName        = m_engine_name.c_str();
        app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion         = m_vulkan_api_version;

        VkInstanceCreateInfo create_info {};
        create_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;

        auto extensions                     = VulkanUtils::getRequiredExtensions(m_validation);
        create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();

#ifdef __MACH__
        create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
        if (m_validation)
        {
            create_info.enabledLayerCount   = static_cast<uint32_t>(m_validation_layers_cstring.size());
            create_info.ppEnabledLayerNames = m_validation_layers_cstring.data();

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

        if (m_debug_utils_callback)
        {
            VkDebugUtilsMessengerCreateInfoEXT create_info;
            VulkanUtils::populateDebugMessengerCreateInfo(create_info);

            if (create_debug_utils_messenger_ext(m_instance, &create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
            {
                LOG_FATAL("failed to set up debug messenger!");
            }
        }

        if (m_debug_report_callback)
        {
            auto FUNC_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");

            VkDebugReportCallbackCreateInfoEXT debug_report_ci;
            debug_report_ci.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            debug_report_ci.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
            debug_report_ci.pfnCallback = debug_report;
            debug_report_ci.pUserData   = nullptr;
            debug_report_ci.pNext       = nullptr;
            if (FUNC_vkCreateDebugReportCallbackEXT(m_instance, &debug_report_ci, nullptr, &m_report_callback))
            {
                LOG_FATAL("failed to set up debug report!");
            }
        }
    }

    void VulkanInstance::initSurface()
    {
        if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create window surface!");
        }
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
            if (m_debug_utils_callback)
            {
                destroy_debug_utils_messenger_ext(m_instance, m_debug_messenger, nullptr);
            }
            if (m_debug_report_callback)
            {
                vkDestroyDebugReportCallbackEXT(m_instance, m_report_callback, nullptr);
            }
        }

        vkDestroyInstance(m_instance, nullptr);
    }
} // namespace ArchViz
