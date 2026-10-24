#pragma once
#include <glad/glad.h>
#include <vector>
#include <fabrikk/core/math.hpp>

namespace fabrikk {
class Mesh {
private:
    unsigned int VAO_ = 0;
    unsigned int VBO_ = 0;
    GLsizei      vertex_count_ = 0;
    GLuint       shader_program_ = 0;
    vec4         color_ = {1.f, 1.f, 1.f, 1.f};
    bool         blend_use_ = true;
    GLenum       blend_sfactor_ = GL_SRC_COLOR;
    GLenum       blend_dfactor_ = GL_ONE_MINUS_SRC_ALPHA;

    void use_shader() const;
public:
    Mesh(const std::vector<float>& vertices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void shader_program(GLuint sp);
    GLuint shader_program() const;

    void color(const vec4& color);
    vec4 color() const;

    void blend_use(const bool blend);
    bool blend_use() const;

    void blend_sfactor(const GLenum factor);
    GLenum blend_sfactor() const;

    void blend_dfactor(const GLenum factor);
    GLenum blend_dfactor() const;

    void draw() const;
};
}