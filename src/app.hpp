#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include <cstdint>
#include <functional>

struct GLFWwindow;

struct app
{
    app();
    ~app();

    template <typename UPDATE_FUNC, typename RENDER_FUNC>
    void loop(UPDATE_FUNC&& update_func, RENDER_FUNC&& render_func)
    {
        while (keep_running())
        {
            auto time_delta = begin_loop();
            update_func(time_delta);
            render_func(time_delta);
            end_loop();
        }
    }

    glm::vec2 viewport_size();

    void resize_framebuffer_if_needed(glm::vec2 size);

    void begin_render_to_framebuffer();
    uint32_t end_render_to_framebuffer();

    bool is_key_down(int key);

  private:
    void init_imgui();
    void init_shader();

    void init_framebuffer();

    bool keep_running();
    float begin_loop();
    void end_loop();

    float last_time_ = 0;

    GLFWwindow* window_;

    uint32_t width_ = 0;
    uint32_t height_ = 0;

    float viewport_width_ = 0.f;
    float viewport_height_ = 0.f;

    uint32_t framebuffer_ = 0;
    uint32_t color_attachment_ = 0;
    uint32_t depth_attachment_ = 0;
};
