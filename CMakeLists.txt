cmake_minimum_required(VERSION 3.20)
project(MUSIC_GAME)

set(SOURCES
        src/main.cpp
        src/mic_reader.cpp
        include/types.hpp
        include/platform.hpp
        include/mic_reader.hpp
)

add_subdirectory(libs/imgui)
set(LIBS imgui)

if(WIN32)
    add_subdirectory(src/platforms/windows)
    list(APPEND LIBS WindowsPlatform)
endif()

add_executable(MusicGame ${SOURCES})
target_include_directories(MusicGame PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(MusicGame ${LIBS})