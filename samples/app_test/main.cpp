#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fabrikk/app/App.hpp>
#include <fabrikk/render/sys/render.hpp>
#include <fabrikk/render/ShaderProgram.hpp>
#include <iostream>
#include <vector>
#include <fabrikk/ecs/ctx/SimulationContext.hpp>
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
    std::vector<float> triangle_vertices1 = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f
    };
    

    auto& reg = app.simulation.registry();
    
    auto e0 = reg.create();
    auto& triangle_tr1 = reg.emplace<fabrikk::ecs::cmp::Transform>(e0);
    auto& triangle_mesh1 = reg.emplace<fabrikk::render::cmp::Mesh>(e0, triangle_vertices1);
    auto& triangle_mat1 = reg.emplace<fabrikk::render::cmp::Material>(e0, shader_program);
    triangle_mat1.albedo_color.a = 0.5f;

    app.simulation.emplace(fabrikk::SimulationPhase::Render,{
        "fabrikk:render:render",
        &fabrikk::render::sys::render,
        0
    });

    app.simulation.emplace(fabrikk::SimulationPhase::Render,{
        "fabrikk:render:test",
        [](entt::registry& reg) {
            double dt = reg.ctx().get<fabrikk::ctx::SimulationContext>().dt;
            for(auto [e, tr] : reg.view<fabrikk::ecs::cmp::Transform>().each()) {
                tr.position.x += 0.1 * dt;
                tr.rotation.z += 1.0 * dt;
            }
            for(auto [e, mat] : reg.view<fabrikk::render::cmp::Material>().each()) {
                //mat.albedo_color.w -= 0.1 * dt;
            }
        },
        0
    });

    app.run();


    return 0;
}
