cmake_minimum_required(VERSION 3.16)

project(MyFramework VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

option(BUILD_PLUGIN_FRAME_TESTS "Build PluginFrame unit tests" OFF)

find_package(Qt5 COMPONENTS Core Widgets Xml REQUIRED)
if(BUILD_PLUGIN_FRAME_TESTS)
    find_package(Qt5 COMPONENTS Test REQUIRED)
endif()

add_subdirectory(framework)
add_subdirectory(src)
add_subdirectory(plugins/HelloPlugin)
add_subdirectory(plugins/TimeServicePlugin)

if(BUILD_PLUGIN_FRAME_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
    enable_testing()
    add_subdirectory(tests)
endif()
