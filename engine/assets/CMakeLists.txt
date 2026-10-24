add_subdirectory(external/glm)

add_library(fabrikk_assets STATIC
    #src/gltf.cpp
    src/pak.cpp
    #src/FileWatcher.cpp
)

target_include_directories(fabrikk_assets PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(fabrikk_assets SYSTEM PRIVATE
    ${CGLTF_INCLUDE_DIRS}
    ${Stb_INCLUDE_DIR}
)

fabrikk_set_target_options(fabrikk_assets)

target_link_libraries(fabrikk_assets
    PUBLIC
        fabrikk_core
        fabrikk_render
)
