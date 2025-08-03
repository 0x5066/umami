cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 20)

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fno-omit-frame-pointer -fno-inline -DDEBUG -fno-optimize-sibling-calls -fdiagnostics-color=always")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-omit-frame-pointer -fno-inline -fno-optimize-sibling-calls -w -fdiagnostics-color=always")
endif()

project(umami)

add_executable(umami
    tinyxml2/tinyxml2.cpp
    src/skin/wasabiparser.cpp
    src/skin/skin.cpp
    src/renderer/renderer.cpp
    src/renderer/render_element.cpp
    src/renderer/render_helpers.cpp
    src/renderer/renderLayer.cpp
    src/renderer/renderText.cpp
    src/renderer/renderGroup.cpp
    src/renderer/renderStatus.cpp
    src/renderer/renderGrid.cpp
    src/renderer/render_shared.cpp
    src/renderer/renderVis.cpp
    src/renderer/renderSlider.cpp
    src/main.cpp
)

target_link_libraries(umami SDL3 SDL3_image SDL3_ttf)
