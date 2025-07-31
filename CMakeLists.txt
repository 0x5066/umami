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
    wasabiparser.cpp
    skin.cpp
    renderer.cpp
    render_element.cpp
    render_helpers.cpp
    renderLayer.cpp
    renderText.cpp
    renderGroup.cpp
    renderStatus.cpp
    renderGrid.cpp
    render_shared.cpp
    renderVis.cpp
    renderSlider.cpp
    main.cpp
)

target_link_libraries(umami SDL3 SDL3_image SDL3_ttf)
