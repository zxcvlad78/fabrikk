#pragma once

#include <glm/glm.hpp>

namespace fabrikk::cmp {
struct Transform {
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};

    [[nodiscard]] glm::mat4 matrix() const noexcept {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position);
        m = glm::rotate(m, rotation.x, glm::vec3(1, 0, 0));
        m = glm::rotate(m, rotation.y, glm::vec3(0, 1, 0));
        m = glm::rotate(m, rotation.z, glm::vec3(0, 0, 1));
        m = glm::scale(m, scale);
        return m;
    }
};
}

// unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
// glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));