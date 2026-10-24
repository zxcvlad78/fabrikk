add_subdirectory(external/glad)
add_subdirectory(external/glfw)

add_library(fabrikk_platform STATIC
    src/Window.cpp
)

target_include_directories(fabrikk_platform PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

fabrikk_set_target_options(fabrikk_platform)

target_link_libraries(fabrikk_platform PUBLIC
    fabrikk_core
    glad
    glfw
)