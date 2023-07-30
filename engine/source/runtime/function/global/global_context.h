#pragma once
#include <memory>
#include <string>

namespace ArchViz
{
    class VFS;
    class FileService;
    class ConfigManager;
    class AssetManager;
    class ResourceManager;
    class WindowSystem;
    class RenderSystem;

    /// Manage the lifetime and creation/destruction order of all global system
    class RuntimeGlobalContext
    {
    public:
        // create all global systems and initialize these systems
        void startSystems(const std::string& config_file_path);
        // destroy all global systems
        void shutdownSystems();

    public:
        std::shared_ptr<FileService>     m_file_service;
        std::shared_ptr<ConfigManager>   m_config_manager;
        std::shared_ptr<AssetManager>    m_asset_manager;
        std::shared_ptr<ResourceManager> m_resource_manager;
        std::shared_ptr<WindowSystem>    m_window_system;
        std::shared_ptr<RenderSystem>    m_render_system;
    };

    extern RuntimeGlobalContext g_runtime_global_context;
} // namespace ArchViz