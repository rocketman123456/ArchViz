#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ArchViz
{
    REFLECTION_TYPE(VulkanInstanceCreateInfo)
    CLASS(VulkanInstanceCreateInfo, Fields)
    {
        REFLECTION_BODY(VulkanInstanceCreateInfo)
    public:
        bool validation;

        META(Disable)
        GLFWwindow* window;
    };

    class VulkanInstance
    {
    public:
        void initialize();
        void clear();

    private:
        void createInstance();
        void setupDebugMessenger();
        void initSurface();

    public:
        bool m_validation {true};

        GLFWwindow* m_window {nullptr};

        uint32_t m_vulkan_api_version {VK_API_VERSION_1_2};

        VkInstance               m_instance {VK_NULL_HANDLE};
        VkDebugUtilsMessengerEXT m_debug_messenger {VK_NULL_HANDLE};
        VkDebugReportCallbackEXT m_report_callback {VK_NULL_HANDLE};

        VkSurfaceKHR m_surface {VK_NULL_HANDLE};
    };
} // namespace ArchViz
