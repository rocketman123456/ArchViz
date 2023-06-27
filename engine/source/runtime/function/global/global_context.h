#pragma once
#include <memory>
#include <string>

namespace ArchViz
{
    class ConfigManager;
    class FileService;
    class LogSystem;
    class AssetManager;
    // class PhysicsManager;
    // class InputSystem;
    // class WorldManager;
    // class RenderSystem;
    class WindowSystem;
    // class ParticleManager;
    // class DebugDrawManager;
    // class RenderDebugConfig;
    class VFS;

    // struct EngineInitParams;

    /// Manage the lifetime and creation/destruction order of all global system
    class RuntimeGlobalContext
    {
    public:
        // create all global systems and initialize these systems
        void startSystems(const std::string& config_file_path);
        // destroy all global systems
        void shutdownSystems();

    public:
        std::shared_ptr<LogSystem>      m_logger_system;
        std::shared_ptr<FileService>    m_file_service;
        std::shared_ptr<AssetManager>   m_asset_manager;
        std::shared_ptr<ConfigManager>  m_config_manager;
        // std::shared_ptr<WorldManager>   m_world_manager;
        // std::shared_ptr<PhysicsManager> m_physics_manager;
        // std::shared_ptr<InputSystem>    m_input_system;
        std::shared_ptr<WindowSystem>   m_window_system;
        // std::shared_ptr<RenderSystem>   m_render_system;
        // std::shared_ptr<ParticleManager>   m_particle_manager;
        // std::shared_ptr<DebugDrawManager>  m_debugdraw_manager;
        // std::shared_ptr<RenderDebugConfig> m_render_debug_config;
        std::shared_ptr<VFS> m_vfs;
    };

    extern RuntimeGlobalContext g_runtime_global_context;
} // namespace Piccolo