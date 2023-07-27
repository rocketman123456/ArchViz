#include "runtime/platform/file_system/vfs.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/global/global_context.h"

#include "runtime/core/meta/reflection/reflection_register.h"

#include "runtime/resource/resource_manager/resource_manager.h"

#include "runtime/resource/res_type/common/level_res.h"
// #include "runtime/resource/res_type/components/material_res.h"
// #include "runtime/resource/res_type/components/mesh_res.h"

// #include "runtime/core/math/vector/vector2.h"
// #include "runtime/core/math/vector/vector3.h"
// #include "runtime/core/math/vector/vector4.h"

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

    g_runtime_global_context.startSystems(config_file_path.generic_string());

    Level level;
    level.load("asset-test/level/0-1.level.json");

    return 0;
}
