cmake_minimum_required(VERSION 3.20)
project(MUSIC_GAME)

set(MG_DIR ${MUSIC_GAME_SOURCE_DIR})
set(MG_SOURCES
    ${MG_DIR}/src/main.cpp
    ${MG_DIR}/src/platform_windows.cpp
    ${MG_DIR}/include/types.hpp
    ${MG_DIR}/include/platform.hpp
)

include(libs/imgui.cmake)
add_executable(MusicGame ${MG_SOURCES})
target_include_directories(MusicGame PRIVATE ${MG_DIR}/include)
target_link_libraries(MusicGame imgui)