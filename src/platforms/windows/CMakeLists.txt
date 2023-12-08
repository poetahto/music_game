set(SOURCES
        platform_windows.cpp
        platform_windows.hpp
        platform_windows_audio.cpp
        platform_windows_renderer.cpp
)
set(LIBS
        d3d11.lib
        imgui
)
add_library(WindowsPlatform ${SOURCES})
target_include_directories(WindowsPlatform PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(WindowsPlatform ${LIBS})