#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fabrikk::cmp {
struct Camera {
    enum class Projection { Perspective, Orthographic };
    
    Projection projection          = Projection::Perspective;
    float      fov                 = 45.f;
    float      near                = 0.1f;
    float      far                 = 1000.f;
    float      orthographic_height = 10.0f;

    [[nodiscard]] glm::mat4 view(const glm::mat4 mat) const noexcept {
        return glm::inverse(mat);
    }
};
}

