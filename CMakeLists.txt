cmake_minimum_required(VERSION 3.20)
project(MUSIC_GAME)

set(MG_DIR ${MUSIC_GAME_SOURCE_DIR})
set(MG_SOURCES
        ${MG_DIR}/src/main.cpp
        ${MG_DIR}/src/platform/platform_windows.cpp
        ${MG_DIR}/include/types.hpp
        ${MG_DIR}/include/platform.hpp
)
set(MG_LIBS imgui)

if(WIN32)
    list(APPEND MG_LIBS d3d11.lib)
endif()

include(libs/imgui.cmake) # todo: i think this causes lots or weirdness...

add_executable(MusicGame ${MG_SOURCES})
target_include_directories(MusicGame PRIVATE ${MG_DIR}/include ${MG_DIR}/libs) # todo: fix scuff
target_link_libraries(MusicGame ${MG_LIBS})