set(SOURCES
        main.cpp
        mic_reader.cpp
        ${INCLUDE_DIR}/types.hpp
        ${INCLUDE_DIR}/platform.hpp
        ${INCLUDE_DIR}/mic_reader.hpp
)
set(LIBS
        imgui
)

if(WIN32)
    add_subdirectory(platforms/windows)
    list(APPEND LIBS platform_windows)
endif()

add_executable(MusicGame ${SOURCES})
target_include_directories(MusicGame PRIVATE ${INCLUDE_DIR})
target_link_libraries(MusicGame ${LIBS})