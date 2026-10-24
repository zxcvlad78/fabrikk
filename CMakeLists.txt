cmake_minimum_required(VERSION 3.25)
set(PROJECT_NAME fabrikk)
project(${PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

option(FABRIKK_BUILD_SAMPLES "Build sample applications" ON)

include(cmake/fabrikk_options.cmake)

add_subdirectory(engine)

if(FABRIKK_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
