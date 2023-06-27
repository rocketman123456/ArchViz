#include "runtime/platform/file_system/vfs.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/window/window_system.h"

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

    std::shared_ptr<ConfigManager> config_manager = std::make_shared<ConfigManager>();
    config_manager->initialize(config_file_path.generic_string());

    std::shared_ptr<AssetManager> asset_manager = std::make_shared<AssetManager>();
    asset_manager->initialize(config_manager);

    VFSConfig config;
    asset_manager->loadAsset<VFSConfig>("config/config.vfs.json", config);
    std::shared_ptr<VFS> vfs = std::make_shared<VFS>();
    vfs->mount(config);

    asset_manager->initialize(vfs);

    std::shared_ptr<WindowSystem> window_system = std::make_shared<WindowSystem>();
    WindowCreateInfo window_create_info;
    asset_manager->loadAsset<WindowCreateInfo>("config/config.window.json", window_create_info);
    window_system->initialize(window_create_info);

    while(!window_system->shouldClose())
    {
        window_system->pollEvents();
    }

    return 0;
}
