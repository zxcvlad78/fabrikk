#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace fabrikk::render::cmp {
struct Material {
    GLuint    shader_program = 0;

    glm::vec4 albedo_color     {1.f};
    GLuint    albedo_texture = 0;

    bool      blend_use =      true;
    GLenum    blend_sfactor =  GL_SRC_COLOR;
    GLenum    blend_dfactor =  GL_ONE_MINUS_SRC_ALPHA;
};
}