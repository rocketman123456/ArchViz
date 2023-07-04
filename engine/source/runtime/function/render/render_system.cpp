#include "runtime/function/render/render_system.h"
#include "runtime/function/render/rhi/vulkan/vulkan_rhi.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/window/window_system.h"

namespace ArchViz
{
    RenderSystem::~RenderSystem() { clear(); }

    void RenderSystem::setConfigManager(std::shared_ptr<ConfigManager> config_manager)
    {
        ASSERT(config_manager);
        m_config_manager = config_manager;
    }

    void RenderSystem::setAssetManager(std::shared_ptr<AssetManager> asset_manager)
    {
        ASSERT(asset_manager);
        m_asset_manager = asset_manager;
    }

    void RenderSystem::initialize(RenderSystemInitInfo init_info)
    {
        // render context initialize
        RHIInitInfo rhi_init_info;
        rhi_init_info.window_system = init_info.window_system;

        m_rhi = std::make_shared<VulkanRHI>();
        m_rhi->setConfigManager(m_config_manager);
        m_rhi->setAssetManager(m_asset_manager);
        m_rhi->initialize(rhi_init_info);
    }

    void RenderSystem::tick(float delta_time)
    {
        // process swap data between logic and render contexts
        processSwapData();

        // prepare render command context
        m_rhi->prepareContext();

        // render context
        m_rhi->render();
    }

    void RenderSystem::processSwapData()
    {
        // RenderSwapData& swap_data = m_swap_context.getRenderSwapData();
    }

    void RenderSystem::clear()
    {
        if (m_rhi)
        {
            m_rhi->clear();
        }
        m_rhi.reset();
    }
} // namespace ArchViz
