#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fabrikk::cmp {
struct Camera {
    float fov    = 45.f;
    float near   = 0.1f;
    float far    = 1000.f;
    float aspect = 0.f;

    [[nodiscard]] glm::mat4 projection() const noexcept {
        return glm::perspective(glm::radians(fov), aspect, near, far);
    }

    [[nodiscard]] glm::mat4 view(const glm::mat4 mat) const noexcept {
        return glm::inverse(mat);
    }
};
}

