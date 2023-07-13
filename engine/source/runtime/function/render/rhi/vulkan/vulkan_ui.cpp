#include "runtime/function/render/rhi/vulkan/vulkan_ui.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_buffer_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_shader_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_texture_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_utils.h"
#include "runtime/function/render/rhi/vulkan/vulkan_buffer.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/vulkan_instance.h"
#include "runtime/function/render/rhi/vulkan/vulkan_shader.h"
#include "runtime/function/render/rhi/vulkan/vulkan_texture.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/core/base/macro.h"
#include <GLFW/glfw3.h>

#define GLFW_INCLUDE_NONE
// #include "backends/imgui_impl_glfw.h"
// #include "backends/imgui_impl_vulkan.h"
#include "imgui.h"

#include <stdio.h>
#include <stdlib.h>

namespace ArchViz
{
    void VulkanUI::setFPS(uint32_t fps) { m_fps = fps; }

    void VulkanUI::initialize()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
        // io.ConfigViewportsNoAutoMerge = true;
        // io.ConfigViewportsNoTaskBarIcon = true;

        // TODO : add multi viewport support

        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        float scale_x, scale_y;
        glfwGetWindowContentScale(m_window, &scale_x, &scale_y);
        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(scale_x);
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will
        // call.
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        auto font_path = m_config_manager->getRootFolder() / "asset-test/data/font/MiSans-Normal.ttf";
        // io.Fonts->AddFontDefault();
        ImFont* font = io.Fonts->AddFontFromFileTTF(font_path.generic_string().c_str(), 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        ASSERT(font);

        unsigned char* font_data;
        int            tex_width, tex_height;

        io.Fonts->GetTexDataAsRGBA32(&font_data, &tex_width, &tex_height);
        VkDeviceSize imageSize = tex_width * tex_height * 4;

        // Upload Fonts
        m_font_texture                 = std::make_shared<VulkanTexture>();
        m_font_texture->m_device       = m_device;
        m_font_texture->m_command_pool = m_command_pool;
        m_font_texture->m_address_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        m_font_texture->initialize(font_data, imageSize, VK_FORMAT_R8G8B8A8_UNORM, tex_width, tex_height);

        m_vertex_buffer = std::make_shared<VulkanBuffer>();
        m_index_buffer  = std::make_shared<VulkanBuffer>();

        m_vertex_buffer->device = m_device->m_device;
        m_index_buffer->device  = m_device->m_device;

        // createRenderPass();
        createDescriptorSetLayout();
        createDescriptorSets();
        createPipelineLayout();
        createPipeline();
    }

    void VulkanUI::createRenderPass()
    {
        VkAttachmentDescription attachment {};
        attachment.format         = m_image_format;
        attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment {};
        color_attachment.attachment = 0;
        color_attachment.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &color_attachment;

        VkSubpassDependency dependency {};
        dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass    = 0;
        dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo info {};
        info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.attachmentCount = 1;
        info.pAttachments    = &attachment;
        info.subpassCount    = 1;
        info.pSubpasses      = &subpass;
        info.dependencyCount = 1;
        info.pDependencies   = &dependency;

        if (vkCreateRenderPass(m_device->m_device, &info, nullptr, &m_ui_pass) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create ui pass!");
        }
    }

    void VulkanUI::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding sampler_layout_binding {};
        sampler_layout_binding.binding            = 0;
        sampler_layout_binding.descriptorCount    = 1;
        sampler_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_layout_binding.pImmutableSamplers = nullptr;
        sampler_layout_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 1> bindings = {sampler_layout_binding};

        VkDescriptorSetLayoutCreateInfo layout_info {};
        layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
        layout_info.pBindings    = bindings.data();

        if (vkCreateDescriptorSetLayout(m_device->m_device, &layout_info, nullptr, &m_descriptor_set_layout) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create descriptor set layout!");
        }
    }

    void VulkanUI::createDescriptorSets()
    {
        VkDescriptorSetAllocateInfo alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool     = m_descriptor_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts        = &m_descriptor_set_layout;

        if (vkAllocateDescriptorSets(m_device->m_device, &alloc_info, &m_descriptor_set) != VK_SUCCESS)
        {
            LOG_FATAL("failed to allocate descriptor sets!");
        }

        VkDescriptorImageInfo image_info {};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView   = m_font_texture->m_view;
        image_info.sampler     = m_font_texture->m_sampler;

        VkWriteDescriptorSet descriptor_write {};

        descriptor_write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_write.dstSet          = m_descriptor_set;
        descriptor_write.dstBinding      = 0;
        descriptor_write.descriptorCount = 1;
        descriptor_write.pImageInfo      = &image_info;

        vkUpdateDescriptorSets(m_device->m_device, 1, &descriptor_write, 0, nullptr);
    }

    void VulkanUI::createPipelineLayout()
    {
        ShaderModuleConfig config;
        config.m_vert_shader = "shader/glsl/imgui.vert";
        config.m_frag_shader = "shader/glsl/imgui.frag";

        m_shader = std::make_shared<VulkanShader>(config);

        m_shader->m_device         = m_device;
        m_shader->m_config_manager = m_config_manager;
        m_shader->m_asset_manager  = m_asset_manager;

        m_shader->initialize();

        VkPushConstantRange push_constant_range {};
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        push_constant_range.size       = sizeof(PushConstantBlock);
        push_constant_range.offset     = 0;

        VkPipelineLayoutCreateInfo pipeline_layout_create_info {};
        pipeline_layout_create_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_create_info.setLayoutCount         = 1;
        pipeline_layout_create_info.pSetLayouts            = &m_descriptor_set_layout;
        pipeline_layout_create_info.pushConstantRangeCount = 1;
        pipeline_layout_create_info.pPushConstantRanges    = &push_constant_range;

        if (vkCreatePipelineLayout(m_device->m_device, &pipeline_layout_create_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create ui pipeline layout");
        }
    }

    void VulkanUI::createPipeline()
    {
        VkPipelineInputAssemblyStateCreateInfo input_assembly_state {};
        input_assembly_state.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_state.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_state.flags                  = 0;
        input_assembly_state.primitiveRestartEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo rasterization {};
        rasterization.sType            = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.polygonMode      = VK_POLYGON_MODE_FILL;
        rasterization.cullMode         = VK_CULL_MODE_NONE;
        rasterization.frontFace        = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization.flags            = 0;
        rasterization.depthClampEnable = VK_FALSE;
        rasterization.lineWidth        = 1.0f;

        VkPipelineColorBlendAttachmentState blend_attachment {};
        blend_attachment.blendEnable         = VK_TRUE;
        blend_attachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;
        blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo color_blend_state {};
        color_blend_state.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_state.attachmentCount = 1;
        color_blend_state.pAttachments    = &blend_attachment;

        VkPipelineDepthStencilStateCreateInfo depth_stencil_state {};
        depth_stencil_state.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil_state.depthTestEnable  = VK_FALSE;
        depth_stencil_state.depthWriteEnable = VK_FALSE;
        depth_stencil_state.depthCompareOp   = VK_COMPARE_OP_LESS_OR_EQUAL;
        depth_stencil_state.back.compareOp   = VK_COMPARE_OP_ALWAYS;

        VkPipelineViewportStateCreateInfo viewport_state {};
        viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount  = 1;
        viewport_state.flags         = 0;

        VkPipelineMultisampleStateCreateInfo multisample_state {};
        multisample_state.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisample_state.flags                = 0;

        std::vector<VkDynamicState> dynamic_state_enables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamic_state {};
        dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.pDynamicStates    = dynamic_state_enables.data();
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_state_enables.size());
        dynamic_state.flags             = 0;

        // std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages {};

        VkVertexInputBindingDescription binding_description {};
        binding_description.binding   = 0;
        binding_description.stride    = sizeof(ImDrawVert);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::vector<VkVertexInputBindingDescription> vertex_input_bindings = {binding_description};

        std::array<VkVertexInputAttributeDescription, 3> vertex_input_attributes {};

        // Location 0: Position
        vertex_input_attributes[0].binding  = 0;
        vertex_input_attributes[0].location = 0;
        vertex_input_attributes[0].format   = VK_FORMAT_R32G32_SFLOAT;
        vertex_input_attributes[0].offset   = offsetof(ImDrawVert, pos);

        // Location 1: UV
        vertex_input_attributes[1].binding  = 0;
        vertex_input_attributes[1].location = 1;
        vertex_input_attributes[1].format   = VK_FORMAT_R32G32_SFLOAT;
        vertex_input_attributes[1].offset   = offsetof(ImDrawVert, uv);

        // Location 0: Color
        vertex_input_attributes[2].binding  = 0;
        vertex_input_attributes[2].location = 2;
        vertex_input_attributes[2].format   = VK_FORMAT_R8G8B8A8_UNORM;
        vertex_input_attributes[2].offset   = offsetof(ImDrawVert, col);

        VkPipelineVertexInputStateCreateInfo vertex_input_state {};
        vertex_input_state.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_state.vertexBindingDescriptionCount   = static_cast<uint32_t>(vertex_input_bindings.size());
        vertex_input_state.pVertexBindingDescriptions      = vertex_input_bindings.data();
        vertex_input_state.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_input_attributes.size());
        vertex_input_state.pVertexAttributeDescriptions    = vertex_input_attributes.data();

        VkGraphicsPipelineCreateInfo pipeline_create_info {};
        pipeline_create_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_create_info.layout              = m_pipeline_layout;
        pipeline_create_info.renderPass          = m_ui_pass;
        pipeline_create_info.flags               = 0;
        pipeline_create_info.basePipelineIndex   = -1;
        pipeline_create_info.basePipelineHandle  = VK_NULL_HANDLE;
        pipeline_create_info.pInputAssemblyState = &input_assembly_state;
        pipeline_create_info.pRasterizationState = &rasterization;
        pipeline_create_info.pColorBlendState    = &color_blend_state;
        pipeline_create_info.pMultisampleState   = &multisample_state;
        pipeline_create_info.pViewportState      = &viewport_state;
        pipeline_create_info.pDepthStencilState  = &depth_stencil_state;
        pipeline_create_info.pDynamicState       = &dynamic_state;
        pipeline_create_info.stageCount          = static_cast<uint32_t>(m_shader->m_stage_info.size());
        pipeline_create_info.pStages             = m_shader->m_stage_info.data();
        pipeline_create_info.pVertexInputState   = &vertex_input_state;

        if (vkCreateGraphicsPipelines(m_device->m_device, m_pipeline_cache, 1, &pipeline_create_info, nullptr, &m_pipeline) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create ui graphics pipeline");
        }
    }

    void VulkanUI::prepareContext(float width, float height)
    {
        ImGuiIO& io = ImGui::GetIO();

#ifdef __MACH__

        int   w, h;
        int   fw, fh;
        float scale_x, scale_y;
        glfwGetWindowSize(m_window, &w, &h);
        glfwGetFramebufferSize(m_window, &fw, &fh);
        scale_x = (float)fw / (float)w;
        scale_y = (float)fh / (float)h;

        io.DisplaySize             = ImVec2(width, height);
        io.DisplayFramebufferScale = ImVec2(scale_x, scale_y);

        int left  = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
        int right = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT);

        double x, y;
        glfwGetCursorPos(m_window, &x, &y);

        LOG_DEBUG("size: {}, {}, scale: {}, {}, pos: {}, {}", w, h, scale_x, scale_y, x, y);

        io.MousePos     = ImVec2(x * scale_x, y * scale_y);
        io.MouseDown[0] = left;
        io.MouseDown[1] = right;
#else
        float scale_x, scale_y;
        glfwGetWindowContentScale(m_window, &scale_x, &scale_y);

        io.DisplaySize             = ImVec2(width, height);
        io.DisplayFramebufferScale = ImVec2(scale_x, scale_y);

        int left  = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
        int right = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT);

        double x, y;
        glfwGetCursorPos(m_window, &x, &y);

        io.MousePos     = ImVec2(x, y);
        io.MouseDown[0] = left;
        io.MouseDown[1] = right;
#endif

        m_push_const.scale     = {2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y};
        m_push_const.translate = {-1.0f, -1.0f};

        ImGui::NewFrame();
    }

    void VulkanUI::renderUI()
    {
        static bool opt_fullscreen = false;
        static bool opt_padding    = false;

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoBackground;

        ImGuiIO& io = ImGui::GetIO();

        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        {
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Options"))
                {
                    // Disabling fullscreen would allow the window to be moved to the front of other windows,
                    // which we can't undo at the moment without finer window depth/z control.
                    // ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                    ImGui::MenuItem("Padding", NULL, &opt_padding);
                    ImGui::Separator();

                    if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))
                    {
                        dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
                    }
                    if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
                    {
                        dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
                    }
                    if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
                    {
                        dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
                    }
                    if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
                    {
                        dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
                    }
                    if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen))
                    {
                        dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
                    }
                    ImGui::Separator();

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }
        }
        ImGui::End();

        // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        ImGui::Begin("ArchViz");
        {
            ImGui::Text("FPS %d", m_fps);
        }
        ImGui::End();

        ImGui::Begin(u8"ArchViz ≤‚ ‘");
        {
            ImGui::Text("Hello, world %d", 123);
        }
        ImGui::End();
    }

    void VulkanUI::showImage(std::shared_ptr<VulkanTexture> image, const std::string& name)
    {
        ImGui::Begin(name.c_str());
        {
            ImGui::Text("pointer = %p", image->m_descriptor_set);
            ImGui::Text("size = %d x %d", image->m_width, image->m_height);
            ImGui::Image((ImTextureID)image->m_descriptor_set, ImVec2(image->m_width, image->m_height));
        }
        ImGui::End();
    }

    void VulkanUI::renderFinish()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Render();

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void VulkanUI::recordCommandBuffer(VkCommandBuffer command_buffer, VkFramebuffer frame_buffer)
    {
        ImGuiIO& io = ImGui::GetIO();

        ImDrawData* main_draw_data    = ImGui::GetDrawData();
        const bool  main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);

        // Note: Alignment is done inside buffer creation
        VkDeviceSize vertex_buffer_size = main_draw_data->TotalVtxCount * sizeof(ImDrawVert);
        VkDeviceSize index_buffer_size  = main_draw_data->TotalIdxCount * sizeof(ImDrawIdx);

        if ((vertex_buffer_size == 0) || (index_buffer_size == 0))
        {
            return;
        }

        // Resize Vertex buffer
        if ((m_vertex_buffer->buffer == VK_NULL_HANDLE) || (m_vertex_count < main_draw_data->TotalVtxCount))
        {
            // TODO : use fence or semphore to wait
            m_device->wait();

            m_vertex_buffer->unmap();
            m_vertex_buffer->destroy();

            m_vertex_buffer->size     = vertex_buffer_size;
            m_vertex_buffer->usage    = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            m_vertex_buffer->property = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            VulkanBufferUtils::createBuffer(m_device, m_vertex_buffer->size, m_vertex_buffer->usage, m_vertex_buffer->property, m_vertex_buffer->buffer, m_vertex_buffer->memory);

            // TODO : add alignment support
            m_vertex_buffer->setupDescriptor();

            m_vertex_count = main_draw_data->TotalVtxCount;
            // m_vertex_buffer->map(vertex_buffer_size, 0);
            m_vertex_buffer->map();
        }

        // Resize Index buffer
        if ((m_index_buffer->buffer == VK_NULL_HANDLE) || (m_index_count < main_draw_data->TotalIdxCount))
        {
            // TODO : use fence or semphore to wait
            m_device->wait();

            m_index_buffer->unmap();
            m_index_buffer->destroy();

            m_index_buffer->size     = index_buffer_size;
            m_index_buffer->usage    = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            m_index_buffer->property = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            VulkanBufferUtils::createBuffer(m_device, m_index_buffer->size, m_index_buffer->usage, m_index_buffer->property, m_index_buffer->buffer, m_index_buffer->memory);

            // TODO : add alignment support
            m_index_buffer->setupDescriptor();

            m_index_count = main_draw_data->TotalIdxCount;
            // m_index_buffer->map(index_buffer_size, 0);
            m_index_buffer->map();
        }

        // Upload data
        ImDrawVert* vtx_dst = (ImDrawVert*)m_vertex_buffer->mapped;
        ImDrawIdx*  idx_dst = (ImDrawIdx*)m_index_buffer->mapped;

        for (int n = 0; n < main_draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list      = main_draw_data->CmdLists[n];
            size_t            copy_vtx_size = cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
            size_t            copy_idx_size = cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
            memcpy(vtx_dst, cmd_list->VtxBuffer.Data, copy_vtx_size);
            memcpy(idx_dst, cmd_list->IdxBuffer.Data, copy_idx_size);
            vtx_dst += cmd_list->VtxBuffer.Size;
            idx_dst += cmd_list->IdxBuffer.Size;
        }

        // Flush to make writes visible to GPU
        m_vertex_buffer->flush();
        m_index_buffer->flush();

        // VkExtent2D extend {};
        // extend.width  = io.DisplaySize.x;
        // extend.height = io.DisplaySize.y;

        // VkRenderPassBeginInfo render_pass_info {};
        // render_pass_info.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        // render_pass_info.renderPass        = m_ui_pass;
        // render_pass_info.framebuffer       = frame_buffer;
        // render_pass_info.renderArea.offset = {0, 0};
        // render_pass_info.renderArea.extent = extend;

        // std::array<VkClearValue, 2> clear_color {};
        // clear_color[0].color        = {{0.2f, 0.3f, 0.4f, 1.0f}};
        // clear_color[1].depthStencil = {1.0f, 0};

        // render_pass_info.clearValueCount = static_cast<uint32_t>(clear_color.size());
        // render_pass_info.pClearValues    = clear_color.data();

        // vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        {
            vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

            VkViewport viewport {};
            viewport.width    = ImGui::GetIO().DisplaySize.x;
            viewport.height   = ImGui::GetIO().DisplaySize.y;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(command_buffer, 0, 1, &viewport);

            // UI scale and translate via push constants
            m_push_const.scale     = {2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y};
            m_push_const.translate = {-1.0f, -1.0f};
            vkCmdPushConstants(command_buffer, m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantBlock), &m_push_const);

            // Render commands
            ImDrawData* main_draw_data = ImGui::GetDrawData();
            int32_t     vertex_offset  = 0;
            int32_t     index_offset   = 0;

            if (main_draw_data->CmdListsCount > 0)
            {
                VkDeviceSize offsets[1] = {0};
                vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_vertex_buffer->buffer, offsets);
                vkCmdBindIndexBuffer(command_buffer, m_index_buffer->buffer, 0, VK_INDEX_TYPE_UINT16);

                for (int32_t i = 0; i < main_draw_data->CmdListsCount; i++)
                {
                    const ImDrawList* cmd_list = main_draw_data->CmdLists[i];
                    for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
                    {
                        const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                        VkRect2D         scissorRect;

                        VkDescriptorSet desc_set = (VkDescriptorSet)pcmd->TextureId;
                        if (sizeof(ImTextureID) < sizeof(ImU64))
                        {
                            // We don't support texture switches if ImTextureID hasn't been redefined to be 64-bit.
                            // Do a flaky check that other textures haven't been used.
                            desc_set = m_descriptor_set;
                        }
                        if (desc_set == VK_NULL_HANDLE)
                        {
                            desc_set = m_descriptor_set;
                        }
                        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, &desc_set, 0, nullptr);

                        scissorRect.offset.x      = std::max((int32_t)(pcmd->ClipRect.x), 0);
                        scissorRect.offset.y      = std::max((int32_t)(pcmd->ClipRect.y), 0);
                        scissorRect.extent.width  = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
                        scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
                        vkCmdSetScissor(command_buffer, 0, 1, &scissorRect);

                        vkCmdDrawIndexed(command_buffer, pcmd->ElemCount, 1, index_offset, vertex_offset, 0);
                        index_offset += pcmd->ElemCount;
                    }
                    vertex_offset += cmd_list->VtxBuffer.Size;
                }
            }
        }

        // vkCmdEndRenderPass(command_buffer);
    }

    void VulkanUI::clear()
    {
        ImGui::DestroyContext();

        vkDestroyPipeline(m_device->m_device, m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device->m_device, m_pipeline_layout, nullptr);

        vkDestroyDescriptorSetLayout(m_device->m_device, m_descriptor_set_layout, nullptr);

        // vkDestroyRenderPass(m_device->m_device, m_ui_pass, nullptr);

        m_index_buffer->destroy();
        m_index_buffer.reset();

        m_vertex_buffer->destroy();
        m_vertex_buffer.reset();

        m_shader->clear();
        m_shader.reset();

        m_font_texture->clear();
        m_font_texture.reset();
    }
} // namespace ArchViz
