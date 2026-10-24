#include <fabrikk/render/Mesh.hpp>


namespace fabrikk {
void Mesh::use_shader() const {
    if (shader_program_ == 0) return;
    glUseProgram(shader_program_);
    GLint color_i = glGetUniformLocation(shader_program_, "uColor");
    if (color_i < 0) return; 
    glUniform4f(color_i, color_.x, color_.y, color_.z, color_.w);
}

Mesh::Mesh(const std::vector<float>& vertices) {
    vertex_count_ = static_cast<int>(vertices.size() / 3);

    glGenVertexArrays(1, &VAO_);  
    glGenBuffers(1, &VBO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::~Mesh() {
    if (VBO_ != 0) glDeleteBuffers(1, &VBO_);
    if (VAO_ != 0) glDeleteVertexArrays(1, &VAO_);
}

Mesh::Mesh(Mesh&& other) noexcept 
: VAO_(std::exchange(other.VAO_, 0)),
  VBO_(std::exchange(other.VBO_, 0)),
  vertex_count_(std::exchange(other.vertex_count_, 0)),
  color_(std::exchange(other.color_, vec4{1.f, 1.f, 1.f, 1.f}))
{ }

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        if (VBO_ != 0) glDeleteBuffers(1, &VBO_);
        if (VAO_ != 0) glDeleteVertexArrays(1, &VAO_);

        VAO_ = std::exchange(other.VAO_, 0);
        VBO_ = std::exchange(other.VBO_, 0);
        vertex_count_ = std::exchange(other.vertex_count_, 0);
        color_ = std::exchange(other.color_, vec4{0.0f});
    }
    return *this;
}

void Mesh::draw() const {
    if (use_blend_) { 
        glEnable(GL_BLEND);
    }
    else glDisable(GL_BLEND);
    use_shader();
    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count_);
    glBindVertexArray(0);
}

void Mesh::shader_program(GLuint sp) { shader_program_ = sp; }
GLuint Mesh::shader_program() const { return shader_program_; }

void Mesh::color(const vec4& c) { color_ = c; }
vec4 Mesh::color() const { return color_; }

void Mesh::use_blend(const bool blend) { use_blend_ = blend; }
bool Mesh::use_blend() const { return use_blend_; }
}