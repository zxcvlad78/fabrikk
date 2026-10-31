add_library(fabrikk_sys STATIC
    src/sys.cpp
)

target_include_directories(fabrikk_sys PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

fabrikk_set_target_options(fabrikk_sys)

target_link_libraries(fabrikk_sys PUBLIC
    fabrikk_ecs
    fabrikk_cmp
)