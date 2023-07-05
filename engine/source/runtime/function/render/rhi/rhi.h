#pragma once

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
// #include <vk_mem_alloc.h>

#include <functional>
#include <memory>
#include <vector>

namespace ArchViz
{
    class ConfigManager;
    class AssetManager;
    class WindowSystem;

    struct RHIInitInfo
    {
        std::shared_ptr<WindowSystem> window_system;
    };

    class RHI
    {
    public:
        virtual ~RHI() = 0;

        virtual void setConfigManager(std::shared_ptr<ConfigManager> config_manager) = 0;
        virtual void setAssetManager(std::shared_ptr<AssetManager> asset_manager)    = 0;

        virtual void initialize(RHIInitInfo initialize_info) = 0;
        virtual void prepareContext()                        = 0;
        virtual void render()                                = 0;

        virtual void createSwapChain()   = 0;
        virtual void recreateSwapChain() = 0;

        // // destory
        virtual void clear() = 0;

    private:
    };

    inline RHI::~RHI() = default;
} // namespace ArchViz
