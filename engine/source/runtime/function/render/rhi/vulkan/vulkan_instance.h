#pragma once

#include <volk.h>

namespace ArchViz
{
    class VulkanInstance
    {
    public:
        void initialize();
        void clear();

    private:
        void createInstance();
        void setupDebugMessenger();

    public:
        const bool m_enable_validation_layers = false;

        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;
    };
} // namespace ArchViz
