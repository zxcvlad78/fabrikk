#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace fabrikk::cmp {

struct Sprite {
    glm::vec2 size     {1.0f, 1.0f};    // world units
    glm::vec4 tint     {1.0f, 1.0f, 1.0f, 1.0f};
    GLuint    texture  = 0;
    bool      flip_x   = false;
    bool      flip_y   = false;
};

}