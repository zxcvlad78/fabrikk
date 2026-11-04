#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace fabrikk {
namespace ShaderProgram {
    unsigned int create(const char* vss, const char* fss);
    GLuint compile_shader(GLenum type, const char* src);
    void use(unsigned int program);
    void destroy(unsigned int program);
    void set_uniform(const char* n, const glm::mat4& m);
    void set_uniform(const char* n, const glm::vec3& v);
    void set_uniform(const char* n, float v);
    void set_uniform(const char* n, int v);
}
}