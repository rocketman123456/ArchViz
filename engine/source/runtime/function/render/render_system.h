#pragma once
#include "runtime/function/render/render_data.h"
#include "runtime/function/render/render_type.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include <array>
#include <memory>
#include <optional>

namespace ArchViz
{
    class WindowSystem;
    class RHI;

    struct RenderSystemInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
    };

    struct EngineContentViewport
    {
        float x {0.f};
        float y {0.f};
        float width {0.f};
        float height {0.f};
    };

    class RenderSystem
    {
    public:
        RenderSystem() = default;
        ~RenderSystem();

        void initialize(RenderSystemInitInfo init_info);
        void tick(float delta_time);
        void clear();

        void setConfigManager(std::shared_ptr<ConfigManager> config_manager);
        void setAssetManager(std::shared_ptr<AssetManager> asset_manager);

    private:
        void processSwapData();

    private:
        RENDER_PIPELINE_TYPE m_render_pipeline_type {RENDER_PIPELINE_TYPE::DEFERRED_PIPELINE};

        // RenderSwapContext m_swap_context;

        std::shared_ptr<RHI> m_rhi;
        // std::shared_ptr<RenderCamera>       m_render_camera;
        // std::shared_ptr<RenderScene>        m_render_scene;
        // std::shared_ptr<RenderResourceBase> m_render_resource;
        // std::shared_ptr<RenderPipelineBase> m_render_pipeline;

        std::shared_ptr<ConfigManager> m_config_manager;
        std::shared_ptr<AssetManager>  m_asset_manager;
    };
} // namespace ArchViz
