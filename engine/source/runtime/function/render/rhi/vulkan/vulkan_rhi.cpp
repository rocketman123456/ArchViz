#include "runtime/function/render/rhi/vulkan/vulkan_rhi.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"
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

VkResult CreateDebugUtilsMessengerEXT(VkInstance                                instance,
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

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
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

        if (m_enable_validation_layers && !checkValidationLayerSupport(VulkanConstants::validation_layers))
        {
            LOG_FATAL("validation layers requested, but not available!");
        }

        VkApplicationInfo app_info {};
        app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName   = "ArchViz";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName        = "ArchViz Engine";
        app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion         = VK_API_VERSION_1_0;

        VkInstanceCreateInfo create_info {};
        create_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;

        auto extensions                     = getRequiredExtensions(m_enable_validation_layers);
        create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
        if (m_enable_validation_layers)
        {
            create_info.enabledLayerCount   = static_cast<uint32_t>(VulkanConstants::validation_layers.size());
            create_info.ppEnabledLayerNames = VulkanConstants::validation_layers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
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

    void VulkanRHI::setupDebugMessenger()
    {
        if (!m_enable_validation_layers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT create_info;
        populateDebugMessengerCreateInfo(create_info);

        if (CreateDebugUtilsMessengerEXT(m_instance, &create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
        {
            LOG_FATAL("failed to set up debug messenger!");
        }
    }

    void VulkanRHI::createSurface()
    {
        m_vulkan_swap_chain = std::make_shared<VulkanSwapChain>();
        m_vulkan_swap_chain->connect(m_instance, VK_NULL_HANDLE, VK_NULL_HANDLE);
        m_vulkan_swap_chain->initSurface(m_initialize_info.window_system->getWindow());

        m_surface = m_vulkan_swap_chain->m_surface;
    }

    void VulkanRHI::pickPhysicalDevice()
    {
        m_vulkan_device = std::make_shared<VulkanDevice>(m_enable_validation_layers);
        m_vulkan_device->connect(m_instance, m_surface);
        m_vulkan_device->initialize();

        m_physical_device = m_vulkan_device->m_physical_device;
        m_device          = m_vulkan_device->m_device;
    }

    void VulkanRHI::createLogicalDevice() { m_device = m_vulkan_device->m_device; }

    void VulkanRHI::createSwapChain()
    {
        uint32_t width  = m_initialize_info.window_system->getWindowSize()[0];
        uint32_t height = m_initialize_info.window_system->getWindowSize()[1];

        m_vulkan_swap_chain->connect(m_instance, m_physical_device, m_device);
        m_vulkan_swap_chain->create(width, height, false, false);
    }

    void VulkanRHI::createImageViews() {}

    void VulkanRHI::createRenderPass()
    {
        VkAttachmentDescription color_attachment {};
        color_attachment.format         = m_vulkan_swap_chain->m_swap_chain_image_format;
        color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &color_attachment_ref;

        VkRenderPassCreateInfo render_pass_info {};
        render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments    = &color_attachment;
        render_pass_info.subpassCount    = 1;
        render_pass_info.pSubpasses      = &subpass;

        if (vkCreateRenderPass(m_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create render pass!");
        }
    }

    void VulkanRHI::createGraphicsPipeline()
    {
        ShaderModuleConfig config;
        config.m_vert_shader = "shader/glsl/shader_base.vert";
        config.m_frag_shader = "shader/glsl/shader_base.frag";

        VulkanShader shader(config);
        shader.m_device         = m_vulkan_device;
        shader.m_config_manager = m_config_manager;
        shader.m_asset_manager  = m_asset_manager;
        shader.initialize();

        // TODO : make auto
        VkPipelineShaderStageCreateInfo vert_shader_stage_info {};
        vert_shader_stage_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_info.module = shader.m_vert_shader;
        vert_shader_stage_info.pName  = "main";

        VkPipelineShaderStageCreateInfo frag_shader_stage_info {};
        frag_shader_stage_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_info.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_info.module = shader.m_frag_shader;
        frag_shader_stage_info.pName  = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vert_shader_stage_info, frag_shader_stage_info};

        VkPipelineVertexInputStateCreateInfo vertex_input_info {};
        vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount   = 0;
        vertex_input_info.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo input_assembly {};
        input_assembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state {};
        viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount  = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer {};
        rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable        = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth               = 1.0f;
        rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable         = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling {};
        multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable  = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState color_blend_attachment {};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable    = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo color_blending {};
        color_blending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable     = VK_FALSE;
        color_blending.logicOp           = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount   = 1;
        color_blending.pAttachments      = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f;
        color_blending.blendConstants[1] = 0.0f;
        color_blending.blendConstants[2] = 0.0f;
        color_blending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState>      dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamic_state {};
        dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates    = dynamic_states.data();

        VkPipelineLayoutCreateInfo pipeline_layout_info {};
        pipeline_layout_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount         = 0;
        pipeline_layout_info.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create pipeline layout!")
        }

        VkGraphicsPipelineCreateInfo pipeline_info {};
        pipeline_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount          = 2;
        pipeline_info.pStages             = shaderStages;
        pipeline_info.pVertexInputState   = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState      = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState   = &multisampling;
        pipeline_info.pColorBlendState    = &color_blending;
        pipeline_info.pDynamicState       = &dynamic_state;
        pipeline_info.layout              = m_pipeline_layout;
        pipeline_info.renderPass          = m_render_pass;
        pipeline_info.subpass             = 0;
        pipeline_info.basePipelineHandle  = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_pipeline) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create graphics pipeline!");
        }

        shader.clear();
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
    }

    void VulkanRHI::prepareContext() {}

    void VulkanRHI::clear()
    {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
        vkDestroyRenderPass(m_device, m_render_pass, nullptr);

        m_vulkan_swap_chain->cleanup();
        m_vulkan_swap_chain.reset();

        m_vulkan_device->cleanup();
        m_vulkan_device.reset();

        if (m_enable_validation_layers)
        {
            DestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        }

        vkDestroyInstance(m_instance, nullptr);
    }
} // namespace ArchViz
