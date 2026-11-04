#pragma once
#include <GLFW/glfw3.h>
#include <fabrikk/cmp/Viewport.hpp>
#include <fabrikk/app/AppContext.hpp>
#include <entt/entt.hpp>

namespace fabrikk::sys {
inline void viewport(entt::registry& reg) {
    auto window = reg.ctx().get<AppContext>().window;
    if (!window) return;
    int fbw = 0, fbh = 0;
    glfwGetFramebufferSize(window->handle(), &fbw, &fbh);
    if (fbw <= 0 || fbh <= 0) return;

    auto v = reg.view<cmp::Viewport>();
    for (auto [e, vp] : v.each()) {
        if (vp.framebuffer != 0) continue;
        vp.x = 0; vp.y = 0;
        vp.width  = fbw;
        vp.height = fbh;
    }
}

}