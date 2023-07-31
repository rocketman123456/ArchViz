#include "runtime/platform/file_system/vfs.h"
#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/render/rhi/spirv_parser.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_shader_utils.h"

#include "runtime/function/global/global_context.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

using namespace ArchViz;
using namespace std;

int main(int argc, char** argv)
{
    std::filesystem::path executable_path(argv[0]);
    std::filesystem::path config_file_path = executable_path.parent_path() / "../ArchVizEditor.ini";
    cout << config_file_path << endl;

    g_runtime_global_context.startSystems(config_file_path.generic_string());

    std::vector<uint32_t> vert = VulkanShaderUtils::createShaderModuleFromVFS("shader/glsl/shader_textures.vert");
    std::vector<uint32_t> frag = VulkanShaderUtils::createShaderModuleFromVFS("shader/glsl/shader_textures.frag");

    SPIRV::ParseResult result;
    std::string name_buffer;
    SPIRV::parse_binary(vert, name_buffer, result);

    return 0;
}
