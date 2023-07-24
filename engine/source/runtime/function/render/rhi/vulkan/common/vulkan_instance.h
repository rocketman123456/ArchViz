#pragma once

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ArchViz
{
    struct VulkanInstanceCreateInfo
    {
        bool        validation;
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
