#pragma once

#include <fabrikk/cmp/Transform.hpp>
#include <fabrikk/cmp/Camera.hpp>
#include <entt/entt.hpp>
#include <fabrikk/app/AppContext.hpp>

namespace {

}

namespace fabrikk::sys {
inline void camera(entt::registry& reg) {
    auto view = reg.view<cmp::Transform, cmp::Camera>();

    auto* win = reg.ctx().get<AppContext>().window;
    if (!win) return;

    for (auto [e, tr, cam] : view.each()) {
        cam.aspect = float(win->width()) / float(win->height());
    }
}
}