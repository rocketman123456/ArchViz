#include "runtime/platform/file_system/vfs.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/core/meta/reflection/reflection_register.h"

#include "runtime/resource/res_type/common/level_res.h"
#include "runtime/resource/res_type/components/material_res.h"
#include "runtime/resource/res_type/components/mesh_res.h"

#include "runtime/function/framework/level/level.h"

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
    asset_manager->setConfigManager(config_manager);

    VFSConfig config;
    asset_manager->loadAsset<VFSConfig>("config/config.vfs.json", config);
    std::shared_ptr<VFS> vfs = std::make_shared<VFS>();
    vfs->mount(config);

    asset_manager->setVFS(vfs);

    Reflection::TypeMetaRegister::metaRegister();

    Level level;
    level.m_asset_manager = asset_manager;
    level.load("asset-test/level/0-1.level.json");

    return 0;
}
