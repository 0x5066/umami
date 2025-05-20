cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "-g3")

project(umami)

add_executable(umami
    tinyxml2/tinyxml2.cpp
    wasabiparser.cpp
    layerparser.cpp
    main.cpp
)

target_link_libraries(umami -static -g3 -O0)
