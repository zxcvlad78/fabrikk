#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <fabrikk/graphics/ShaderProgram.hpp>
#include <vector>
#include <iostream>

namespace {
    thread_local GLuint current_program = 0;
    std::unordered_map<GLuint, std::unordered_map<std::string, GLint>> loc_cache;

    GLint loc(const char* name) {
        auto& m = loc_cache[current_program];
        auto it = m.find(name);
        if (it != m.end()) return it->second;
        GLint l = glGetUniformLocation(current_program, name);
        m.emplace(name, l);
        return l;
    }
}

namespace fabrikk::ShaderProgram {
GLuint compile_shader(GLenum type, const char* src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);

    GLint ok = GL_FALSE;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len > 1 ? len : 1);
        glGetShaderInfoLog(sh, len, NULL, log.data());
        std::cerr << "[ShaderProgram] complile_shader: " << type << " failed:\n" << log.data() << "\n";
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

GLuint create(const char* vss, const char* fss) {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vss);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fss);

    GLuint sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);
    glLinkProgram(sp);

    GLint ok = GL_FALSE;
    glGetProgramiv(sp, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(sp, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len > 1 ? len : 1);
        glGetProgramInfoLog(sp, len, NULL, log.data());
        std::cerr << "[ShaderProgram] create: link failed:\n" << log.data() << "\n";
        glDeleteProgram(sp);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return sp;
}

void destroy(unsigned int program) {
    loc_cache.erase(program);
    if (current_program == program) current_program = 0;
    glDeleteProgram(program);
}

void use(unsigned int program) {
    glUseProgram(program);
    current_program = program;
}

void set_uniform(const char* n, const glm::mat4& m) {
    glUniformMatrix4fv(loc(n), 1, GL_FALSE, glm::value_ptr(m));
}
void set_uniform(const char* n, const glm::vec4& v) {
    glUniform4fv(loc(n), 1, glm::value_ptr(v));
}
void set_uniform(const char* n, const glm::vec3& v) {
    glUniform3fv(loc(n), 1, glm::value_ptr(v));
}
void set_uniform(const char* n, float v) {
    glUniform1f(loc(n), v);
}
void set_uniform(const char* n, int v) {
    glUniform1i(loc(n), v);
}
}