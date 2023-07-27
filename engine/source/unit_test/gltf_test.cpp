#include "runtime/platform/file_system/vfs.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

using namespace ArchViz;
using namespace std;

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

bool loadModelFromFile(tinygltf::Model& model, const char* filename)
{
    tinygltf::TinyGLTF loader;
    std::string        err;
    std::string        warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty())
    {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty())
    {
        std::cout << "ERR: " << err << std::endl;
    }

    if (!res)
        std::cout << "Failed to load glTF: " << filename << std::endl;
    else
        std::cout << "Loaded glTF: " << filename << std::endl;

    return res;
}

bool loadModelFromString(tinygltf::Model& model, const std::string& content, const std::string base_path)
{
    tinygltf::TinyGLTF loader;
    std::string        err;
    std::string        warn;

    bool res = loader.LoadASCIIFromString(&model, &err, &warn, content.c_str(), content.length(), base_path);
    if (!warn.empty())
    {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty())
    {
        std::cout << "ERR: " << err << std::endl;
    }

    if (!res)
        std::cout << "Failed to load glTF: " << std::endl;
    else
        std::cout << "Loaded glTF: " << std::endl;

    return res;
}

// TODO : use custom loader to use vfs for file management
// void SetImageLoader(LoadImageDataFunction LoadImageData, void *user_data);
// typedef bool (*WriteImageDataFunction)(const std::string *basepath,
//                                        const std::string *filename,
//                                        const Image *image, bool embedImages,
//                                        const URICallbacks *uri_cb,
//                                        std::string *out_uri,
//                                        void *user_pointer);

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

    std::string file_path = "asset/model-glTF/2.0/Cube/glTF/Cube.gltf";
    //std::string file_path = "asset/model-glTF/2.0/Sponza/glTF/Sponza.gltf";
    std::string base_path = (config_manager->getRootFolder() / file_path).parent_path().generic_string();
    std::string content;
    asset_manager->readTextFile(file_path, content);

    tinygltf::Model model;
    loadModelFromString(model, content, base_path);

    return 0;
}
