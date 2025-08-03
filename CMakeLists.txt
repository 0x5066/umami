cmake_minimum_required(VERSION 3.5)
project(umami)

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fno-omit-frame-pointer -fno-inline -DDEBUG -fno-optimize-sibling-calls -fdiagnostics-color=always")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-omit-frame-pointer -fno-inline -fno-optimize-sibling-calls -w -fdiagnostics-color=always")
endif()

set(SOURCES
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
    PlayerCore.cpp
    WinampSaData.cpp
    main.cpp
)

option(PLUGIN "Build as shared library (DLL)" OFF)

if(PLUGIN)
    add_library(umami SHARED ${SOURCES})
    target_compile_definitions(umami PRIVATE PLUGIN=1)
    # For Windows DLLs (MinGW or MSVC), define DLL export
    target_compile_definitions(umami PRIVATE BUILDING_DLL)
else()
    add_executable(umami ${SOURCES})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "gen_") # Prevent CMake from prepending "lib" to the binary name 
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dll") # Prevent cross-compilers from adding the wrong suffix (".so") to the binary name 

target_link_libraries(umami SDL3 SDL3_image SDL3_ttf)
