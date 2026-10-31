#pragma once

#include <fabrikk/ecs/cmp/Transform.hpp>
#include <fabrikk/render/cmp/Mesh.hpp>
#include <fabrikk/render/cmp/Material.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
void set_uniforms(
    fabrikk::ecs::cmp::Transform& tr,
    fabrikk::render::cmp::Mesh& mesh,
    fabrikk::render::cmp::Material& mat
) {
    GLint color_id = glGetUniformLocation(mat.shader_program, "fuColor");
    if (color_id >= 0) {
        glUniform4f(color_id,
            mat.albedo_color.x,
            mat.albedo_color.y,
            mat.albedo_color.z,
            mat.albedo_color.w
        );
    }

    GLint transform_id = glGetUniformLocation(mat.shader_program, "fuTransform");
    if (transform_id >= 0) {
        auto matrix = tr.matrix();
        glUniformMatrix4fv(transform_id, 1, GL_FALSE, glm::value_ptr(matrix));
    }
}
}

namespace fabrikk::sys {
inline void render(entt::registry& reg) {
    GLuint last_shader_program = 0;
    auto view = reg.view<cmp::Transform, cmp::Mesh, cmp::Material>();

    for (auto [e, tr, mesh, mat] : view.each()) {
        if (mat.shader_program == 0) continue;

        if (mesh.VAO == 0) glGenVertexArrays(1, &mesh.VAO);
        if (mesh.VBO == 0) glGenBuffers(1, &mesh.VBO);

        glBindVertexArray(mesh.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);

        if (mesh.vertices_dirty) {
            mesh.vertex_count = static_cast<int>(mesh.vertices.size() / 3);
            glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            mesh.vertices_dirty = false;
        }

        if (mesh.vertex_count <= 0) continue;

        if (mat.shader_program != last_shader_program) {
            glUseProgram(mat.shader_program);
            last_shader_program = mat.shader_program;
        }

        if (mat.blend_use) {
            glEnable(GL_BLEND);
            glBlendFunc(mat.blend_sfactor, mat.blend_dfactor);
            printf("%f\n", mat.albedo_color.a);
        } else glDisable(GL_BLEND);

        set_uniforms(tr, mesh, mat);

        glDrawArrays(GL_TRIANGLES, 0, mesh.vertex_count);
        printf("draw\n");
    }

    glBindVertexArray(0);
}
}