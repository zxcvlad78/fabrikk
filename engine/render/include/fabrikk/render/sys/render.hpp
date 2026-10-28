#pragma once

#include <fabrikk/ecs/cmp/Transform.hpp>
#include <fabrikk/render/cmp/Mesh.hpp>
#include <fabrikk/render/cmp/Material.hpp>

namespace fabrikk::render::sys {
void render(entt::registry& reg) {
    auto view = reg.view<cmp::Transform, cmp::Mesh, cmp::Material>();
}
}