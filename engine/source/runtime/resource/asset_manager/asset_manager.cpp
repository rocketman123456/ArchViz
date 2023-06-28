#include "runtime/resource/asset_manager/asset_manager.h"

#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/platform/file_system/vfs.h"

#include <filesystem>

namespace ArchViz
{
    void AssetManager::setVFS(std::shared_ptr<VFS> vfs)
    {
        m_vfs = vfs;
        ASSERT(m_vfs);
    }

    void AssetManager::setConfigManager(std::shared_ptr<ConfigManager> config_manager)
    {
        m_config_manager = config_manager;
        ASSERT(m_config_manager);
    }

    std::filesystem::path AssetManager::getFullPath(const std::string& relative_path) const
    {
        return std::filesystem::absolute(m_config_manager->getRootFolder() / relative_path);
    }

    void AssetManager::readTextFile(const std::filesystem::path& file_path, std::string& content) const
    {
        std::ifstream fin(file_path, std::ios::in);
        if (!fin)
        {
            LOG_ERROR("open file: {} failed!", file_path.generic_string());
            return;
        }
        content = {std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>()};
    }

    void AssetManager::readBinaryFile(const std::filesystem::path& file_path, std::vector<std::byte>& content) const
    {
        std::ifstream fin(file_path, std::ios::in | std::ios::binary);
        if (!fin)
        {
            LOG_ERROR("open file: {} failed!", file_path.generic_string());
            return;
        }

        size_t file_size = fin.tellg();
        content.resize(file_size);

        fin.seekg(0);
        fin.read(reinterpret_cast<char*>(content.data()), sizeof(file_size));
        fin.close();
    }

    void AssetManager::readVFSTextFile(const std::filesystem::path& file_path, std::string& content) const
    {
        auto file = m_vfs->open(file_path.string(), File::read_text);
        file->read(content);
        file->close();
    }

    void AssetManager::readVFSBinaryFile(const std::filesystem::path& file_path, std::vector<std::byte>& content) const
    {
        auto file = m_vfs->open(file_path.string(), File::read_bin);
        file->read(content);
        file->close();
    }

    void AssetManager::writeVFSTextFile(const std::filesystem::path& file_path, const std::string& content) const
    {
        auto file = m_vfs->open(file_path.string(), File::write_text);
        file->write(content);
        file->close();
    }

    void AssetManager::writeVFSBinaryFile(const std::filesystem::path& file_path, const std::vector<std::byte>& content) const
    {
        auto file = m_vfs->open(file_path.string(), File::write_bin);
        file->write(content);
        file->close();
    }
} // namespace ArchViz