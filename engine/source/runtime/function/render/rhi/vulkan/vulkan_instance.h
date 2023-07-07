#pragma once

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ArchViz
{
    class VulkanInstance
    {
    public:
        explicit VulkanInstance(bool validation);

        void connect(GLFWwindow* window);
        void initialize();
        void clear();

    private:
        void createInstance();
        void setupDebugMessenger();
        void initSurface();

    public:
        bool m_validation;

        GLFWwindow* m_window;

        uint32_t m_vulkan_api_version {VK_API_VERSION_1_0};

        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;
        VkDebugReportCallbackEXT m_report_callback; // TODO

        // TODO
        VkSurfaceKHR m_surface;
    };
} // namespace ArchViz
