find_package(OpenGL REQUIRED)

add_library(fabrikk_graphics STATIC
    src/ShaderProgram.cpp
)

target_include_directories(fabrikk_graphics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

fabrikk_set_target_options(fabrikk_graphics)

target_link_libraries(fabrikk_graphics PUBLIC
    fabrikk_core
    fabrikk_platform
    OpenGL::GL
)
