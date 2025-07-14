cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "-g3")

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
    main.cpp
)

target_link_libraries(umami SDL2 SDL2_image )
