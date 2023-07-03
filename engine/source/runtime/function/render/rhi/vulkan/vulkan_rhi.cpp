#include "runtime/function/render/rhi/vulkan/vulkan_rhi.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/vulkan_instance.h"
#include "runtime/function/render/rhi/vulkan/vulkan_pipeline.h"
#include "runtime/function/render/rhi/vulkan/vulkan_render_pass.h"
#include "runtime/function/render/rhi/vulkan/vulkan_shader.h"
#include "runtime/function/render/rhi/vulkan/vulkan_swap_chain.h"
#include "runtime/function/render/rhi/vulkan/vulkan_utils.h"

#include "runtime/function/window/window_system.h"

#include "runtime/core/base/macro.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#if defined(__GNUC__)
// https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
#if defined(__linux__)
#include <stdlib.h>
#elif defined(__MACH__)
// https://developer.apple.com/library/archive/documentation/Porting/Conceptual/PortingUnix/compiling/compiling.html
#include <stdlib.h>
#else
#error Unknown Platform
#endif
#endif

#include <memory>
#include <set>

static VkResult CreateDebugUtilsMessengerEXT(VkInstance                                instance,
                                             const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                             const VkAllocationCallbacks*              pAllocator,
                                             VkDebugUtilsMessengerEXT*                 pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

namespace ArchViz
{
    void VulkanRHI::setConfigManager(std::shared_ptr<ConfigManager> config_manager)
    {
        ASSERT(config_manager);
        m_config_manager = config_manager;
    }

    void VulkanRHI::setAssetManager(std::shared_ptr<AssetManager> asset_manager)
    {
        ASSERT(asset_manager);
        m_asset_manager = asset_manager;
    }

    void VulkanRHI::createInstance()
    {
        m_vulkan_instance = std::make_shared<VulkanInstance>(m_enable_validation_layers);
        m_vulkan_instance->connect(m_initialize_info.window_system->getWindow());
        m_vulkan_instance->initialize();
    }

    void VulkanRHI::setupDebugMessenger() {}

    void VulkanRHI::createSurface() {}

    void VulkanRHI::pickPhysicalDevice()
    {
        m_vulkan_device = std::make_shared<VulkanDevice>(m_enable_validation_layers);
        m_vulkan_device->connect(m_vulkan_instance->m_instance, m_vulkan_instance->m_surface);
        m_vulkan_device->initialize();
    }

    void VulkanRHI::createLogicalDevice() {}

    void VulkanRHI::createSwapChain()
    {
        m_vulkan_swap_chain = std::make_shared<VulkanSwapChain>();

        uint32_t width  = m_initialize_info.window_system->getWindowWidth();
        uint32_t height = m_initialize_info.window_system->getWindowHeight();

        m_vulkan_swap_chain->connect(
            m_vulkan_instance->m_instance, m_vulkan_instance->m_surface, m_vulkan_device->m_physical_device, m_vulkan_device->m_device);
        m_vulkan_swap_chain->initialize(width, height, false, false);
    }

    void VulkanRHI::createImageViews() {}

    void VulkanRHI::createRenderPass()
    {
        m_vulkan_render_pass = std::make_shared<VulkanRenderPass>();
        m_vulkan_render_pass->setDevice(m_vulkan_device);
        m_vulkan_render_pass->m_color_format = m_vulkan_swap_chain->m_swap_chain_image_format;
        m_vulkan_render_pass->initialize();
    }

    void VulkanRHI::createGraphicsPipeline()
    {
        ShaderModuleConfig config;
        config.m_vert_shader = "shader/glsl/shader_base.vert";
        config.m_frag_shader = "shader/glsl/shader_base.frag";

        std::shared_ptr<VulkanShader> shader = std::make_shared<VulkanShader>(config);

        shader->m_device         = m_vulkan_device;
        shader->m_config_manager = m_config_manager;
        shader->m_asset_manager  = m_asset_manager;

        m_vulkan_pipeline = std::make_shared<VulkanPipeline>();
        m_vulkan_pipeline->setDevice(m_vulkan_device);
        m_vulkan_pipeline->setShaderModule(shader);
        m_vulkan_pipeline->setRenderPass(m_vulkan_render_pass->m_render_pass);
        m_vulkan_pipeline->initialize();
    }

    void VulkanRHI::createFramebuffers()
    {
        m_swap_chain_framebuffers.resize(m_vulkan_swap_chain->m_buffers.size());

        for (size_t i = 0; i < m_vulkan_swap_chain->m_buffers.size(); i++)
        {
            VkImageView attachments[] = {m_vulkan_swap_chain->m_buffers[i].view};

            VkFramebufferCreateInfo framebuffer_info {};
            framebuffer_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass      = m_vulkan_render_pass->m_render_pass;
            framebuffer_info.attachmentCount = 1;
            framebuffer_info.pAttachments    = attachments;
            framebuffer_info.width           = m_vulkan_swap_chain->m_swap_chain_extent.width;
            framebuffer_info.height          = m_vulkan_swap_chain->m_swap_chain_extent.height;
            framebuffer_info.layers          = 1;

            if (vkCreateFramebuffer(m_vulkan_device->m_device, &framebuffer_info, nullptr, &m_swap_chain_framebuffers[i]) != VK_SUCCESS)
            {
                LOG_FATAL("failed to create framebuffer!");
            }
        }
    }

    void VulkanRHI::createCommandPool()
    {
        VkCommandPoolCreateInfo pool_info {};
        pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = m_vulkan_device->m_indices.m_graphics_family.value();

        if (vkCreateCommandPool(m_vulkan_device->m_device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create command pool!");
        }
    }

    void VulkanRHI::createCommandBuffer()
    {
        VkCommandBufferAllocateInfo alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool        = m_command_pool;
        alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(m_vulkan_device->m_device, &alloc_info, &m_command_buffer) != VK_SUCCESS)
        {
            LOG_FATAL("failed to allocate command buffers!");
        }
    }

    void VulkanRHI::initialize(RHIInitInfo initialize_info)
    {
        m_initialize_info = initialize_info;

        createInstance();
        setupDebugMessenger();
        createSurface();

        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();

        createRenderPass();
        createGraphicsPipeline();

        createFramebuffers();

        createCommandPool();
        createCommandBuffer();
    }

    void VulkanRHI::prepareContext() {}

    void VulkanRHI::clear()
    {
        for (auto framebuffer : m_swap_chain_framebuffers)
        {
            vkDestroyFramebuffer(m_vulkan_device->m_device, framebuffer, nullptr);
        }

        m_vulkan_pipeline->clear();
        m_vulkan_pipeline.reset();

        m_vulkan_render_pass->clear();
        m_vulkan_render_pass.reset();

        m_vulkan_swap_chain->clear();
        m_vulkan_swap_chain.reset();

        m_vulkan_device->clear();
        m_vulkan_device.reset();

        m_vulkan_instance->clear();
        m_vulkan_instance.reset();

        // if (m_enable_validation_layers)
        // {
        //     DestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        // }

        // vkDestroyInstance(m_instance, nullptr);
    }
} // namespace ArchViz
