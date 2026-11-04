#pragma once
#include <glm/glm.hpp>
#include <entt/entity/entity.hpp>

namespace fabrikk::cmp {
struct Viewport {
    int          width               = 1280;
    int          height              = 720;
    int          x                   = 0;
    int          y                   = 0;
    float        pixels_per_unit     = 1.0f;
    bool         y_down              = true;
    bool         clear_color_enabled = true;
    glm::vec4    clear_color         {0.08f, 0.08f, 0.10f, 1.0f};
    bool         clear_depth         = true;
    bool         use_scissor         = false;
    GLuint       framebuffer         = 0;
    entt::entity camera              = entt::null;

    [[nodiscard]] float aspect() const noexcept {
        return height > 0 ? float(width) / float(height) : 1.0f;
    }
};
}