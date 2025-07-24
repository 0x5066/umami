cmake_minimum_required(VERSION 3.5)

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g3 -DDEBUG -fsanitize=address")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fsanitize=address")
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

target_link_libraries(umami SDL2 SDL2_image SDL2_ttf)
