#pragma once
#include <glad/glad.h>
#include <fabrikk/cmp/Viewport.hpp>
#include <fabrikk/cmp/Camera.hpp>
#include <fabrikk/cmp/Transform.hpp>
#include <fabrikk/sys/camera.hpp>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fabrikk::sys {
inline glm::mat4 view_matrix(const cmp::Transform& t) noexcept {
    return glm::inverse(t.matrix());
}
inline glm::mat4 projection_matrix(const cmp::Camera& cam, const cmp::Viewport& vp) noexcept {
    switch (cam.projection) {
    case cmp::Camera::Projection::Orthographic: {
        const float h = cam.orthographic_height * 0.5f;
        const float w = h * vp.aspect();
        return glm::ortho(-w, w, -h, h, cam.near, cam.far);
    }
    case cmp::Camera::Projection::Perspective:
    default: return glm::perspective(glm::radians(cam.fov), vp.aspect(), cam.near, cam.far);
    }
}

inline glm::mat4 projection_pixels(const cmp::Viewport& vp) noexcept {
    const float w = float(vp.width) / (vp.pixels_per_unit > 0.f ? vp.pixels_per_unit : 1.f);
    const float h = float(vp.height) / (vp.pixels_per_unit > 0.f ? vp.pixels_per_unit : 1.f);
    if (vp.y_down) return glm::ortho(0.f, w, h, 0.f, -1.f, 1.f);
    return glm::ortho(0.f, w, 0.f, h, -1.f, 1.f);
}


inline void render(entt::registry& reg) {

}

}