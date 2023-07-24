#include "runtime/function/render/rhi/vulkan/vulkan_rhi.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_buffer.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_instance.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_pipeline.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_render_pass.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_shader.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_swap_chain.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_texture.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_buffer_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_debug_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_texture_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_utils.h"
#include "runtime/function/render/rhi/vulkan/vulkan_ui.h"

#include "runtime/function/render/geometry/particle.h"
#include "runtime/function/render/geometry/vertex.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/window/window_system.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/math/math.h"

// TODO : move this to asset loader part
#include <tiny_obj_loader.h>

#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>

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

    void VulkanRHI::setFPS(uint32_t fps)
    {
        m_fps = fps;
        m_vulkan_ui->setFPS(m_fps);
    }

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    void VulkanRHI::createInstance()
    {
        m_vulkan_instance = std::make_shared<VulkanInstance>();

        m_vulkan_instance->m_validation = m_enable_validation_layers;
        m_vulkan_instance->m_window     = m_initialize_info.window_system->getWindow();
        m_vulkan_instance->initialize();

        VulkanDebugUtils::setup(m_vulkan_instance->m_instance);
    }

    void VulkanRHI::createVulkanDevice()
    {
        m_vulkan_device = std::make_shared<VulkanDevice>();

        m_vulkan_device->m_validation = m_enable_validation_layers;
        m_vulkan_device->m_instance   = m_vulkan_instance;
        m_vulkan_device->initialize();
    }

    void VulkanRHI::createSwapChain()
    {
        m_vulkan_swap_chain = std::make_shared<VulkanSwapChain>();

        m_vulkan_swap_chain->m_instance = m_vulkan_instance;
        m_vulkan_swap_chain->m_device   = m_vulkan_device;

        uint32_t width  = m_initialize_info.window_system->getWindowWidth();
        uint32_t height = m_initialize_info.window_system->getWindowHeight();
        m_vulkan_swap_chain->initialize(width, height, false, false);
    }

    void VulkanRHI::recreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_initialize_info.window_system->getWindow(), &width, &height);
        // for minimize
        while (width == 0 || height == 0)
        {
            // TODO : add variable to judge this
            glfwGetFramebufferSize(m_initialize_info.window_system->getWindow(), &width, &height);
            glfwWaitEvents();
        }

        m_vulkan_device->wait();

        vkDestroyImageView(m_vulkan_device->m_device, m_depth_image_view, nullptr);
        vkDestroyImage(m_vulkan_device->m_device, m_depth_image, nullptr);
        vkFreeMemory(m_vulkan_device->m_device, m_depth_image_memory, nullptr);

        for (auto framebuffer : m_swap_chain_framebuffers)
        {
            vkDestroyFramebuffer(m_vulkan_device->m_device, framebuffer, nullptr);
        }

        m_vulkan_swap_chain->clear();

        m_vulkan_swap_chain->initialize(width, height);
        createDepthResources();
        createFramebuffers();
    }

    void VulkanRHI::createDescriptorPool()
    {
        // Since DescriptorSet should be treated as asset in Vulkan, DescriptorPool
        // should be big enough, and thus we can sub-allocate DescriptorSet from
        // DescriptorPool merely as we sub-allocate Buffer/Image from DeviceMemory.
        std::array<VkDescriptorPoolSize, 11> pool_sizes {};
        pool_sizes[0].type             = VK_DESCRIPTOR_TYPE_SAMPLER;
        pool_sizes[0].descriptorCount  = VulkanConstants::k_global_pool_elements;
        pool_sizes[1].type             = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[1].descriptorCount  = VulkanConstants::k_global_pool_elements;
        pool_sizes[2].type             = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        pool_sizes[2].descriptorCount  = VulkanConstants::k_global_pool_elements;
        pool_sizes[3].type             = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        pool_sizes[3].descriptorCount  = VulkanConstants::k_global_pool_elements;
        pool_sizes[4].type             = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        pool_sizes[4].descriptorCount  = VulkanConstants::k_global_pool_elements;
        pool_sizes[5].type             = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        pool_sizes[5].descriptorCount  = VulkanConstants::k_global_pool_elements;
        pool_sizes[6].type             = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[6].descriptorCount  = VulkanConstants::k_global_pool_elements;
        pool_sizes[7].type             = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        pool_sizes[7].descriptorCount  = VulkanConstants::k_global_pool_elements;
        pool_sizes[8].type             = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        pool_sizes[8].descriptorCount  = VulkanConstants::k_global_pool_elements;
        pool_sizes[9].type             = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        pool_sizes[9].descriptorCount  = VulkanConstants::k_global_pool_elements;
        pool_sizes[10].type            = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        pool_sizes[10].descriptorCount = VulkanConstants::k_global_pool_elements;

        VkDescriptorPoolCreateInfo pool_info {};
        pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes    = pool_sizes.data();
        pool_info.maxSets       = static_cast<uint32_t>(pool_sizes.size() * VulkanConstants::k_global_pool_elements);

        if (vkCreateDescriptorPool(m_vulkan_device->m_device, &pool_info, nullptr, &m_descriptor_pool) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create global discriptor pool");
        }
    }

    void VulkanRHI::createBindlessDescriptorPool()
    {
        if (!m_bindless_supported)
            return;

        // Since DescriptorSet should be treated as asset in Vulkan, DescriptorPool
        // should be big enough, and thus we can sub-allocate DescriptorSet from
        // DescriptorPool merely as we sub-allocate Buffer/Image from DeviceMemory.
        std::array<VkDescriptorPoolSize, 2> pool_sizes_bindless {};
        pool_sizes_bindless[0].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes_bindless[0].descriptorCount = VulkanConstants::k_max_bindless_resources;
        pool_sizes_bindless[1].type            = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        pool_sizes_bindless[1].descriptorCount = VulkanConstants::k_max_bindless_resources;

        VkDescriptorPoolCreateInfo bindless_pool_info {};
        bindless_pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        bindless_pool_info.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
        bindless_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes_bindless.size());
        bindless_pool_info.pPoolSizes    = pool_sizes_bindless.data();
        bindless_pool_info.maxSets       = static_cast<uint32_t>(pool_sizes_bindless.size() * VulkanConstants::k_max_bindless_resources);

        m_bindless_pool_size = pool_sizes_bindless.size();

        if (vkCreateDescriptorPool(m_vulkan_device->m_device, &bindless_pool_info, nullptr, &m_bindless_pool) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create bindless discriptor pool");
        }

        m_bindless_pool_size = pool_sizes_bindless.size();
    }

    void VulkanRHI::createBindlessDescriptorSetLayout()
    {
        if (!m_bindless_supported)
            return;

        VkDescriptorSetLayoutBinding vk_binding[4];

        VkDescriptorSetLayoutBinding& image_sampler_binding {vk_binding[0]};
        image_sampler_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        image_sampler_binding.descriptorCount    = VulkanConstants::k_max_bindless_resources;
        image_sampler_binding.binding            = VulkanConstants::k_bindless_texture_binding;
        image_sampler_binding.stageFlags         = VK_SHADER_STAGE_ALL;
        image_sampler_binding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding& storage_image_binding {vk_binding[1]};
        storage_image_binding.descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        storage_image_binding.descriptorCount    = VulkanConstants::k_max_bindless_resources;
        storage_image_binding.binding            = VulkanConstants::k_bindless_texture_binding + 1;
        storage_image_binding.stageFlags         = VK_SHADER_STAGE_ALL;
        storage_image_binding.pImmutableSamplers = nullptr;

        VkDescriptorBindingFlags bindless_flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        VkDescriptorBindingFlags binding_flags[4];
        binding_flags[0] = bindless_flags;
        binding_flags[1] = bindless_flags;

        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extended_info {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT, nullptr};
        extended_info.bindingCount  = m_bindless_pool_size;
        extended_info.pBindingFlags = binding_flags;

        VkDescriptorSetLayoutCreateInfo layout_info {};
        layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = m_bindless_pool_size;
        layout_info.pBindings    = vk_binding;
        layout_info.flags        = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
        layout_info.pNext        = &extended_info;

        if (vkCreateDescriptorSetLayout(m_vulkan_device->m_device, &layout_info, nullptr, &m_bindless_set_layout) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create bindless descriptor set layout!");
        }
    }

    void VulkanRHI::createRenderPass()
    {
        m_vulkan_render_pass                 = std::make_shared<VulkanRenderPass>();
        m_vulkan_render_pass->m_device       = m_vulkan_device;
        m_vulkan_render_pass->m_color_format = m_vulkan_swap_chain->m_surface_format.format;
        m_vulkan_render_pass->initialize();
    }

    void VulkanRHI::createGraphicsPipelineCache()
    {
        VkPipelineCacheCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

        // read pipeline cache

        auto cache_path = m_config_manager->getRootFolder() / "pipeline.cache";

        // TODO : use vfs instead
        FILE* file = fopen(cache_path.generic_string().c_str(), "rb");

        if (file != nullptr)
        {
            size_t               cache_size = 0;
            std::vector<uint8_t> cache;

            fseek(file, 0, SEEK_END);
            cache_size = ftell(file);
            cache.resize(cache_size);
            rewind(file);
            fread(cache.data(), cache.size(), 1, file);

            VkPipelineCacheHeaderVersionOne* cache_header = (VkPipelineCacheHeaderVersionOne*)cache.data();
            if (cache_header->deviceID == m_vulkan_device->m_properties.deviceID && cache_header->vendorID == m_vulkan_device->m_properties.vendorID &&
                memcmp(cache_header->pipelineCacheUUID, m_vulkan_device->m_properties.pipelineCacheUUID, VK_UUID_SIZE))
            {
                create_info.initialDataSize = cache.size();
                create_info.pInitialData    = cache.data();
            }

            fclose(file);
        }

        if (vkCreatePipelineCache(m_vulkan_device->m_device, &create_info, nullptr, &m_pipeline_cache) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create pipeline cache");
        }
    }

    void VulkanRHI::createGraphicsPipeline()
    {
        ShaderModuleCreateInfo config;
        config.m_vert_shader = "shader/glsl/shader_phong.vert";
        config.m_frag_shader = "shader/glsl/shader_phong.frag";

        std::shared_ptr<VulkanShader> shader = std::make_shared<VulkanShader>(config);

        shader->m_device         = m_vulkan_device;
        shader->m_config_manager = m_config_manager;
        shader->m_asset_manager  = m_asset_manager;

        m_vulkan_pipeline = std::make_shared<VulkanPipeline>();

        m_vulkan_pipeline->m_device         = m_vulkan_device;
        m_vulkan_pipeline->m_shader         = shader;
        m_vulkan_pipeline->m_render_pass    = m_vulkan_render_pass->m_render_pass;
        m_vulkan_pipeline->m_pipeline_cache = m_pipeline_cache;

        m_vulkan_pipeline->initialize();
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

    void VulkanRHI::createDepthResources()
    {
        m_depth_format = VulkanUtils::findDepthFormat(m_vulkan_device->m_physical_device);

        VulkanTextureUtils::createImage(m_vulkan_device,
                                        m_vulkan_swap_chain->m_swap_chain_extent.width,
                                        m_vulkan_swap_chain->m_swap_chain_extent.height,
                                        1,
                                        m_depth_format,
                                        VK_IMAGE_TILING_OPTIMAL,
                                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        m_depth_image,
                                        m_depth_image_memory);
        m_depth_image_view = VulkanTextureUtils::createImageView(m_vulkan_device, m_depth_image, m_depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

        VulkanTextureUtils::transitionImageLayout(m_vulkan_device, m_command_pool, m_depth_image, m_depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
    }

    void VulkanRHI::createFramebuffers()
    {
        m_swap_chain_framebuffers.resize(m_vulkan_swap_chain->m_buffers.size());

        for (size_t i = 0; i < m_vulkan_swap_chain->m_buffers.size(); i++)
        {
            std::array<VkImageView, 2> attachments = {m_vulkan_swap_chain->m_buffers[i].view, m_depth_image_view};

            VkFramebufferCreateInfo framebuffer_info {};
            framebuffer_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass      = m_vulkan_render_pass->m_render_pass;
            framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebuffer_info.pAttachments    = attachments.data();
            framebuffer_info.width           = m_vulkan_swap_chain->m_swap_chain_extent.width;
            framebuffer_info.height          = m_vulkan_swap_chain->m_swap_chain_extent.height;
            framebuffer_info.layers          = 1;

            if (vkCreateFramebuffer(m_vulkan_device->m_device, &framebuffer_info, nullptr, &m_swap_chain_framebuffers[i]) != VK_SUCCESS)
            {
                LOG_FATAL("failed to create framebuffer!");
            }
        }
    }

    void VulkanRHI::createCommandBuffer()
    {
        m_command_buffers.resize(VulkanConstants::k_max_frames_in_flight);

        VkCommandBufferAllocateInfo alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool        = m_command_pool;
        alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = VulkanConstants::k_max_frames_in_flight;

        if (vkAllocateCommandBuffers(m_vulkan_device->m_device, &alloc_info, m_command_buffers.data()) != VK_SUCCESS)
        {
            LOG_FATAL("failed to allocate command buffers!");
        }

        alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool        = m_command_pool;
        alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(m_vulkan_device->m_device, &alloc_info, &m_transfer_buffer) != VK_SUCCESS)
        {
            LOG_FATAL("failed to allocate command buffers!");
        }
    }

    void VulkanRHI::createTextureImage()
    {
        m_vulkan_texture                   = std::make_shared<VulkanTexture>();
        m_vulkan_texture->m_asset_manager  = m_asset_manager;
        m_vulkan_texture->m_config_manager = m_config_manager;
        m_vulkan_texture->m_device         = m_vulkan_device;
        m_vulkan_texture->m_command_pool   = m_command_pool;
        m_vulkan_texture->m_address_mode   = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        m_vulkan_texture->initizlize("asset-test/data/model/viking_room/viking_room.png");
        // m_vulkan_texture->initizlize("asset-test/data/texture/object/white.tga");

        m_vulkan_texture_ui                   = std::make_shared<VulkanTexture>();
        m_vulkan_texture_ui->m_asset_manager  = m_asset_manager;
        m_vulkan_texture_ui->m_config_manager = m_config_manager;
        m_vulkan_texture_ui->m_device         = m_vulkan_device;
        m_vulkan_texture_ui->m_command_pool   = m_command_pool;
        m_vulkan_texture_ui->m_address_mode   = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        m_vulkan_texture_ui->initizlize("asset-test/data/texture/object/texture.jpg");
    }

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    // TODO : move this to scene management
    void VulkanRHI::loadModel()
    {
        std::filesystem::path model_uri = m_config_manager->getRootFolder() / "asset-test/data/model/viking_room/viking_room.obj";
        // std::filesystem::path model_uri = m_config_manager->getRootFolder() / "asset-test/data/model/nanosuit/nanosuit.obj";
        std::filesystem::path mtl_path = m_config_manager->getRootFolder() / "asset-test/data/model/viking_room";
        // std::filesystem::path model_uri = m_config_manager->getRootFolder() / "asset-test/data/model/basic/capsule.obj";
        // TODO : make this with world load
        tinyobj::attrib_t                attrib;
        std::vector<tinyobj::shape_t>    shapes;
        std::vector<tinyobj::material_t> materials;
        std::string                      warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_uri.generic_string().c_str(), mtl_path.generic_string().c_str()))
        {
            LOG_WARN(warn);
            LOG_FATAL(err);
        }

        if (!warn.empty())
        {
            LOG_WARN(warn);
        }

        std::unordered_map<Vertex, uint32_t> unique_vertices {};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex {};

                vertex.pos = {attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1], attrib.vertices[3 * index.vertex_index + 2]};

                // vertex.color = {attrib.colors[3 * index.vertex_index + 0], attrib.colors[3 * index.vertex_index + 1], attrib.colors[3 * index.vertex_index + 2]};
                vertex.color = {1.0f, 1.0f, 1.0f};

                vertex.normal = {attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1], attrib.normals[3 * index.normal_index + 2]};

                vertex.tex_coord = {attrib.texcoords[2 * index.texcoord_index + 0], 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
                // vertex.tex_coord = {attrib.texcoords[2 * index.texcoord_index + 0], attrib.texcoords[2 * index.texcoord_index + 1]};

                if (unique_vertices.count(vertex) == 0)
                {
                    unique_vertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                    m_vertices.push_back(vertex);
                }

                m_indices.push_back(unique_vertices[vertex]);
            }
        }
    }

    void VulkanRHI::createVertexBuffer()
    {
        VkDeviceSize buffer_size = sizeof(m_vertices[0]) * m_vertices.size();

        m_vulkan_vertex_buffer           = std::make_shared<VulkanBuffer>();
        m_vulkan_vertex_buffer->size     = buffer_size;
        m_vulkan_vertex_buffer->usage    = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        m_vulkan_vertex_buffer->property = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        m_vulkan_vertex_buffer->device   = m_vulkan_device;

        VulkanBufferUtils::createBuffer(
            m_vulkan_device, m_vulkan_vertex_buffer->size, m_vulkan_vertex_buffer->usage, m_vulkan_vertex_buffer->property, m_vulkan_vertex_buffer->buffer, m_vulkan_vertex_buffer->memory);

        m_vulkan_vertex_buffer->setupDescriptor();
        m_vulkan_vertex_buffer->map();
        memcpy(m_vulkan_vertex_buffer->mapped, m_vertices.data(), (size_t)buffer_size);
        m_vulkan_vertex_buffer->flush();
        m_vulkan_vertex_buffer->unmap();
    }

    void VulkanRHI::createIndexBuffer()
    {
        VkDeviceSize buffer_size = sizeof(m_indices[0]) * m_indices.size();

        m_vulkan_index_buffer           = std::make_shared<VulkanBuffer>();
        m_vulkan_index_buffer->size     = buffer_size;
        m_vulkan_index_buffer->usage    = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        m_vulkan_index_buffer->property = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        m_vulkan_index_buffer->device   = m_vulkan_device;

        VulkanBufferUtils::createBuffer(
            m_vulkan_device, m_vulkan_index_buffer->size, m_vulkan_index_buffer->usage, m_vulkan_index_buffer->property, m_vulkan_index_buffer->buffer, m_vulkan_index_buffer->memory);

        m_vulkan_index_buffer->setupDescriptor();
        m_vulkan_index_buffer->map();
        memcpy(m_vulkan_index_buffer->mapped, m_indices.data(), (size_t)buffer_size);
        m_vulkan_index_buffer->flush();
        m_vulkan_index_buffer->unmap();
    }

    void VulkanRHI::createUniformBuffers()
    {
        VkDeviceSize buffer_size = sizeof(UBO);

        m_uniform_buffers.resize(VulkanConstants::k_max_frames_in_flight);
        m_uniform_buffers_memory.resize(VulkanConstants::k_max_frames_in_flight);
        m_uniform_buffers_mapped.resize(VulkanConstants::k_max_frames_in_flight);

        m_uniform_light_buffers.resize(VulkanConstants::k_max_frames_in_flight);
        m_uniform_light_buffers_memory.resize(VulkanConstants::k_max_frames_in_flight);
        m_uniform_light_buffers_mapped.resize(VulkanConstants::k_max_frames_in_flight);

        for (size_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            auto usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            auto flag  = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            VulkanBufferUtils::createBuffer(m_vulkan_device, buffer_size, usage, flag, m_uniform_buffers[i], m_uniform_buffers_memory[i]);
            vkMapMemory(m_vulkan_device->m_device, m_uniform_buffers_memory[i], 0, buffer_size, 0, &m_uniform_buffers_mapped[i]);

            VulkanBufferUtils::createBuffer(m_vulkan_device, buffer_size, usage, flag, m_uniform_light_buffers[i], m_uniform_light_buffers_memory[i]);
            vkMapMemory(m_vulkan_device->m_device, m_uniform_light_buffers_memory[i], 0, buffer_size, 0, &m_uniform_light_buffers_mapped[i]);
        }
    }

    void VulkanRHI::createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(VulkanConstants::k_max_frames_in_flight, m_vulkan_pipeline->m_descriptor_set_layout);

        VkDescriptorSetAllocateInfo alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool     = m_descriptor_pool;
        alloc_info.descriptorSetCount = VulkanConstants::k_max_frames_in_flight;
        alloc_info.pSetLayouts        = layouts.data();

        m_descriptor_sets.resize(VulkanConstants::k_max_frames_in_flight);
        if (vkAllocateDescriptorSets(m_vulkan_device->m_device, &alloc_info, m_descriptor_sets.data()) != VK_SUCCESS)
        {
            LOG_FATAL("failed to allocate descriptor sets!");
        }

        for (uint32_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            VkDescriptorBufferInfo buffer_info {};
            buffer_info.buffer = m_uniform_buffers[i];
            buffer_info.offset = 0;
            buffer_info.range  = sizeof(UBO);

            VkDescriptorImageInfo image_info {};
            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView   = m_vulkan_texture->m_view;
            image_info.sampler     = m_vulkan_texture->m_sampler;

            VkDescriptorBufferInfo light_info {};
            light_info.buffer = m_uniform_light_buffers[i];
            light_info.offset = 0;
            light_info.range  = sizeof(Light);

            std::array<VkWriteDescriptorSet, 3> descriptor_writes {};

            descriptor_writes[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[0].dstSet          = m_descriptor_sets[i];
            descriptor_writes[0].dstBinding      = 0;
            descriptor_writes[0].dstArrayElement = 0;
            descriptor_writes[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_writes[0].descriptorCount = 1;
            descriptor_writes[0].pBufferInfo     = &buffer_info;

            descriptor_writes[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[1].dstSet          = m_descriptor_sets[i];
            descriptor_writes[1].dstBinding      = 1;
            descriptor_writes[1].dstArrayElement = 0;
            descriptor_writes[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_writes[1].descriptorCount = 1;
            descriptor_writes[1].pImageInfo      = &image_info;

            descriptor_writes[2].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[2].dstSet          = m_descriptor_sets[i];
            descriptor_writes[2].dstBinding      = 2;
            descriptor_writes[2].dstArrayElement = 0;
            descriptor_writes[2].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_writes[2].descriptorCount = 1;
            descriptor_writes[2].pBufferInfo     = &light_info;

            vkUpdateDescriptorSets(m_vulkan_device->m_device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
        }
    }

    void VulkanRHI::createBindlessDescriptorSets()
    {
        if (!m_bindless_supported)
            return;

        uint32_t max_binding = VulkanConstants::k_max_bindless_resources - 1;

        VkDescriptorSetVariableDescriptorCountAllocateInfoEXT count_info {};
        count_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
        count_info.descriptorSetCount = 1;
        count_info.pDescriptorCounts  = &max_binding; // This number is the max allocatable count

        VkDescriptorSetAllocateInfo alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool     = m_bindless_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts        = &m_bindless_set_layout;
        // alloc_info.pNext = &count_info;

        if (vkAllocateDescriptorSets(m_vulkan_device->m_device, &alloc_info, &m_bindless_set) != VK_SUCCESS)
        {
            LOG_FATAL("failed to allocate bindless descriptor sets!");
        }
    }

    void VulkanRHI::createSyncObjects()
    {
        m_image_available_semaphores.resize(VulkanConstants::k_max_frames_in_flight);
        m_render_finished_semaphores.resize(VulkanConstants::k_max_frames_in_flight);
        m_in_flight_fences.resize(VulkanConstants::k_max_frames_in_flight);

        VkSemaphoreCreateInfo semaphore_info {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            if (vkCreateSemaphore(m_vulkan_device->m_device, &semaphore_info, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_vulkan_device->m_device, &semaphore_info, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_vulkan_device->m_device, &fence_info, nullptr, &m_in_flight_fences[i]) != VK_SUCCESS)
            {
                LOG_FATAL("failed to create synchronization objects for a frame!");
            }
        }
    }

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    void VulkanRHI::createComputeDescriptorSetLayout()
    {
        std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings {};
        layoutBindings[0].binding            = 0;
        layoutBindings[0].descriptorCount    = 1;
        layoutBindings[0].descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBindings[0].pImmutableSamplers = nullptr;
        layoutBindings[0].stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT;

        layoutBindings[1].binding            = 1;
        layoutBindings[1].descriptorCount    = 1;
        layoutBindings[1].descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        layoutBindings[1].pImmutableSamplers = nullptr;
        layoutBindings[1].stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT;

        layoutBindings[2].binding            = 2;
        layoutBindings[2].descriptorCount    = 1;
        layoutBindings[2].descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        layoutBindings[2].pImmutableSamplers = nullptr;
        layoutBindings[2].stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo {};
        layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 3;
        layoutInfo.pBindings    = layoutBindings.data();

        if (vkCreateDescriptorSetLayout(m_vulkan_device->m_device, &layoutInfo, nullptr, &m_compute_descriptor_set_layout) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create compute descriptor set layout!");
        }
    }

    void VulkanRHI::createComputePipeline()
    {
        ShaderModuleCreateInfo config;
        config.m_comp_shader = "shader/glsl/shader_compute.comp";

        std::shared_ptr<VulkanShader> shader = std::make_shared<VulkanShader>(config);

        shader->m_device         = m_vulkan_device;
        shader->m_config_manager = m_config_manager;
        shader->m_asset_manager  = m_asset_manager;
        shader->initialize();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts    = &m_compute_descriptor_set_layout;

        if (vkCreatePipelineLayout(m_vulkan_device->m_device, &pipelineLayoutInfo, nullptr, &m_compute_pipeline_layout) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create compute pipeline layout!");
        }

        VkPipelineShaderStageCreateInfo compute_shader_stage_info {};
        compute_shader_stage_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        compute_shader_stage_info.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
        compute_shader_stage_info.module = shader->m_comp_shader;
        compute_shader_stage_info.pName  = "main";

        VkComputePipelineCreateInfo pipelineInfo {};
        pipelineInfo.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = m_compute_pipeline_layout;
        pipelineInfo.stage  = compute_shader_stage_info;

        if (vkCreateComputePipelines(m_vulkan_device->m_device, m_pipeline_cache, 1, &pipelineInfo, nullptr, &m_compute_pipeline) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create compute pipeline!");
        }

        shader->clear();
    }

    void VulkanRHI::createShaderStorageBuffers()
    {
        // Initialize particles
        std::default_random_engine            rnd_engine(0); //(unsigned)time(nullptr));
        std::uniform_real_distribution<float> rnd_dist(0.0f, 1.0f);

        // Initial particle positions on a circle
        std::vector<Particle> particles(VulkanConstants::k_particle_count);
        for (auto& particle : particles)
        {
            float r           = 0.25f * sqrt(rnd_dist(rnd_engine));
            float theta       = rnd_dist(rnd_engine) * 2.0f * 3.14159265358979323846f;
            float x           = r * cos(theta) * (float)m_vulkan_swap_chain->m_swap_chain_extent.height / (float)m_vulkan_swap_chain->m_swap_chain_extent.width;
            float y           = r * sin(theta);
            particle.position = {x, y};
            particle.velocity = FVector2(x, y).normalized() * 0.00025f;
            particle.color    = {rnd_dist(rnd_engine), rnd_dist(rnd_engine), rnd_dist(rnd_engine), 1.0f};
        }

        VkDeviceSize buffer_size = sizeof(Particle) * VulkanConstants::k_particle_count;

        // Create a staging buffer used to upload data to the gpu
        VkBuffer       stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VulkanBufferUtils::createBuffer(
            m_vulkan_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(m_vulkan_device->m_device, stagingBufferMemory, 0, buffer_size, 0, &data);
        memcpy(data, particles.data(), (size_t)buffer_size);
        vkUnmapMemory(m_vulkan_device->m_device, stagingBufferMemory);

        m_shader_storage_buffers.resize(VulkanConstants::k_max_frames_in_flight);
        m_shader_storage_buffers_memory.resize(VulkanConstants::k_max_frames_in_flight);

        // Copy initial particle data to all storage buffers
        for (uint32_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            VulkanBufferUtils::createBuffer(m_vulkan_device,
                                            buffer_size,
                                            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                            m_shader_storage_buffers[i],
                                            m_shader_storage_buffers_memory[i]);
            VulkanBufferUtils::copyBuffer(m_vulkan_device, m_command_pool, stagingBuffer, m_shader_storage_buffers[i], buffer_size);
        }

        vkDestroyBuffer(m_vulkan_device->m_device, stagingBuffer, nullptr);
        vkFreeMemory(m_vulkan_device->m_device, stagingBufferMemory, nullptr);
    }

    void VulkanRHI::createComputeUniformBuffers()
    {
        VkDeviceSize buffer_size = sizeof(float);

        m_particle_uniform_buffers.resize(VulkanConstants::k_max_frames_in_flight);
        m_particle_uniform_buffers_memory.resize(VulkanConstants::k_max_frames_in_flight);
        m_particle_uniform_buffers_mapped.resize(VulkanConstants::k_max_frames_in_flight);

        for (uint32_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            auto usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            auto flag  = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            VulkanBufferUtils::createBuffer(m_vulkan_device, buffer_size, usage, flag, m_particle_uniform_buffers[i], m_particle_uniform_buffers_memory[i]);

            vkMapMemory(m_vulkan_device->m_device, m_particle_uniform_buffers_memory[i], 0, buffer_size, 0, &m_particle_uniform_buffers_mapped[i]);
        }
    }

    void VulkanRHI::createComputeDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(VulkanConstants::k_max_frames_in_flight, m_compute_descriptor_set_layout);

        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = m_descriptor_pool;
        allocInfo.descriptorSetCount = VulkanConstants::k_max_frames_in_flight;
        allocInfo.pSetLayouts        = layouts.data();

        m_compute_descriptor_sets.resize(VulkanConstants::k_max_frames_in_flight);
        if (vkAllocateDescriptorSets(m_vulkan_device->m_device, &allocInfo, m_compute_descriptor_sets.data()) != VK_SUCCESS)
        {
            LOG_ERROR("failed to allocate compute descriptor sets!");
        }

        for (uint32_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            VkDescriptorBufferInfo uniformBufferInfo {};
            uniformBufferInfo.buffer = m_particle_uniform_buffers[i];
            uniformBufferInfo.offset = 0;
            uniformBufferInfo.range  = sizeof(float);

            std::array<VkWriteDescriptorSet, 3> descriptorWrites {};
            descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet          = m_compute_descriptor_sets[i];
            descriptorWrites[0].dstBinding      = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo     = &uniformBufferInfo;

            VkDescriptorBufferInfo storageBufferInfoLastFrame {};
            storageBufferInfoLastFrame.buffer = m_shader_storage_buffers[(i - 1) % VulkanConstants::k_max_frames_in_flight];
            storageBufferInfoLastFrame.offset = 0;
            storageBufferInfoLastFrame.range  = sizeof(Particle) * VulkanConstants::k_particle_count;

            descriptorWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet          = m_compute_descriptor_sets[i];
            descriptorWrites[1].dstBinding      = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo     = &storageBufferInfoLastFrame;

            VkDescriptorBufferInfo storageBufferInfoCurrentFrame {};
            storageBufferInfoCurrentFrame.buffer = m_shader_storage_buffers[i];
            storageBufferInfoCurrentFrame.offset = 0;
            storageBufferInfoCurrentFrame.range  = sizeof(Particle) * VulkanConstants::k_particle_count;

            descriptorWrites[2].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet          = m_compute_descriptor_sets[i];
            descriptorWrites[2].dstBinding      = 2;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[2].descriptorCount = 1;
            descriptorWrites[2].pBufferInfo     = &storageBufferInfoCurrentFrame;

            vkUpdateDescriptorSets(m_vulkan_device->m_device, 3, descriptorWrites.data(), 0, nullptr);
        }
    }

    void VulkanRHI::createComputeCommandBuffers()
    {
        m_compute_command_buffers.resize(VulkanConstants::k_max_frames_in_flight);

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = m_command_pool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)m_compute_command_buffers.size();

        if (vkAllocateCommandBuffers(m_vulkan_device->m_device, &allocInfo, m_compute_command_buffers.data()) != VK_SUCCESS)
        {
            LOG_FATAL("failed to allocate compute command buffers!");
        }
    }

    void VulkanRHI::createComputeSyncObjects()
    {
        m_compute_finished_semaphores.resize(VulkanConstants::k_max_frames_in_flight);
        m_compute_in_flight_fences.resize(VulkanConstants::k_max_frames_in_flight);

        VkSemaphoreCreateInfo semaphoreInfo {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            if (vkCreateSemaphore(m_vulkan_device->m_device, &semaphoreInfo, nullptr, &m_compute_finished_semaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_vulkan_device->m_device, &fenceInfo, nullptr, &m_compute_in_flight_fences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create compute synchronization objects for a frame!");
            }
        }
    }

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    void VulkanRHI::createImGui()
    {
        m_vulkan_ui = std::make_shared<VulkanUI>();

        m_vulkan_ui->m_window = m_initialize_info.window_system->getWindow();

        m_vulkan_ui->m_instance        = m_vulkan_instance;
        m_vulkan_ui->m_device          = m_vulkan_device;
        m_vulkan_ui->m_image_count     = static_cast<uint32_t>(m_vulkan_swap_chain->m_images.size());
        m_vulkan_ui->m_image_format    = m_vulkan_swap_chain->m_surface_format.format;
        m_vulkan_ui->m_command_pool    = m_command_pool;
        m_vulkan_ui->m_descriptor_pool = m_descriptor_pool;
        m_vulkan_ui->m_asset_manager   = m_asset_manager;
        m_vulkan_ui->m_config_manager  = m_config_manager;
        m_vulkan_ui->m_pipeline_cache  = m_pipeline_cache;
        m_vulkan_ui->m_ui_pass         = m_vulkan_render_pass->m_render_pass;

        m_vulkan_ui->initialize();

        m_vulkan_texture->m_descriptor_set_layout = m_vulkan_ui->m_descriptor_set_layout;
        m_vulkan_texture->m_descriptor_pool       = m_vulkan_ui->m_descriptor_pool;
        m_vulkan_texture->createDescriptorSet();

        m_vulkan_texture_ui->m_descriptor_set_layout = m_vulkan_ui->m_descriptor_set_layout;
        m_vulkan_texture_ui->m_descriptor_pool       = m_vulkan_ui->m_descriptor_pool;
        m_vulkan_texture_ui->createDescriptorSet();
    }

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    void VulkanRHI::initialize(RHIInitInfo initialize_info)
    {
        m_initialize_info = initialize_info;

        createInstance();

        createVulkanDevice();

        createSwapChain();

        createDescriptorPool();
        createBindlessDescriptorPool();

        createBindlessDescriptorSetLayout();

        createRenderPass();
        createGraphicsPipelineCache();
        createGraphicsPipeline();

        createCommandPool();

        createDepthResources();
        createFramebuffers();

        createCommandBuffer();

        createTextureImage();

        loadModel();

        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorSets();

        createBindlessDescriptorSets();

        createSyncObjects();

        createComputeDescriptorSetLayout();
        createComputePipeline();
        createShaderStorageBuffers();
        createComputeUniformBuffers();
        createComputeDescriptorSets();
        createComputeCommandBuffers();
        createComputeSyncObjects();

        createImGui();
    }

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    void VulkanRHI::recordComputeCommandBuffer(VkCommandBuffer command_buffer)
    {
        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(command_buffer, &beginInfo) != VK_SUCCESS)
        {
            LOG_FATAL("failed to begin recording compute command buffer!");
        }

        VulkanDebugUtils::cmdBeginLabel(command_buffer, "subpass 0: compute pass", {1.0f, 0.78f, 0.05f, 1.0f});

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_compute_pipeline);

        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_compute_pipeline_layout, 0, 1, &m_compute_descriptor_sets[m_current_frame], 0, nullptr);

        vkCmdDispatch(command_buffer, VulkanConstants::k_particle_count / 256, 1, 1);

        VulkanDebugUtils::cmdEndLabel(command_buffer);

        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
        {
            LOG_FATAL("failed to record compute command buffer!");
        }
    }

    void VulkanRHI::recordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index)
    {
        VkCommandBufferBeginInfo begin_info {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
        {
            LOG_FATAL("failed to begin recording command buffer!");
        }

        {
            VkRenderPassBeginInfo render_pass_info {};
            render_pass_info.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass        = m_vulkan_render_pass->m_render_pass;
            render_pass_info.framebuffer       = m_swap_chain_framebuffers[image_index];
            render_pass_info.renderArea.offset = {0, 0};
            render_pass_info.renderArea.extent = m_vulkan_swap_chain->m_swap_chain_extent;

            std::array<VkClearValue, 2> clear_color {};
            clear_color[0].color        = {{0.2f, 0.3f, 0.4f, 1.0f}};
            clear_color[1].depthStencil = {1.0f, 0};

            render_pass_info.clearValueCount = static_cast<uint32_t>(clear_color.size());
            render_pass_info.pClearValues    = clear_color.data();

            vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

            {
                VulkanDebugUtils::cmdBeginLabel(command_buffer, "subpass 1: color pass", {0.0f, 0.5f, 1.0f, 1.0f});

                vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vulkan_pipeline->m_pipeline);

                // https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/
                VkViewport viewport {};
                viewport.x        = 0.0f;
                viewport.y        = (float)m_vulkan_swap_chain->m_swap_chain_extent.height;
                viewport.width    = (float)m_vulkan_swap_chain->m_swap_chain_extent.width;
                viewport.height   = -(float)m_vulkan_swap_chain->m_swap_chain_extent.height;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vkCmdSetViewport(command_buffer, 0, 1, &viewport);

                VkRect2D scissor {};
                scissor.offset = {0, 0};
                scissor.extent = m_vulkan_swap_chain->m_swap_chain_extent;
                vkCmdSetScissor(command_buffer, 0, 1, &scissor);

                VkBuffer     vertex_buffers[] = {m_vulkan_vertex_buffer->buffer};
                VkDeviceSize offsets[]        = {0};
                vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

                vkCmdBindIndexBuffer(command_buffer, m_vulkan_index_buffer->buffer, 0, VK_INDEX_TYPE_UINT32);

                vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vulkan_pipeline->m_pipeline_layout, 0, 1, &m_descriptor_sets[m_current_frame], 0, nullptr);

                vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);

                VulkanDebugUtils::cmdEndLabel(command_buffer);
            }

            {
                // VulkanDebugUtils::cmdBeginLabel(command_buffer, "subpass 1.1: color pass", {0.0f, 0.5f, 1.0f, 1.0f});

                // VkBuffer     vertex_buffers[] = {m_vulkan_vertex_buffer->buffer};
                // VkDeviceSize offsets[]        = {0};
                // vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
                // vkCmdBindIndexBuffer(command_buffer, m_vulkan_index_buffer->buffer, 0, VK_INDEX_TYPE_UINT32);
                // vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vulkan_pipeline->m_pipeline_layout, 0, 1, &m_descriptor_sets[m_current_frame], 0, nullptr);
                // vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);

                // VulkanDebugUtils::cmdEndLabel(command_buffer);
            }

            {
                VulkanDebugUtils::cmdBeginLabel(command_buffer, "subpass 2: ui pass", {0.5f, 0.76f, 0.34f, 1.0f});

                vkCmdNextSubpass(command_buffer, VK_SUBPASS_CONTENTS_INLINE);

                m_vulkan_ui->recordCommandBuffer(command_buffer, m_swap_chain_framebuffers[image_index]);

                VulkanDebugUtils::cmdEndLabel(command_buffer);
            }

            vkCmdEndRenderPass(command_buffer);
        }

        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
        {
            LOG_FATAL("failed to record command buffer!");
        }
    }

    void VulkanRHI::updateUniformBuffer(uint32_t current_image)
    {
        m_dt_ubo = 1.0f;

        // update particle ubo
        memcpy(m_particle_uniform_buffers_mapped[current_image], &m_dt_ubo, sizeof(float));
        // update required data
        memcpy(m_uniform_buffers_mapped[current_image], &m_ubo, sizeof(m_ubo));
        // update light info
        memcpy(m_uniform_light_buffers_mapped[current_image], &m_light_ubo, sizeof(m_light_ubo));
    }

    void VulkanRHI::prepareContext()
    {
        // wait compute fence (first render pass)
        // Compute submission
        vkWaitForFences(m_vulkan_device->m_device, 1, &m_compute_in_flight_fences[m_current_frame], VK_TRUE, UINT64_MAX);
        vkResetFences(m_vulkan_device->m_device, 1, &m_compute_in_flight_fences[m_current_frame]);

        updateUniformBuffer(m_current_frame);
    }

    void VulkanRHI::drawFrame()
    {
        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        vkResetCommandBuffer(m_compute_command_buffers[m_current_frame], /*VkCommandBufferResetFlagBits*/ 0);
        recordComputeCommandBuffer(m_compute_command_buffers[m_current_frame]);

        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &m_compute_command_buffers[m_current_frame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &m_compute_finished_semaphores[m_current_frame];

        if (vkQueueSubmit(m_vulkan_device->m_compute_queue, 1, &submitInfo, m_compute_in_flight_fences[m_current_frame]) != VK_SUCCESS)
        {
            LOG_FATAL("failed to submit compute command buffer!");
        };

        vkWaitForFences(m_vulkan_device->m_device, 1, &m_in_flight_fences[m_current_frame], VK_TRUE, UINT64_MAX);
        vkResetFences(m_vulkan_device->m_device, 1, &m_in_flight_fences[m_current_frame]);

        // TODO : Although many drivers and platforms trigger VK_ERROR_OUT_OF_DATE_KHR automatically after a window resize, it is not guaranteed to happen.
        // That's why we'll add some extra code to also handle resizes explicitly.

        // handle swap chain recreation
        uint32_t image_index;

        VkResult result = m_vulkan_swap_chain->acquireNextImage(m_image_available_semaphores[m_current_frame], &image_index);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            LOG_FATAL("failed to acquire swap chain image!");
        }

        vkResetFences(m_vulkan_device->m_device, 1, &m_in_flight_fences[m_current_frame]);

        vkResetCommandBuffer(m_command_buffers[m_current_frame], /*VkCommandBufferResetFlagBits*/ 0);
        recordCommandBuffer(m_command_buffers[m_current_frame], image_index);

        std::array<VkSemaphore, 2>          wait_semaphores   = {m_compute_finished_semaphores[m_current_frame], m_image_available_semaphores[m_current_frame]};
        std::array<VkPipelineStageFlags, 2> wait_stages       = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        std::array<VkSemaphore, 1>          signal_semaphores = {m_render_finished_semaphores[m_current_frame]};

        VkSubmitInfo submit_info {};
        submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount   = static_cast<uint32_t>(wait_semaphores.size());
        submit_info.pWaitSemaphores      = wait_semaphores.data();
        submit_info.pWaitDstStageMask    = wait_stages.data();
        submit_info.commandBufferCount   = 1;
        submit_info.pCommandBuffers      = &m_command_buffers[m_current_frame];
        submit_info.signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size());
        submit_info.pSignalSemaphores    = signal_semaphores.data();

        if (vkQueueSubmit(m_vulkan_device->m_graphics_queue, 1, &submit_info, m_in_flight_fences[m_current_frame]) != VK_SUCCESS)
        {
            LOG_FATAL("failed to submit draw command buffer!");
        }

        result = m_vulkan_swap_chain->queuePresent(m_vulkan_device->m_present_queue, image_index, m_render_finished_semaphores[m_current_frame]);
        if (VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
        {
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            LOG_FATAL("failed to present swap chain image!");
        }

        m_current_frame = (m_current_frame + 1) % VulkanConstants::k_max_frames_in_flight;
    }

    void VulkanRHI::render()
    {
        m_vulkan_ui->prepareContext(m_vulkan_swap_chain->m_swap_chain_extent.width, m_vulkan_swap_chain->m_swap_chain_extent.height);
        m_vulkan_ui->renderUI();
        m_vulkan_ui->showImage(m_vulkan_texture, "simple texture");
        m_vulkan_ui->showImage(m_vulkan_texture_ui, "simple texture 2");
        m_vulkan_ui->renderFinish();

        drawFrame();
    }

    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------
    // ---------------------------------------------------------------------------

    void VulkanRHI::clear()
    {
        m_vulkan_device->wait();

        m_vulkan_texture_ui->clear();
        m_vulkan_texture_ui.reset();

        m_vulkan_texture->clear();
        m_vulkan_texture.reset();

        m_vulkan_ui->clear();
        m_vulkan_ui.reset();

        m_vulkan_pipeline->clear();
        m_vulkan_pipeline.reset();

        m_vulkan_render_pass->clear();
        m_vulkan_render_pass.reset();

        m_vulkan_vertex_buffer->destroy();
        m_vulkan_index_buffer->destroy();

        for (uint32_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            vkDestroySemaphore(m_vulkan_device->m_device, m_compute_finished_semaphores[i], nullptr);
            vkDestroyFence(m_vulkan_device->m_device, m_compute_in_flight_fences[i], nullptr);
        }

        for (uint32_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            vkDestroySemaphore(m_vulkan_device->m_device, m_image_available_semaphores[i], nullptr);
            vkDestroySemaphore(m_vulkan_device->m_device, m_render_finished_semaphores[i], nullptr);
            vkDestroyFence(m_vulkan_device->m_device, m_in_flight_fences[i], nullptr);
        }

        for (uint32_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            VulkanBufferUtils::destroyBuffer(m_vulkan_device, m_particle_uniform_buffers[i], m_particle_uniform_buffers_memory[i]);
        }

        for (uint32_t i = 0; i < VulkanConstants::k_max_frames_in_flight; i++)
        {
            VulkanBufferUtils::destroyBuffer(m_vulkan_device, m_particle_uniform_buffers[i], m_particle_uniform_buffers_memory[i]);
            VulkanBufferUtils::destroyBuffer(m_vulkan_device, m_uniform_light_buffers[i], m_uniform_light_buffers_memory[i]);
            VulkanBufferUtils::destroyBuffer(m_vulkan_device, m_uniform_buffers[i], m_uniform_buffers_memory[i]);
            VulkanBufferUtils::destroyBuffer(m_vulkan_device, m_shader_storage_buffers[i], m_shader_storage_buffers_memory[i]);
        }

        vkDestroyCommandPool(m_vulkan_device->m_device, m_command_pool, nullptr);

        // VulkanBufferUtils::destroyBuffer(m_vulkan_device, m_index_buffer, m_index_buffer_memory);
        // VulkanBufferUtils::destroyBuffer(m_vulkan_device, m_vertex_buffer, m_vertex_buffer_memory);

        vkDestroyImageView(m_vulkan_device->m_device, m_depth_image_view, nullptr);
        vkDestroyImage(m_vulkan_device->m_device, m_depth_image, nullptr);
        vkFreeMemory(m_vulkan_device->m_device, m_depth_image_memory, nullptr);

        for (auto framebuffer : m_swap_chain_framebuffers)
        {
            vkDestroyFramebuffer(m_vulkan_device->m_device, framebuffer, nullptr);
        }

        vkDestroyPipeline(m_vulkan_device->m_device, m_compute_pipeline, nullptr);
        vkDestroyPipelineLayout(m_vulkan_device->m_device, m_compute_pipeline_layout, nullptr);

        if (m_bindless_supported)
        {
            vkDestroyDescriptorSetLayout(m_vulkan_device->m_device, m_bindless_set_layout, nullptr);
            vkDestroyDescriptorPool(m_vulkan_device->m_device, m_bindless_pool, nullptr);
        }

        vkDestroyDescriptorSetLayout(m_vulkan_device->m_device, m_compute_descriptor_set_layout, nullptr);
        // vkDestroyDescriptorSetLayout(m_vulkan_device->m_device, m_descriptor_set_layout, nullptr);

        vkDestroyDescriptorPool(m_vulkan_device->m_device, m_descriptor_pool, nullptr);

        // store pipeline cache
        {
            size_t cache_size = 0;
            vkGetPipelineCacheData(m_vulkan_device->m_device, m_pipeline_cache, &cache_size, nullptr);
            std::vector<uint8_t> cache;
            cache.resize(cache_size);
            vkGetPipelineCacheData(m_vulkan_device->m_device, m_pipeline_cache, &cache_size, cache.data());

            auto cache_path = m_config_manager->getRootFolder() / "pipeline.cache";

            FILE* file = fopen(cache_path.generic_string().c_str(), "wb");
            fwrite(cache.data(), cache.size(), 1, file);
            fclose(file);
        }

        vkDestroyPipelineCache(m_vulkan_device->m_device, m_pipeline_cache, nullptr);

        m_vulkan_swap_chain->clear();
        m_vulkan_swap_chain.reset();

        m_vulkan_device->clear();
        m_vulkan_device.reset();

        m_vulkan_instance->clear();
        m_vulkan_instance.reset();
    }
} // namespace ArchViz
