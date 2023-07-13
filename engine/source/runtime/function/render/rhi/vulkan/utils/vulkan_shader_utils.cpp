#include "runtime/function/render/rhi/vulkan/utils/vulkan_shader_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_header_includer.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/string/string_utils.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include <SPIRV/GLSL.ext.EXT.h>
#include <SPIRV/GLSL.ext.KHR.h>
#include <SPIRV/GLSL.std.450.h>
#include <SPIRV/GlslangToSpv.h>
#include <SPIRV/disassemble.h>
#include <SPIRV/doc.h>

#include <glslang/Include/ResourceLimits.h>
#include <glslang/Include/ShHandle.h>

#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace ArchViz
{
    EShLanguage shaderLanguageStageFromFileName(const char* fileName)
    {
        if (end_with(fileName, ".vert"))
            return EShLangVertex;
        if (end_with(fileName, ".frag"))
            return EShLangFragment;
        if (end_with(fileName, ".geom"))
            return EShLangGeometry;
        if (end_with(fileName, ".comp"))
            return EShLangCompute;
        if (end_with(fileName, ".tesc"))
            return EShLangTessControl;
        if (end_with(fileName, ".tese"))
            return EShLangTessEvaluation;
        return EShLangVertex;
    }

    std::vector<uint32_t> VulkanShaderUtils::createShaderModuleFromVFS(const std::string& shader_file, std::shared_ptr<ConfigManager> config_manager, std::shared_ptr<AssetManager> asset_manager)
    {
        std::filesystem::path root_path        = config_manager->getRootFolder();
        std::filesystem::path shader_file_path = root_path / "shader" / "glsl" / shader_file;
        std::filesystem::path include_path     = root_path / "shader" / "include";
        LOG_DEBUG("open shader: " + shader_file_path.generic_string());

        std::string shader_code = "";
        // asset_manager->readTextFile(shader_file_path, shader_code);
        asset_manager->readVFSTextFile(shader_file, shader_code);

        EShLanguage stage = shaderLanguageStageFromFileName(shader_file.c_str());

        VulkanHeaderIncluder includer;
        includer.pushExternalLocalDirectory(include_path.generic_string());

        auto client          = glslang::EShClientVulkan;
        auto client_version  = glslang::EShTargetVulkan_1_0;
        auto target_language = glslang::EShTargetSpv;
        auto target_version  = glslang::EShTargetSpv_1_0;
        auto messages        = EShMsgDefault;

        glslang::InitializeProcess();

        glslang::TProgram* program = new glslang::TProgram;
        glslang::TShader*  shader  = new glslang::TShader(stage);

        const char* file_names[]   = {shader_file.c_str()};
        const char* shader_codes[] = {shader_code.c_str()};
        shader->setStringsWithLengthsAndNames(shader_codes, NULL, file_names, 1);

        std::string              preamble_str = "";
        std::vector<std::string> processes    = {};

        shader->setPreamble(preamble_str.c_str());
        shader->addProcesses(processes);

        shader->setEnvInput(glslang::EShSourceGlsl, stage, client, 100);
        shader->setEnvClient(client, client_version);
        shader->setEnvTarget(target_language, target_version);

        const TBuiltInResource* resources      = GetDefaultResources();
        const int               defaultVersion = 100;
        std::string             str;

        if (!shader->preprocess(resources, defaultVersion, ENoProfile, false, false, messages, &str, includer))
        {
            LOG_ERROR(shader->getInfoLog());
            LOG_FATAL(shader->getInfoDebugLog());
        }

        if (!shader->parse(resources, defaultVersion, false, messages, includer))
        {
            LOG_ERROR(shader->getInfoLog());
            LOG_FATAL(shader->getInfoDebugLog());
        }

        program->addShader(shader);

        if (!program->link(messages))
        {
            LOG_ERROR(program->getInfoLog());
            LOG_FATAL(program->getInfoDebugLog());
        }

        if (!program->mapIO())
        {
            LOG_ERROR(program->getInfoLog());
            LOG_FATAL(program->getInfoDebugLog());
        }

        bool SpvToolsDisassembler = false;
        bool SpvToolsValidate     = false;

        std::vector<uint32_t> spirv;
        if (program->getIntermediate(stage))
        {
            spv::SpvBuildLogger logger;
            glslang::SpvOptions spvOptions;
            spvOptions.stripDebugInfo   = true;
            spvOptions.disableOptimizer = true;
            spvOptions.optimizeSize     = true;
            spvOptions.disassemble      = SpvToolsDisassembler;
            spvOptions.validate         = SpvToolsValidate;
            glslang::GlslangToSpv(*program->getIntermediate((EShLanguage)stage), spirv, &logger, &spvOptions);
        }
        else
        {
            LOG_FATAL("cannot find target shader")
        }

        glslang::FinalizeProcess();

        delete shader;
        delete program;

        // copy data to unsigned char;
        // std::vector<unsigned char> spirv_char;
        // spirv_char.resize(spirv.size() * sizeof(unsigned int));
        // memcpy(spirv_char.data(), spirv.data(), spirv_char.size());

        return spirv;
    }

    std::vector<uint32_t> VulkanShaderUtils::createShaderModuleFromFile(const std::string& shader_file, std::shared_ptr<ConfigManager> config_manager, std::shared_ptr<AssetManager> asset_manager)
    {
        std::filesystem::path root_path        = config_manager->getRootFolder();
        std::filesystem::path shader_file_path = root_path / "shader" / "glsl" / shader_file;
        std::filesystem::path include_path     = root_path / "shader" / "include";
        LOG_DEBUG("open shader: " + shader_file_path.generic_string());

        std::string shader_code = "";
        // asset_manager->readTextFile(shader_file_path, shader_code);
        asset_manager->readTextFile(shader_file, shader_code);

        EShLanguage stage = shaderLanguageStageFromFileName(shader_file.c_str());

        VulkanHeaderIncluder includer;
        includer.pushExternalLocalDirectory(include_path.generic_string());

        auto client          = glslang::EShClientVulkan;
        auto client_version  = glslang::EShTargetVulkan_1_0;
        auto target_language = glslang::EShTargetSpv;
        auto target_version  = glslang::EShTargetSpv_1_0;
        auto messages        = EShMsgDefault;

        glslang::InitializeProcess();

        glslang::TProgram* program = new glslang::TProgram;
        glslang::TShader*  shader  = new glslang::TShader(stage);

        const char* file_names[]   = {shader_file.c_str()};
        const char* shader_codes[] = {shader_code.c_str()};
        shader->setStringsWithLengthsAndNames(shader_codes, NULL, file_names, 1);

        std::string              preamble_str = "";
        std::vector<std::string> processes    = {};

        shader->setPreamble(preamble_str.c_str());
        shader->addProcesses(processes);

        shader->setEnvInput(glslang::EShSourceGlsl, stage, client, 100);
        shader->setEnvClient(client, client_version);
        shader->setEnvTarget(target_language, target_version);

        const TBuiltInResource* resources      = GetDefaultResources();
        const int               defaultVersion = 100;
        std::string             str;

        if (!shader->preprocess(resources, defaultVersion, ENoProfile, false, false, messages, &str, includer))
        {
            LOG_ERROR(shader->getInfoLog());
            LOG_FATAL(shader->getInfoDebugLog());
        }

        if (!shader->parse(resources, defaultVersion, false, messages, includer))
        {
            LOG_ERROR(shader->getInfoLog());
            LOG_FATAL(shader->getInfoDebugLog());
        }

        program->addShader(shader);

        if (!program->link(messages))
        {
            LOG_ERROR(program->getInfoLog());
            LOG_FATAL(program->getInfoDebugLog());
        }

        if (!program->mapIO())
        {
            LOG_ERROR(program->getInfoLog());
            LOG_FATAL(program->getInfoDebugLog());
        }

        bool SpvToolsDisassembler = false;
        bool SpvToolsValidate     = false;

        std::vector<uint32_t> spirv;
        if (program->getIntermediate(stage))
        {
            spv::SpvBuildLogger logger;
            glslang::SpvOptions spvOptions;
            spvOptions.stripDebugInfo   = true;
            spvOptions.disableOptimizer = true;
            spvOptions.optimizeSize     = true;
            spvOptions.disassemble      = SpvToolsDisassembler;
            spvOptions.validate         = SpvToolsValidate;
            glslang::GlslangToSpv(*program->getIntermediate((EShLanguage)stage), spirv, &logger, &spvOptions);
        }
        else
        {
            LOG_FATAL("cannot find target shader");
        }

        glslang::FinalizeProcess();

        delete shader;
        delete program;

        // copy data to unsigned char;
        // std::vector<unsigned char> spirv_char;
        // spirv_char.resize(spirv.size() * sizeof(unsigned int));
        // memcpy(spirv_char.data(), spirv.data(), spirv_char.size());

        return spirv;
    }

    std::vector<uint32_t> VulkanShaderUtils::createShaderModuleFromCode(const std::string& shader_code, const std::string& shader_type, std::shared_ptr<ConfigManager> config_manager)
    {
        std::filesystem::path root_path    = config_manager->getRootFolder();
        std::filesystem::path include_path = root_path / "shader" / "include";

        EShLanguage stage = shaderLanguageStageFromFileName(shader_type.c_str());

        VulkanHeaderIncluder includer;
        includer.pushExternalLocalDirectory(include_path.generic_string());

        auto client          = glslang::EShClientVulkan;
        auto client_version  = glslang::EShTargetVulkan_1_0;
        auto target_language = glslang::EShTargetSpv;
        auto target_version  = glslang::EShTargetSpv_1_0;
        auto messages        = EShMsgDefault;

        glslang::InitializeProcess();

        glslang::TProgram* program = new glslang::TProgram;
        glslang::TShader*  shader  = new glslang::TShader(stage);

        const char* file_names[]   = {shader_type.c_str()};
        const char* shader_codes[] = {shader_code.c_str()};
        shader->setStringsWithLengthsAndNames(shader_codes, NULL, file_names, 1);

        std::string              preamble_str = "";
        std::vector<std::string> processes    = {};

        shader->setPreamble(preamble_str.c_str());
        shader->addProcesses(processes);

        shader->setEnvInput(glslang::EShSourceGlsl, stage, client, 100);
        shader->setEnvClient(client, client_version);
        shader->setEnvTarget(target_language, target_version);

        const TBuiltInResource* resources      = GetDefaultResources();
        const int               defaultVersion = 100;
        std::string             str;

        if (!shader->preprocess(resources, defaultVersion, ENoProfile, false, false, messages, &str, includer))
        {
            LOG_ERROR(shader->getInfoLog());
            LOG_FATAL(shader->getInfoDebugLog());
        }

        if (!shader->parse(resources, defaultVersion, false, messages, includer))
        {
            LOG_ERROR(shader->getInfoLog());
            LOG_FATAL(shader->getInfoDebugLog());
        }

        program->addShader(shader);

        if (!program->link(messages))
        {
            LOG_ERROR(program->getInfoLog());
            LOG_FATAL(program->getInfoDebugLog());
        }

        if (!program->mapIO())
        {
            LOG_ERROR(program->getInfoLog());
            LOG_FATAL(program->getInfoDebugLog());
        }

        bool SpvToolsDisassembler = false;
        bool SpvToolsValidate     = false;

        std::vector<uint32_t> spirv;
        if (program->getIntermediate(stage))
        {
            spv::SpvBuildLogger logger;
            glslang::SpvOptions spvOptions;
            spvOptions.stripDebugInfo   = true;
            spvOptions.disableOptimizer = true;
            spvOptions.optimizeSize     = true;
            spvOptions.disassemble      = SpvToolsDisassembler;
            spvOptions.validate         = SpvToolsValidate;
            glslang::GlslangToSpv(*program->getIntermediate((EShLanguage)stage), spirv, &logger, &spvOptions);
        }
        else
        {
            LOG_FATAL("cannot find target shader");
        }

        glslang::FinalizeProcess();

        delete shader;
        delete program;

        // copy data to unsigned char;
        // std::vector<unsigned char> spirv_char;
        // spirv_char.resize(spirv.size() * sizeof(unsigned int));
        // memcpy(spirv_char.data(), spirv.data(), spirv_char.size());

        return spirv;
    }

    VkShaderModule VulkanShaderUtils::createShaderModule(VkDevice device, const std::vector<uint32_t>& shader_code)
    {
        VkShaderModuleCreateInfo shader_module_create_info {};
        shader_module_create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_info.codeSize = shader_code.size() * sizeof(uint32_t);
        shader_module_create_info.pCode    = reinterpret_cast<const uint32_t*>(shader_code.data());

        VkShaderModule shader_module;
        if (vkCreateShaderModule(device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
        {
            return VK_NULL_HANDLE;
        }
        return shader_module;
    }
} // namespace ArchViz
