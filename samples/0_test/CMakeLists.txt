add_executable(0_test main.cpp)

fabrikk_set_target_options(0_test)

target_link_libraries(0_test PRIVATE
    fabrikk_app
)

