#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fabrikk/app/App.hpp>
#include <fabrikk/render/ShaderProgram.hpp>
#include <iostream>
#include <vector>
#include <fabrikk/render/Mesh.hpp>

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

void gl_setup() {
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main() {
    fabrikk::App app("app_test");

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

    app.simulation.emplace(
        fabrikk::SimulationPhase::Render,
        [&triangle_mesh, &triangle_mesh1]() {
            triangle_mesh->draw();
            triangle_mesh1->draw();
        },
        "app_test:render"
    );

    app.run();


    return 0;
}