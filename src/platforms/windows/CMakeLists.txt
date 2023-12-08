set(SOURCES
        platform_windows.cpp
        platform_windows_audio.cpp
        platform_windows_renderer.cpp
        ${INCLUDE_DIR}/platforms/platform_windows.hpp
)
set(LIBS
        d3d11.lib
        imgui
)
add_library(WindowsPlatform ${SOURCES})
target_include_directories(WindowsPlatform PRIVATE ${INCLUDE_DIR})
target_link_libraries(WindowsPlatform ${LIBS})