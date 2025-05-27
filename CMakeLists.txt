cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "-g3 -static")

project(umami)

add_executable(umami
    tinyxml2/tinyxml2.cpp
    wasabiparser.cpp
    main.cpp
)

target_link_libraries(umami)
