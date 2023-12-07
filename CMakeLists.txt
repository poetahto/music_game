cmake_minimum_required(VERSION 3.20)
project(MUSIC_GAME)

set(SOURCES
    src/main.cpp
    src/platform/windows/platform_windows.cpp
    include/platform.hpp
    include/types.hpp

    src/imgui/imgui.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    include/imgui.h
    include/imconfig.h
    include/imgui_internal.h
)

if(WIN32)
    list(APPEND SOURCES 
        src/platform/windows/imgui_impl_win32.cpp
        src/platform/windows/imgui_impl_dx11.cpp
        include/platform/windows/imgui_impl_win32.h
        include/platform/windows/imgui_impl_dx11.h
    )
endif()

add_executable(MusicGame ${SOURCES})
target_include_directories(MusicGame PRIVATE ${MUSIC_GAME_SOURCE_DIR}/game)