add_library(fabrikk_cmp STATIC
    src/cmp.cpp
)

target_include_directories(fabrikk_cmp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

fabrikk_set_target_options(fabrikk_cmp)

target_link_libraries(fabrikk_cmp PUBLIC
    fabrikk_core
    fabrikk_platform
    fabrikk_ecs
)