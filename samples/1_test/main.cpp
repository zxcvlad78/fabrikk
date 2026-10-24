#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fabrikk/render/ShaderProgram.hpp>
#include <iostream>
#include <vector>
#include <fabrikk/render/Mesh.hpp>


float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

float vertices1[] = {
    0.3f, -0.25f, 0.0f,
    0.8f, -0.25f, 0.0f,
    0.55f, 0.25f, 0.0f
};

const char *vss =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fss =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 uColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = uColor;\n"
    "}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
    
}

int main() {
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "LearningOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, 1280, 720);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    auto shader_program = fabrikk::ShaderProgram::create(vss, fss);

    auto triangle_mesh = std::make_shared<fabrikk::Mesh>(std::vector<float>{
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    });
    triangle_mesh->shader_program(shader_program);
    triangle_mesh->color({1.f, 0.5f, 0.4f,  0.5f});

    auto triangle_mesh1 = std::make_shared<fabrikk::Mesh>(std::vector<float>{
     0.0f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f
    });
    triangle_mesh1->shader_program(shader_program);
    triangle_mesh1->color({1.f, 0.1f, 0.8f, 0.78f});

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        triangle_mesh->draw();
        triangle_mesh1->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}