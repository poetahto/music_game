cmake_minimum_required(VERSION 3.20)
project(MUSIC_GAME)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(libs/imgui)
add_subdirectory(src)