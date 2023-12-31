#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

namespace ArchViz
{
    REFLECTION_TYPE(VulkanInstanceCreateInfo)
    CLASS(VulkanInstanceCreateInfo, Fields)
    {
        REFLECTION_BODY(VulkanInstanceCreateInfo)
    public:
        bool validation;
        bool standlone_table;
        bool debug_utils_callback;
        bool debug_report_callback;

        std::string name;
        std::string engine_name;

        std::vector<std::string> validation_layers;
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
        bool m_standlone_table {false};
        bool m_debug_utils_callback {true};
        bool m_debug_report_callback {false};

        std::vector<std::string> m_validation_layers;
        std::vector<const char*> m_validation_layers_cstring;

        std::string m_name;
        std::string m_engine_name;

        GLFWwindow* m_window {nullptr};

        uint32_t m_vulkan_api_version {VK_API_VERSION_1_2};

        VkInstance               m_instance {VK_NULL_HANDLE};
        VkDebugUtilsMessengerEXT m_debug_messenger {VK_NULL_HANDLE};
        VkDebugReportCallbackEXT m_report_callback {VK_NULL_HANDLE};

        VkSurfaceKHR m_surface {VK_NULL_HANDLE};
    };
} // namespace ArchViz
