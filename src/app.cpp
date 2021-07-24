#include "app.hpp"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>

#include <ImGuizmo.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

app::app()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto* monitor = glfwGetPrimaryMonitor();
    auto* mode = glfwGetVideoMode(monitor);

    window_ = glfwCreateWindow(mode->width, mode->height, "imguizmo_test", nullptr, nullptr);

    glfwSetWindowUserPointer(window_, this);
    glfwMaximizeWindow(window_);
    glfwMakeContextCurrent(window_);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    gladLoadGL();

    int width = 0, height = 0;
    glfwGetWindowSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    width_ = (uint32_t)width;
    height_ = (uint32_t)height;

    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
        auto self = (app*)glfwGetWindowUserPointer(glfwGetCurrentContext());

        self->width_ = (uint32_t)width;
        self->height_ = (uint32_t)height;
        self->init_framebuffer();

        glViewport(0, 0, width, height);
    });

    glClearColor(0.27f, 0.33f, 0.47f, 1.0f);

    init_imgui();
    init_shader();
    init_framebuffer();

    last_time_ = (float)glfwGetTime();
}

app::~app()
{
    if (framebuffer_)
    {
        glDeleteTextures(1, &depth_attachment_);
        glDeleteTextures(1, &color_attachment_);
        glDeleteFramebuffers(1, &framebuffer_);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

glm::vec2 app::viewport_size()
{
    return {viewport_width_, viewport_height_};
}

void app::resize_framebuffer_if_needed(glm::vec2 size)
{
    if (size.x != viewport_width_ || size.y != viewport_height_)
    {
        viewport_width_ = size.x;
        viewport_height_ = size.y;
        init_framebuffer();
    }
}

void app::begin_render_to_framebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

uint32_t app::end_render_to_framebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return color_attachment_;
}

bool app::is_key_down(int key)
{
    if (key == GLFW_MOUSE_BUTTON_RIGHT)
        return glfwGetMouseButton(window_, key) == GLFW_PRESS;
    return glfwGetKey(window_, key) == GLFW_PRESS;
}

void app::init_imgui()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consolab.ttf", 14);
    // io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consola.ttf", 14);

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void app::init_shader()
{
    auto vs_code = R"(
#version 330 core
layout (location = 0) in vec3 in_pos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(in_pos, 1.0);
})";

    auto ps_code = R"(
#version 330 core
out vec4 out_color;

void main()
{
    out_color = vec4(0.8f, 0.8f, 0.8f, 1.0f);
})";

    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader, 1, &vs_code, nullptr);
    glShaderSource(fragment_shader, 1, &ps_code, nullptr);

    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    auto program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);
    glUseProgram(program);
}

void app::init_framebuffer()
{
    if (framebuffer_)
    {
        glDeleteTextures(1, &depth_attachment_);
        glDeleteTextures(1, &color_attachment_);
        glDeleteFramebuffers(1, &framebuffer_);
    }

    glCreateFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

    glCreateTextures(GL_TEXTURE_2D, 1, &color_attachment_);
    glBindTexture(GL_TEXTURE_2D, color_attachment_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (uint32_t)viewport_width_, (uint32_t)viewport_height_, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_, 0);

    glCreateTextures(GL_TEXTURE_2D, 1, &depth_attachment_);
    glBindTexture(GL_TEXTURE_2D, depth_attachment_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, (uint32_t)viewport_width_, (uint32_t)viewport_height_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_, 0);

    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
}

bool app::keep_running()
{
    return !glfwWindowShouldClose(window_);
}

float app::begin_loop()
{
    float current_time = static_cast<float>(glfwGetTime());
    float delta_time = current_time - last_time_;
    last_time_ = current_time;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGui::DockSpaceOverViewport();

    return delta_time;
}

void app::end_loop()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        auto* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window_);
    glfwPollEvents();
}
