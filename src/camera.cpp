#include "camera.hpp"

#include <glm/ext.hpp>

#include <algorithm>

camera::camera()
  : position_{-5, 0, 0}
  , front_{0, 0, -1}
  , world_up_{0, 1, 0}
  , up_{0, 1, 0}
  , yaw_{0}
  , pitch_{0}
{
    update();
}

glm::mat4 camera::projection(glm::vec2 viewport_size)
{
    return glm::perspective(45.f, viewport_size.x / viewport_size.y, 1.f, 100.f);
}

glm::mat4 camera::view()
{
    return glm::lookAt(position_, position_ + front_, up_);
}

void camera::move(glm::vec3 position)
{
    position_ += front_ * position.x;
    position_ += right_ * position.z;
    position_ += up_ * position.y;
}

void camera::look(float x_offset, float y_offset, bool constrain_pitch)
{
    constexpr float sensitivity = 0.5;
    yaw_ += x_offset * sensitivity;
    pitch_ += y_offset * sensitivity;

    if (constrain_pitch)
        pitch_ = std::clamp(pitch_, -89.f, 89.f);

    update();
}

void camera::update()
{
    auto x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    auto y = sin(glm::radians(pitch_));
    auto z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(glm::vec3{x, y, z});

    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
