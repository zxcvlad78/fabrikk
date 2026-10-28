find_package(OpenGL REQUIRED)

add_library(fabrikk_render STATIC
    src/ShaderProgram.cpp
)

target_include_directories(fabrikk_render PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

fabrikk_set_target_options(fabrikk_render)

target_link_libraries(fabrikk_render PUBLIC
    fabrikk_core
    fabrikk_platform
    EnTT::EnTT
    OpenGL::GL
)
