add_subdirectory(external/entt)

add_library(fabrikk_ecs STATIC
    src/Simulation.cpp
)

target_include_directories(fabrikk_ecs PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

fabrikk_set_target_options(fabrikk_ecs)

target_link_libraries(fabrikk_ecs PUBLIC
    fabrikk_core
    EnTT::EnTT
)