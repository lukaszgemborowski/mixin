cmake_minimum_required(VERSION 3.2)
project(mixin)
set(project-root ${CMAKE_CURRENT_SOURCE_DIR})

add_library(mixin INTERFACE)
target_include_directories(mixin INTERFACE include/)
target_compile_features(mixin INTERFACE cxx_std_17)


add_subdirectory(tests)
