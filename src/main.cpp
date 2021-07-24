#include "app.hpp"
#include "camera.hpp"

#include <imgui.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <vector>

#include <ImGuizmo.h>

void compile_shaders();
void create_mesh();
void update(float delta_time);

app application;
camera main_camera;
uint32_t num_indices = 0;

void render_viewport()
{
    application.begin_render_to_framebuffer();
    {
        glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr);
    }
    auto framebuffer = application.end_render_to_framebuffer();

    ImGui::SetNextWindowPos({0, 50}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({80, 80}, ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Viewport"))
    {
        auto panel_size = ImGui::GetContentRegionAvail();
        application.resize_framebuffer_if_needed({panel_size.x, panel_size.y});

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

        ImGui::Image((void*)(size_t)framebuffer, panel_size, {0, 1}, {1, 0});

        auto view = main_camera.view();
        auto proj = main_camera.projection(application.viewport_size());
        auto model = glm::mat4(1.0f);
        ImGuizmo::Manipulate(&view[0][0], &proj[0][0], ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &model[0][0]);
    }
    ImGui::End();
}

void render(float delta_time)
{
    render_viewport();

    ImGui::SetNextWindowPos({100, 50}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({80, 80}, ImGuiCond_FirstUseEver);
    if (ImGui::Begin("panel 1"))
    {
    }
    ImGui::End();

    ImGui::SetNextWindowPos({200, 50}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({80, 80}, ImGuiCond_FirstUseEver);
    if (ImGui::Begin("panel 2"))
    {
    }
    ImGui::End();
}

int main()
{
    create_mesh();

    application.loop(
        [](float delta_time) {
            update(delta_time);
        },
        [](float delta_time) {
            render(delta_time);
        });
}

void move_camera(float delta_time)
{
    float x = 0;
    float y = 0;
    float z = 0;

    static ImVec2 last_mouse_pos{};

    auto current_mouse_pos = ImGui::GetMousePos();

    if (application.is_key_down(GLFW_KEY_W))
        x += 5.f * delta_time;
    if (application.is_key_down(GLFW_KEY_S))
        x -= 5.f * delta_time;
    if (application.is_key_down(GLFW_KEY_D))
        z += 5.f * delta_time;
    if (application.is_key_down(GLFW_KEY_A))
        z -= 5.f * delta_time;
    if (application.is_key_down(GLFW_KEY_Q))
        y += 5.f * delta_time;
    if (application.is_key_down(GLFW_KEY_E))
        y -= 5.f * delta_time;

    auto pos = glm::vec3{x, y, z};
    main_camera.move(pos);

    if (application.is_key_down(GLFW_MOUSE_BUTTON_RIGHT))
    {
        main_camera.look(current_mouse_pos.x - last_mouse_pos.x, last_mouse_pos.y - current_mouse_pos.y, true);
    }
    last_mouse_pos = current_mouse_pos;
}

void update(float delta_time)
{
    if (application.viewport_size() == glm::vec2{0, 0})
        return;

    move_camera(delta_time);

    auto view = main_camera.view();
    auto proj = main_camera.projection(application.viewport_size());
    auto model = glm::mat4(1.0f);

    glUniformMatrix4fv(0, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(1, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix4fv(2, 1, GL_FALSE, &model[0][0]);
}

void create_mesh()
{
    std::vector<uint32_t> indices = {
        // top
        0, 1, 2, //
        2, 3, 0, //

        // left
        7, 4, 0, //
        0, 3, 7, //

        // right
        1, 5, 6, //
        6, 2, 1, //

        // back
        5, 1, 0, //
        0, 4, 5, //

        // front
        3, 2, 6, //
        6, 7, 3, //

        // bottom
        6, 5, 4, //
        4, 7, 6, //
    };

    std::vector<float> vertices = {
        // top
        -0.5f, +0.5f, +0.5f, // 0
        +0.5f, +0.5f, +0.5f, // 1
        +0.5f, +0.5f, -0.5f, // 2
        -0.5f, +0.5f, -0.5f, // 3

        // bottom
        -0.5f, -0.5f, +0.5f, // 4
        +0.5f, -0.5f, +0.5f, // 5
        +0.5f, -0.5f, -0.5f, // 6
        -0.5f, -0.5f, -0.5f, // 7
    };

    uint32_t vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    uint32_t buffers[2]{};
    glGenBuffers(2, buffers);

    auto vertex_buffer = buffers[0];
    auto index_buffer = buffers[1];

    // vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    num_indices = (uint32_t)indices.size();
}
