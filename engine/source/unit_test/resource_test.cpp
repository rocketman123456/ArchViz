#include "runtime/function/global/global_context.h"

#include "runtime/platform/file_system/vfs.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"
#include "runtime/resource/resource_manager/resource_manager.h"

#include "runtime/resource/res_type/components/material_res.h"
#include "runtime/resource/res_type/data/material_data.h"

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

    TextureRes texture_res {"asset-test/data/texture/default/albedo.jpg"};

    ResHandle texture = g_runtime_global_context.m_resource_manager->loadResource<TextureData, TextureRes>("asset-test/data/texture/default/albedo.jpg", texture_res);

    ResHandle texture_2 = g_runtime_global_context.m_resource_manager->loadResource<TextureData, TextureRes>("asset-test/data/texture/default/albedo.jpg");

    ResHandle material = g_runtime_global_context.m_resource_manager->loadResource<MaterialData, MaterialRes>("asset-test/material/white.material.json");

    return 0;
}
