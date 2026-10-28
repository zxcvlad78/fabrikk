#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace fabrikk::render::cmp {
struct Material {
    GLuint shader_program;
    glm::vec4 albedo_color;
    GLuint albedo_texture;
};
}