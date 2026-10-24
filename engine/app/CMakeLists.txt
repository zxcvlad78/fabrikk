add_library(fabrikk_app STATIC
    src/App.cpp
)

target_include_directories(fabrikk_app PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

fabrikk_set_target_options(fabrikk_app)

target_link_libraries(fabrikk_app PUBLIC
    fabrikk_core
    fabrikk_ecs
    fabrikk_platform
    fabrikk_render
)