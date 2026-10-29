#pragma once
#include <vector>

namespace fabrikk::render::cmp {
struct Mesh {
    std::vector<float> vertices;
    bool               vertices_dirty = true;
    unsigned int       VAO =            0;
    unsigned int       VBO =            0;
    GLsizei            vertex_count =  -1;

};
}