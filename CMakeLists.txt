cmake_minimum_required(VERSION 3.20)
project(MUSIC_GAME)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCES
        src/main.cpp
        src/mic_reader.cpp
        ${INCLUDE_DIR}/types.hpp
        ${INCLUDE_DIR}/platform.hpp
        ${INCLUDE_DIR}/mic_reader.hpp
)

add_subdirectory(libs/imgui)
set(LIBS imgui)

if(WIN32)
    add_subdirectory(src/platforms/windows)
    list(APPEND LIBS WindowsPlatform)
endif()

add_executable(MusicGame ${SOURCES})
target_include_directories(MusicGame PRIVATE ${INCLUDE_DIR})
target_link_libraries(MusicGame ${LIBS})