#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

struct camera
{
    camera();

    glm::mat4 projection(glm::vec2 viewport_size);
    glm::mat4 view();

    void move(glm::vec3 position);

    void look(float x_offset, float y_offset, bool constrain_pitch);

  private:
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 world_up_;

    float pitch_ = 0;
    float yaw_ = 0;

    void update();
};
