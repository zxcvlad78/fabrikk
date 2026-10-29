add_subdirectory(external/glm)

add_library(fabrikk_core STATIC
    src/core.cpp
)

target_include_directories(fabrikk_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

fabrikk_set_target_options(fabrikk_core)

target_link_libraries(fabrikk_core PUBLIC
    glm::glm
)