#include "runtime/platform/file_system/vfs.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include <filesystem>
#include <iostream>
#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

using namespace ArchViz;
using namespace std;

int main(int argc, char** argv)
{
    std::filesystem::path executable_path(argv[0]);
    std::filesystem::path config_file_path = executable_path.parent_path() / "../ArchVizEditor.ini";
    cout << config_file_path << endl;

    std::shared_ptr<ConfigManager> config_manager = std::make_shared<ConfigManager>();
    config_manager->initialize(config_file_path.generic_string());

    auto model_path = config_manager->getRootFolder() / "asset-test/data/model/basic/cube.obj";

    tinyobj::ObjReaderConfig config;
    tinyobj::ObjReader       reader;

    reader.ParseFromFile(model_path.generic_string());

    auto& shapes    = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    return 0;
}
