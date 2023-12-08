set(SOURCES
        platform_windows.cpp
        platform_windows_audio.cpp
        platform_windows_renderer.cpp
        ${INCLUDE_DIR}/platforms/platform_windows.hpp
)
set(LIBS
        d3d11
        imgui
)
add_library(platform_windows ${SOURCES})
target_include_directories(platform_windows PRIVATE ${INCLUDE_DIR})
target_link_libraries(platform_windows ${LIBS})