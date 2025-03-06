cmake_minimum_required(VERSION 3.5)

project(umami)

add_executable(umami
    tinyxml2/tinyxml2.cpp
    main.cpp)

target_link_libraries(umami -static)
