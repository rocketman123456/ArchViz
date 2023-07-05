#pragma once

#include <filesystem>
#include <unordered_map>

namespace ArchViz
{
    struct EngineInitParams;

    class ConfigManager
    {
    public:
        void initialize(const std::filesystem::path& config_file_path);
        void clear();
        void reload();

        const std::filesystem::path& getRootFolder() const;
        const std::filesystem::path& getAssetFolder() const;
        const std::filesystem::path& getTemplateFolder() const;
        const std::filesystem::path& getEditorBigIconPath() const;
        const std::filesystem::path& getEditorSmallIconPath() const;
        const std::filesystem::path& getEditorFontPath() const;

        const std::string& getDefaultWorldUrl() const;
        const std::string& getGlobalRenderingResUrl() const;
        const std::string& getGlobalParticleResUrl() const;

    private:
        void setAssetConfig(const std::string& name, const std::string& value);
        void setKeyConfig(const std::string& name, const std::string& value);

    private:
        const std::string ASSET_CONFIG     = "[asset]";
        const std::string KEY_CONFIG       = "[key]";
        std::string       m_current_config = ASSET_CONFIG;

        // TODO : use map
        std::filesystem::path m_config_file_path;
        std::filesystem::path m_root_folder;
        std::filesystem::path m_asset_folder;
        std::filesystem::path m_template_folder;
        std::filesystem::path m_editor_big_icon_path;
        std::filesystem::path m_editor_small_icon_path;
        std::filesystem::path m_editor_font_path;

        std::string m_default_world_url;
        std::string m_global_rendering_res_url;
        std::string m_global_particle_res_url;

        std::unordered_map<std::string, std::string> m_key_binding;
    };
} // namespace ArchViz
