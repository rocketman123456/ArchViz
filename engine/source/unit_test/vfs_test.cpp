#include "runtime/function/global/global_context.h"

#include "runtime/platform/file_system/vfs.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include <filesystem>
#include <iostream>
#include <memory>

using namespace ArchViz;
using namespace std;

int main(int argc, char** argv)
{
    std::filesystem::path executable_path(argv[0]);
    std::filesystem::path config_file_path = executable_path.parent_path() / "../ArchVizEditor.ini";
    cout << config_file_path << endl;

    g_runtime_global_context.startSystems(config_file_path.generic_string());

    // std::shared_ptr<ConfigManager> config_manager = std::make_shared<ConfigManager>();
    // config_manager->initialize(config_file_path.generic_string());

    // std::shared_ptr<AssetManager> asset_manager = std::make_shared<AssetManager>();
    // asset_manager->setConfigManager(config_manager);

    // VFSConfig config;
    // asset_manager->loadAsset<VFSConfig>("config/config.vfs.json", config);
    // std::shared_ptr<VFS> vfs = std::make_shared<VFS>();
    // vfs->mount(config);

    // asset_manager->setVFS(vfs);

    return 0;
}
