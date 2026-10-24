#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fabrikk/render/ShaderProgram.hpp>
#include <vector>
#include <iostream>

namespace fabrikk {
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

GLuint ShaderProgram::create(const char* vss, const char* fss) {
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
}