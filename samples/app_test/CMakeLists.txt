add_executable(app_test main.cpp)

fabrikk_set_target_options(app_test)

target_link_libraries(app_test PRIVATE
    fabrikk_app
)

