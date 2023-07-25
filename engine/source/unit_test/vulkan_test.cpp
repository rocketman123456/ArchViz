#include "runtime/platform/file_system/vfs.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/framework/world/world_manager.h"

#include "runtime/function/window/window_system.h"

#include "runtime/function/render/render_system.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>

using namespace ArchViz;
using namespace std;

int main(int argc, char** argv)
{
    std::filesystem::path executable_path(argv[0]);
    std::filesystem::path config_file_path = executable_path.parent_path() / "../ArchVizEditor.ini";

    std::shared_ptr<ConfigManager> config_manager = std::make_shared<ConfigManager>();
    config_manager->initialize(config_file_path.generic_string());

    std::shared_ptr<AssetManager> asset_manager = std::make_shared<AssetManager>();
    asset_manager->setConfigManager(config_manager);

    VFSConfig config;
    asset_manager->loadAsset<VFSConfig>("config/config.vfs.json", config);
    std::shared_ptr<VFS> vfs = std::make_shared<VFS>();
    vfs->mount(config);

    asset_manager->setVFS(vfs);

    

    std::shared_ptr<WindowSystem> window_system = std::make_shared<WindowSystem>();
    WindowCreateInfo window_create_info;
    asset_manager->loadAsset<WindowCreateInfo>("config/config.window.json", window_create_info);
    window_system->initialize(window_create_info);

    std::shared_ptr<RenderSystem> render_system = std::make_shared<RenderSystem>();
    RenderSystemInitInfo render_init_info;
    render_init_info.window_system = window_system;
    render_system->setConfigManager(config_manager);
    render_system->setAssetManager(asset_manager);
    render_system->initialize(render_init_info);

    using namespace std::chrono;

    steady_clock::time_point last_time_point = steady_clock::now();
    steady_clock::time_point current_time_point;

    float    fps_alpha        = 1.f / 1000;
    uint64_t frame_count      = 0;
    float    average_duration = 0;
    uint64_t update_interval  = 30;

    while (!window_system->shouldClose())
    {
        current_time_point         = steady_clock::now();
        duration<float> time_span  = duration_cast<duration<float>>(current_time_point - last_time_point);
        float           delta_time = time_span.count();
        last_time_point            = current_time_point;

        frame_count++;
        if (frame_count == 1)
        {
            average_duration = delta_time;
        }
        else
        {
            average_duration = average_duration * (1 - fps_alpha) + delta_time * fps_alpha;
        }
        uint32_t fps = static_cast<uint32_t>(1.f / average_duration);
        frame_count++;

        window_system->pollEvents();

        if (frame_count % update_interval == 0)
        {
            render_system->setFPS(fps);
        }

        render_system->tick(delta_time);
    }

    return 0;
}
