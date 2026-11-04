#pragma once

inline constexpr const char* kFabrikkDefaultVertexShader = R"(#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
)";

inline constexpr const char* kFabrikkDefaultFragmentShader = R"(#version 330 core
uniform vec4 uBaseColor;
out vec4 FragColor;
void main() {
    FragColor = uBaseColor;
}
)";