#include "runtime/function/render/render_system.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/rhi/vulkan/vulkan_rhi.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/window/window_system.h"

#include "runtime/core/math/math.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>
#include <functional>

namespace ArchViz
{
    RenderSystem::~RenderSystem() { clear(); }

    void RenderSystem::setConfigManager(std::shared_ptr<ConfigManager> config_manager)
    {
        ASSERT(config_manager);
        m_config_manager = config_manager;
    }

    void RenderSystem::setAssetManager(std::shared_ptr<AssetManager> asset_manager)
    {
        ASSERT(asset_manager);
        m_asset_manager = asset_manager;
    }

    void RenderSystem::onMouseCallback(double x, double y)
    {
        static bool  first_mouse;
        static float last_x;
        static float last_y;

        if (first_mouse)
        {
            last_x      = x;
            last_y      = y;
            first_mouse = false;
        }

        int right = glfwGetMouseButton(m_window_system->getWindow(), GLFW_MOUSE_BUTTON_RIGHT);
        if (right)
        {
            float xoffset = x - last_x;
            float yoffset = last_y - y;
            m_render_camera->rotate(xoffset, yoffset);
        }

        last_x = x;
        last_y = y;
    }

    void RenderSystem::initialize(RenderSystemInitInfo init_info)
    {
        // render context initialize
        RHIInitInfo rhi_init_info {};
        rhi_init_info.window_system = init_info.window_system;

        m_window_system = init_info.window_system;

        m_render_camera                 = std::make_shared<RenderCamera>();
        m_render_camera->m_type         = RenderCameraType::LookAt;
        m_render_camera->m_position     = {0, 0, 2};
        m_render_camera->m_front        = {0.0f, 0.0f, 1.0f};
        m_render_camera->m_up           = {0.0f, 1.0f, 0.0f};
        m_render_camera->m_world_up     = {0.0f, 1.0f, 0.0f};
        m_render_camera->m_right        = m_render_camera->m_front.cross(m_render_camera->m_world_up);
        m_render_camera->m_move_speed   = 1.0;
        m_render_camera->m_rotate_speed = 0.1;

        m_rhi = std::make_shared<VulkanRHI>();
        m_rhi->setConfigManager(m_config_manager);
        m_rhi->setAssetManager(m_asset_manager);
        m_rhi->initialize(rhi_init_info);

        rhi_init_info.window_system->registerOnCursorPosFunc(std::bind(&RenderSystem::onMouseCallback, this, std::placeholders::_1, std::placeholders::_2));
    }

    void RenderSystem::setFPS(uint32_t fps) { m_rhi->setFPS(fps); }

    void RenderSystem::tick(float delta_time)
    {
        // process swap data between logic and render contexts
        processSwapData(delta_time);

        // prepare render command context
        m_rhi->prepareContext();

        // render context
        m_rhi->render();
    }

    void RenderSystem::processSwapData(float delta_time)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto  currentTime = std::chrono::high_resolution_clock::now();
        float time        = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        // TODO : move to input manager or scene manager
        int right = glfwGetMouseButton(m_window_system->getWindow(), GLFW_MOUSE_BUTTON_RIGHT);
        if (right)
        {
            if (glfwGetKey(m_window_system->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
                m_render_camera->move(CameraMovement::Forward, delta_time);
            if (glfwGetKey(m_window_system->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
                m_render_camera->move(CameraMovement::Backward, delta_time);
            if (glfwGetKey(m_window_system->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
                m_render_camera->move(CameraMovement::Left, delta_time);
            if (glfwGetKey(m_window_system->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
                m_render_camera->move(CameraMovement::Right, delta_time);
        }

        int width, height;
        glfwGetWindowSize(m_window_system->getWindow(), &width, &height);
        m_render_camera->m_width  = width;
        m_render_camera->m_height = height;
        m_render_camera->setPerspective(45.0f, (float)width / (float)height, 0.1, 100.0f);
        m_render_camera->setViewPort((float)width, (float)height);

        m_render_camera->update();

        FMatrix4 model {FMatrix4::Identity()};
        model.block<3, 3>(0, 0) = Eigen::AngleAxisf(time * 0.1f, FVector3::UnitZ()).toRotationMatrix();

        m_rhi->m_ubo.view  = Math::lookAt({2, 2, 2}, {0, 0, 0}, {0, 0, 1});                         // m_render_camera->m_view;
        m_rhi->m_ubo.proj  = Math::perspective(45.0f, (float)width / (float)height, 0.01f, 100.0f); // m_render_camera->m_projction;
        m_rhi->m_ubo.model = model;
        // m_rhi->m_ubo.proj(1, 1) = m_rhi->m_ubo.proj(1, 1) * -1.0f; // vulkan's screen coordinate y axis is inerted
    }

    void RenderSystem::clear()
    {
        if (m_rhi)
        {
            m_rhi->clear();
        }
        m_rhi.reset();
    }
} // namespace ArchViz
