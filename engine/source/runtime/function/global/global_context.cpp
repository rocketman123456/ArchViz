#include "runtime/function/global/global_context.h"

#include "runtime/core/meta/reflection/reflection_register.h"

#include "runtime/function/render/render_system.h"
#include "runtime/function/window/window_system.h"
#include "runtime/platform/file_service/file_service.h"
#include "runtime/platform/file_system/vfs.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"
#include "runtime/resource/resource_manager/resource_manager.h"

namespace ArchViz
{
    RuntimeGlobalContext g_runtime_global_context;

    void RuntimeGlobalContext::startSystems(const std::string& config_file_path)
    {
        // m_log_system = std::make_shared<LogSystem>();

        Reflection::TypeMetaRegister::metaRegister();

        m_config_manager = std::make_shared<ConfigManager>();
        m_config_manager->initialize(config_file_path);

        m_file_service = std::make_shared<FileService>();

        m_asset_manager = std::make_shared<AssetManager>();
        // m_asset_manager->setConfigManager(m_config_manager);

        VFSConfig config;
        m_asset_manager->loadAsset<VFSConfig>("config/config.vfs.json", config);
        std::shared_ptr<VFS> vfs = std::make_shared<VFS>();
        vfs->mount(config);

        m_asset_manager->setVFS(vfs);

        m_resource_manager = std::make_shared<ResourceManager>();
        m_resource_manager->initialize();
    }

    void RuntimeGlobalContext::shutdownSystems()
    {
        m_resource_manager->clear();
        m_resource_manager.reset();

        m_asset_manager.reset();
        m_file_service.reset();
        m_config_manager.reset();
    }
} // namespace ArchViz