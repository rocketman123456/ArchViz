#pragma once

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ArchViz
{
    class VulkanUI
    {
    public:
        void connect();
        void initialize();
        void clear();

        void prepareContext();
        void render();
        void recordCommandBuffer(VkCommandBuffer command_buffer, VkFramebuffer frame_buffer, uint32_t width, uint32_t height);

    public:
        GLFWwindow* m_window;

        VkInstance       m_instance;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice         m_device;
        uint32_t         m_graphics_queue_family;
        VkQueue          m_graphics_queue;
        uint32_t         m_image_count; // swap chain
        VkFormat         m_image_format;

        VkDescriptorPool      m_descriptor_pool;
        VkDescriptorSetLayout m_descriptor_set_layout;
        VkDescriptorSet       m_descriptor_set;

        VkCommandPool   m_command_pool;
        VkCommandBuffer m_command_buffer;

        VkRenderPass m_ui_pass;
    };
} // namespace ArchViz
