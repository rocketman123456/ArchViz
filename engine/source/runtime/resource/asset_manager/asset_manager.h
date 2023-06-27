#pragma once

#include "runtime/core/base/macro.h"
#include "runtime/core/meta/serializer/serializer.h"
#include "runtime/platform/file_system/vfs.h"

#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "_generated/serializer/all_serializer.h"

namespace ArchViz
{
    class AssetManager
    {
    public:
        template<typename AssetType>
        bool loadAsset(const std::string& asset_url, AssetType& out_asset) const
        {
            // read json file to string
            std::filesystem::path asset_path = getFullPath(asset_url);
            std::string           asset_json_text;
            readTextFile(asset_path, asset_json_text);

            // parse to json object and read to runtime res object
            std::string error;
            auto&&      asset_json = Json::parse(asset_json_text, error);
            if (!error.empty())
            {
                LOG_ERROR("parse json file {} failed!", asset_url);
                return false;
            }

            Serializer::read(asset_json, out_asset);
            return true;
        }

        template<typename AssetType>
        bool saveAsset(const AssetType& out_asset, const std::string& asset_url) const
        {
            std::ofstream asset_json_file(getFullPath(asset_url));
            if (!asset_json_file)
            {
                LOG_ERROR("open file {} failed!", asset_url);
                return false;
            }

            // write to json object and dump to string
            auto&&        asset_json      = Serializer::write(out_asset);
            std::string&& asset_json_text = asset_json.dump();

            // write to file
            asset_json_file << asset_json_text;
            asset_json_file.flush();
            return true;
        }

        template<typename AssetType>
        bool loadVFSAsset(const std::string& asset_url, AssetType& out_asset) const
        {
            // read json file to string
            std::string asset_json_text;
            readVFSTextFile(asset_url, asset_json_text);

            // parse to json object and read to runtime res object
            std::string error;
            auto&&      asset_json = Json::parse(asset_json_text, error);
            if (!error.empty())
            {
                LOG_ERROR("parse json file {} failed!", asset_url);
                return false;
            }

            Serializer::read(asset_json, out_asset);
            return true;
        }

        template<typename AssetType>
        bool saveVFSAsset(const AssetType& out_asset, const std::string& asset_url) const
        {
            // write to json object and dump to string
            auto&&        asset_json      = Serializer::write(out_asset);
            std::string&& asset_json_text = asset_json.dump();

            // write to file
            writeVFSTextFile(asset_url, asset_json_text);
            return true;
        }

        void readTextFile(const std::filesystem::path& file_path, std::string& content) const;
        void readBinaryFile(const std::filesystem::path& file_path, std::vector<std::byte>& content) const;

        void readVFSTextFile(const std::filesystem::path& file_path, std::string& content) const;
        void readVFSBinaryFile(const std::filesystem::path& file_path, std::vector<std::byte>& content) const;
        void writeVFSTextFile(const std::filesystem::path& file_path, const std::string& content) const;
        void writeVFSBinaryFile(const std::filesystem::path& file_path, const std::vector<std::byte>& content) const;

        std::filesystem::path getFullPath(const std::string& relative_path) const;

    public:
        void setConfigManager(std::shared_ptr<ConfigManager> config_manager);
        void setVFS(std::shared_ptr<VFS> vfs);

    private:
        std::shared_ptr<ConfigManager> m_config_manager;
        std::shared_ptr<VFS>           m_vfs;
    };
} // namespace ArchViz
