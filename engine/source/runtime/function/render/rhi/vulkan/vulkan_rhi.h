#pragma once
#include "runtime/function/render/rhi/rhi.h"

namespace ArchViz
{
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

    private:
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;

        const std::vector<const char*> s_validationLayers = {"VK_LAYER_KHRONOS_validation"};

        const bool s_enableValidationLayers = false;
    };
} // namespace ArchViz
