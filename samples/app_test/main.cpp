#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fabrikk/app/App.hpp>
#include <fabrikk/sys/render.hpp>
#include <fabrikk/sys/camera.hpp>
#include <fabrikk/graphics/ShaderProgram.hpp>
#include <iostream>
#include <vector>
#include <fabrikk/ecs/SimulationContext.hpp>
#include <fabrikk/cmp/Mesh.hpp>

const char *vss =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aUV;\n"

    "uniform mat4 uModel;\n"
    "uniform mat4 uView;\n"
    "uniform mat4 uProjection;\n"

    "void main()\n"
    "{\n"
        "vec4 worldPos = uModel * vec4(aPos, 1.0);\n"
        "vNormalWS = mat3(transpose(inverse(uModel))) * aNormal;\n"
        "vUV = aUV;\n"
        "gl_Position = uProjection * uView * worldPos;\n"
    "}\0";

const char *fss =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 fuColor;\n"
    "void main()\n"
    "{\n"
        "FragColor = fuColor;\n"
    "}\0";


int main() {
    fabrikk::App app("app_test");

    auto shader_program = fabrikk::ShaderProgram::create(vss, fss);
    std::vector<float> triangle_vertices0 = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f
    };

    std::vector<float> triangle_vertices1 = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f
    };
    

    auto& reg = app.simulation.registry();
    
    auto e0 = reg.create();
    auto& triangle_tr0 = reg.emplace<fabrikk::cmp::Transform>(e0);
    auto& triangle_mesh0 = reg.emplace<fabrikk::cmp::Mesh>(e0, triangle_vertices0);
    auto& triangle_mat0 = reg.emplace<fabrikk::cmp::Material>(e0, shader_program);
    triangle_mat0.albedo_color = { 1.f, .0f, .0f, .5f };

    auto e1 = reg.create();
    auto& triangle_tr1 = reg.emplace<fabrikk::cmp::Transform>(e1);
    triangle_tr1.position.y = -0.5f;
    triangle_tr1.rotation.z = 25.f;
    auto& triangle_mesh1 = reg.emplace<fabrikk::cmp::Mesh>(e1, triangle_vertices1);
    auto& triangle_mat1 = reg.emplace<fabrikk::cmp::Material>(e1, shader_program);
    triangle_mat1.albedo_color = { 0.f, 1.f, 0.f, .5f };

    app.simulation.emplace(fabrikk::SimulationPhase::Update,{
        "fabrikk:render:camera",
        &fabrikk::sys::camera,
        0
    });

    app.simulation.emplace(fabrikk::SimulationPhase::Render,{
        "fabrikk:render:test",
        [](entt::registry& reg) {
            double dt = reg.ctx().get<fabrikk::SimulationContext>().dt;
            for(auto [e, tr, cam] : reg.view<fabrikk::cmp::Transform, fabrikk::cmp::Camera>().each()) {
                tr.position.y += static_cast<float>(0.05 * dt);
            }
            for(auto [e, tr, mesh] : reg.view<fabrikk::cmp::Transform, fabrikk::cmp::Mesh>().each()) {
                tr.position.x += static_cast<float>(0.05 * dt);
                tr.rotation.z += static_cast<float>(0.15 * dt);
            }
        },
        0
    });

    app.simulation.emplace(fabrikk::SimulationPhase::Render,{
        "fabrikk:render:render",
        &fabrikk::sys::render,
        0
    });

    app.run();


    return 0;
}
