#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fabrikk/app/App.hpp>
#include <fabrikk/render/sys/render.hpp>
#include <fabrikk/render/ShaderProgram.hpp>
#include <iostream>
#include <vector>
#include <fabrikk/render/Mesh.hpp>
#include <fabrikk/render/cmp/Mesh.hpp>

const char *vss =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 fuTransform;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = fuTransform * vec4(aPos, 1.0f);\n"
    "}\0";

const char *fss =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 fuColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = fuColor;\n"
    "}\0";


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
    triangle_mesh1->color({1.f, 0.1f, 0.8f, 0.5f});

    auto& reg = app.simulation.registry();
    auto e0 = reg.create();
    reg.emplace<fabrikk::render::cmp::Mesh>(e0, triangle_mesh);
    auto e1 = reg.create();
    reg.emplace<fabrikk::render::cmp::Mesh>(e1, triangle_mesh1);

    app.simulation.emplace(fabrikk::SimulationPhase::Render,{
        "fabrikk:render:render",
        &fabrikk::render::sys::render,
        0
    });

    app.run();


    return 0;
}
