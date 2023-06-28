#pragma once
#include "runtime/function/render/rhi/rhi.h"

#include <optional>

namespace ArchViz
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> m_graphics_family;
        std::optional<uint32_t> m_present_family;
        std::optional<uint32_t> m_compute_family;

        // bool isComplete() { return m_graphics_family.has_value() && m_present_family.has_value() && m_compute_family.has_value(); }
        bool isComplete() { return m_graphics_family.has_value() && m_compute_family.has_value(); }
    };

    class VulkanRHI : public RHI
    {
    public:
        VulkanRHI()          = default;
        virtual ~VulkanRHI() = default;

        void initialize(RHIInitInfo initialize_info) override;
        void prepareContext() override;

        void clear() override;

    private:
        void createInstance();
        void setupDebugMessenger();
        void pickPhysicalDevice();

        bool isDeviceSuitable(VkPhysicalDevice device);

    private:
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;

        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

        const std::vector<const char*> s_validation_layers = {"VK_LAYER_KHRONOS_validation"};

        const bool s_enable_validation_layers = false;
    };
} // namespace ArchViz
