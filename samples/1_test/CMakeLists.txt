add_executable(1_test main.cpp)

fabrikk_set_target_options(1_test)

target_link_libraries(1_test PRIVATE
    fabrikk_render
)

